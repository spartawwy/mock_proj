
#include "inflection_sell_task.h"

#include <TLib/core/tsystem_utility_functions.h>
#include "common.h"
#include "winner_app.h"

InflectionSellTask::InflectionSellTask(T_TaskInformation &task_info, WinnerApp *app)
    : StrategyTask(task_info, app)
    , time_point_open_warning_(0)
    , time_point_top_price_(0)
    , top_price_(0.0)
    , time_point_half_ready_(0)
{

}

void InflectionSellTask::HandleQuoteData()
{
    static auto get_sell_inflect_percent = [](double alert, double top, double cur)->double
    {
        double percent_inflect = 0.0; 
            percent_inflect =  (top - cur) * 100 / alert; 
        return percent_inflect;
    };

    if( is_waitting_removed_ )
        return;
     
    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);

    double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;

    if( IsPriceJumpUp(pre_price, iter->cur_price) )
    {
        app_->local_logger().LogLocal(
                    TSystem::utility::FormatStr("%d InflectionSellTask price jump %.2f to %.2f", para_.id, pre_price, iter->cur_price));
        return;
    }

    bool is_to_send = false;
    bool is_first_open = false;
    if( time_point_open_warning_ != 0 ) // if warning has been opened
    { 
        if( iter->cur_price > top_price_ )
        {
            top_price_ = iter->cur_price;
            time_point_top_price_ = iter->time_stamp;
            app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionSellTask set top:%.2f ",  para_.id, top_price_));
        }
         
    }else if( iter->cur_price > 0 && iter->cur_price > para_.alert_price && pre_price > para_.alert_price )
    {
        is_first_open = true;
        time_point_open_warning_ = time_point_top_price_ = iter->time_stamp; // open warning
        top_price_ = iter->cur_price; 
        app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionSellTask first open set top:%.2f ",  para_.id, top_price_));
    }

    bool is_in_trigger_area = false;

    // if not first open and if reback > percent or reback to alert price
    if( time_point_open_warning_ != 0 && !is_first_open )
    {
        if( iter->cur_price < top_price_ ) // inflect
        {
            // if reback to alert price
            if( para_.back_alert_trigger 
                && (iter->cur_price < para_.alert_price || Equal(iter->cur_price, para_.alert_price)) 
                //&& (para_.continue_second > 0 ? (iter->time_stamp - time_point_top_price_ > para_.continue_second) : true) 
                )
            {
                is_in_trigger_area = true; 
                app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionSellTask back trig %s %.2f %d | top:%.2f %d s ", para_.id, this->code_data(), iter->cur_price, para_.quantity, top_price_, para_.continue_second)); 
            
            }
            // if retreat_percent > rebounce_percent 
            else if( (top_price_ - iter->cur_price) * 100 / para_.alert_price > para_.rebounce
               // && (para_.continue_second > 0 ? (iter->time_stamp - time_point_top_price_ > para_.continue_second) : true)
                ) 
            {
                is_in_trigger_area = true; 
                app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionSellTask %s %.2f %d | top:%.2f %d s| infle %.2f", para_.id, this->code_data(), iter->cur_price, para_.quantity, top_price_, para_.continue_second, (top_price_ - iter->cur_price) * 100 / para_.alert_price)); 
            
            }

            if( is_in_trigger_area )
            {
                if( time_point_half_ready_ == 0 )
                {
                    time_point_half_ready_ = iter->time_stamp;
                } 
            }
        }
    }

    if( !is_in_trigger_area )
    {
        time_point_half_ready_ = 0;
    }
    
    if( para_.continue_second == 0 )
    {
        if( is_in_trigger_area ) 
            is_to_send = true;
    }else if( time_point_half_ready_ != 0 && iter->time_stamp - time_point_half_ready_ >= para_.continue_second )
    {
        is_to_send = true;
    }

    if( is_to_send ) 
    {  
        is_waitting_removed_ = true;
        time_point_open_warning_ = 0; //reset

        app_->trade_strand().PostTask([iter, this]()
        {
            // send order 
        char result[1024] = {0};
        char error_info[1024] = {0};
	      
        // to choice price to sell
        const auto price = GetQuoteTargetPrice(*iter, false);
        int qty = HandleSellByStockPosition(price);
        if( qty == 0 )
            return;
#ifdef USE_TRADE_FLAG
        assert(this->app_->trade_agent().account_data(market_type_));

        auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
        this->app_->local_logger().LogLocal(TagOfOrderLog(), 
            TSystem::utility::FormatStr("触发任务:%d 拐点卖出 %s 价格:%f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, qty, this->para_.continue_second)); 
        this->app_->AppendLog2Ui("触发任务:%d 拐点卖出 %s 价格:%f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, qty, this->para_.continue_second);
        // sell the stock
        this->app_->trade_agent().SendOrder(this->app_->trade_client_id()
            , (int)TypeOrderCategory::SELL, 0
            , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
            , price, qty
            , result, error_info); 
#endif
        // judge result 
        if( strlen(error_info) > 0 )
        {
            auto ret_str = new std::string(utility::FormatStr("error %d 拐点卖出 %s %f %d fail:%s"
                        , para_.id, para_.stock.c_str(), price, qty, error_info));

            this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
            this->app_->AppendLog2Ui(ret_str->c_str());
            this->app_->EmitSigShowUi(ret_str, true);
        }else
        {
            this->app_->SubPosition(para_.stock, qty);
            auto ret_str = new std::string(utility::FormatStr("任务:%d 拐点卖出 %s %.2f %d 成功!", para_.id, para_.stock.c_str(), price, qty));
            this->app_->EmitSigShowUi(ret_str, true);
        }

        this->app_->RemoveTask(this->task_id(), TypeTask::INFLECTION_SELL);

        });
            
    }  
}