#include "inflection_buy_task.h" 

#include <TLib/core/tsystem_utility_functions.h>
#include "common.h"
#include "winner_app.h"

InflectionBuyTask::InflectionBuyTask(T_TaskInformation &task_info, WinnerApp *app)
    : StrategyTask(task_info, app)
    , time_point_open_warning_(0)
    , time_point_bottom_price_(0)
	, time_point_half_ready_(0)
    , bottom_price_(9999.0)
{

}

void InflectionBuyTask::HandleQuoteData()
{
    //double percent_a = (para_.alert_price - bottom_price_) * 100 / para_.alert_price;
    static auto get_inflect_percent = [](double alert, double bottom, double cur)->double
    {
        double percent_inflect = 0.0;
        percent_inflect = (cur - bottom) * 100 / alert;
        return percent_inflect;
    };

    if( is_waitting_removed_ )
        return;

    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);
    double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;

    if( IsPriceJumpDown(pre_price, iter->cur_price) )
    {
        app_->local_logger().LogLocal(TSystem::utility::FormatStr("%d InflectionBuyTask price jump %.2f to %.2f", para_.id, pre_price, iter->cur_price));
        return;
    };
     
    bool is_to_send = false; 
    bool is_first_open = false;
    if( time_point_open_warning_ != 0 ) // if warning has been opened
    {
        if( iter->cur_price < bottom_price_ )
        {
            bottom_price_ = iter->cur_price;
            time_point_bottom_price_ = iter->time_stamp;
            app_->local_logger().LogLocal(TagOfOrderLog(), 
				utility::FormatStr("%d InflectionBuyTask::HandleQuoteData bottom %.2f", para_.id, bottom_price_));
        }
        //para_.continue_second = iter->time_stamp - time_point_open_warning_;
         
    }else if( iter->cur_price > 0 && iter->cur_price < para_.alert_price )
    {
        is_first_open = true;
        time_point_open_warning_ = time_point_bottom_price_ = iter->time_stamp; // open warning
        bottom_price_ = iter->cur_price; 
        app_->local_logger().LogLocal(TagOfOrderLog(), 
			utility::FormatStr("%d InflectionBuyTask::HandleQuoteData first open. cur:%.2f alert:%.2f bottom %.2f", para_.id, iter->cur_price, para_.alert_price, bottom_price_));
        
    }

	bool is_in_trigger_area = false;

    // if not first open and if reback > percent or reback to alert price
    if( time_point_open_warning_ != 0 && !is_first_open )
    {
        if( iter->cur_price > bottom_price_ )
        {
            // if reback to alert price
            if( para_.back_alert_trigger 
                && (iter->cur_price > para_.alert_price || Equal(iter->cur_price, para_.alert_price)) 
                //&& (para_.continue_second > 0 ? (iter->time_stamp - time_point_bottom_price_ > para_.continue_second) : true) 
                ) 
            {
                is_in_trigger_area = true; 
                app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionBuyTask bk_alert_trg %s %.2f %d | %.2f %ds ", para_.id, this->code_data(), iter->cur_price, para_.quantity, bottom_price_, para_.continue_second)); 
            }
            // if retreat_percent > rebounce_percent 
            else if( get_inflect_percent(para_.alert_price, bottom_price_, iter->cur_price) > para_.rebounce
                //&& (para_.continue_second > 0 ? (iter->time_stamp - time_point_bottom_price_ > para_.continue_second) : true)
				) 
            {
               is_in_trigger_area = true; 
               app_->local_logger().LogLocal(TagOfOrderLog(), 
                    TSystem::utility::FormatStr("%d InflectionBuyTask %s %.2f %d | %.2f %ds| infle %.2f ", para_.id, this->code_data(), iter->cur_price, para_.quantity, bottom_price_, para_.continue_second
                    , get_inflect_percent(para_.alert_price, bottom_price_, iter->cur_price))); 
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
      
    //-----------------------  
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
    //-----------------------
    if( is_to_send ) 
    {  
        time_point_open_warning_ = 0; //reset
        is_waitting_removed_ = true;
        app_->trade_strand().PostTask([iter, this]()
        {
            // send order 
        char result[1024] = {0};
        char error_info[1024] = {0};
               
        // to choice price to sell
        const auto price = GetQuoteTargetPrice(*iter, false);
#ifdef USE_TRADE_FLAG
        assert(this->app_->trade_agent().account_data(market_type_));

        auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
        this->app_->local_logger().LogLocal(TagOfOrderLog(), 
            TSystem::utility::FormatStr("触发任务:%d 拐点买入 %s 价格:%.2f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, para_.quantity, this->para_.continue_second)); 
        this->app_->AppendLog2Ui("触发任务:%d 拐点买入 %s 价格:%.2f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, para_.quantity, this->para_.continue_second);
        // sell the stock
        this->app_->trade_agent().SendOrder(this->app_->trade_client_id()
            , (int)TypeOrderCategory::BUY, 0
            , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
            , price, para_.quantity
            , result, error_info); 
#endif
        // judge result 
        if( strlen(error_info) > 0 )
        {
            auto ret_str = new std::string(utility::FormatStr("error %d 拐点买入 %s %f %d fail:%s"
                        , para_.id, para_.stock.c_str(), price, para_.quantity, error_info));
            this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
            this->app_->AppendLog2Ui(ret_str->c_str());
            this->app_->EmitSigShowUi(ret_str, true);
             
        }else
        {
            auto ret_str = new std::string(utility::FormatStr("任务:%d 拐点买入 %s %.2f %d 成功!", para_.id, para_.stock.c_str(), price, para_.quantity));
            this->app_->EmitSigShowUi(ret_str, true);
        }

        this->app_->RemoveTask(this->task_id(), TypeTask::INFLECTION_BUY);

        });
            
    }
}