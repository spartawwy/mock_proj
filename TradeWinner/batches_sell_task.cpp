#include "batches_sell_task.h"

#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"
  
BatchesSellTask::BatchesSellTask(T_TaskInformation &task_info, WinnerApp *app)
    : StrategyTask(task_info, app)
    , time_point_open_warning_(0) 
{ 
    step_items_.resize(50);
 
    auto array_ordered = utility::split(task_info.assistant_field, ";");
     
    for( int i = 0; i < array_ordered.size(); ++i )
    {
        try
        {
            auto index = std::stoi(array_ordered[i]);
            step_items_[index].has_selled = true;
        }catch(...)
        {
            app_->local_logger().LogLocal(utility::FormatStr("error: task %d construct BatchesSellTask trans assistant_field index %d", para_.id, i));
        }
    }

    for( int i = 0; i < step_items_.size(); ++i )
    {
        step_items_[i].bottom_price = para_.alert_price * (100 + i * para_.step) / 100;
        step_items_[i].up_price = para_.alert_price * (100 + (i + 1) * para_.step) / 100;
    }
}


void BatchesSellTask::HandleQuoteData()
{
    static auto in_which_part = [this](double price) ->int
    {
        for( int i = 0; i < step_items_.size(); ++i )
        { 
            if( (price > step_items_[i].bottom_price || Equal(price, step_items_[i].bottom_price))
                && price < step_items_[i].up_price
              )
                return i;
        }
        return -1;
    };

    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);

    //double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;

    bool is_to_remove = false;

    if( iter->cur_price > para_.alert_price )
    {
        int index = in_which_part(iter->cur_price);
        if( index < 0 )
            return;
        if( step_items_[index].has_selled )
            return;
        step_items_[index].has_selled = true;
        if( index == step_items_.size() - 1 )
            is_to_remove = true;
        // todo: if stocks quantity is 0 then to remove task

        app_->trade_strand().PostTask([iter, is_to_remove, this]()
        {
        // send order 
        char result[1024] = {0};
        char error_info[1024] = {0};
	    
        // to choice price to sell
        const auto price = GetQuoteTargetPrice(*iter, false);
        int qty = HandleSellByStockPosition(price, is_to_remove);
        if( qty == 0 )
            return;

#ifdef USE_TRADE_FLAG
        assert(this->app_->trade_agent().account_data(market_type_));

        auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
        this->app_->local_logger().LogLocal(TagOfOrderLog(), 
            TSystem::utility::FormatStr("触发任务:%d 分批出货 %s 价格:%f 数量:%d", para_.id, this->code_data(), price, qty)); 
        this->app_->AppendLog2Ui("触发任务:%d 分批出货 %s 价格:%f 数量:%d", para_.id, this->code_data(), price, qty);
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
            auto ret_str = new std::string(utility::FormatStr("error %d 分批出货 %s %.2f %d fail:%s"
                        , para_.id, para_.stock.c_str(), price, qty, error_info));
            this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
            this->app_->AppendLog2Ui(ret_str->c_str());
            this->app_->EmitSigShowUi(ret_str, true);
        }else
        {
            this->app_->SubPosition(para_.stock, qty); // sub availiable position
            auto ret_str = new std::string(utility::FormatStr("执行任务:%d 分批出货 %s %.2f %d 成功!", para_.id, para_.stock.c_str(), price, qty));
            this->app_->EmitSigShowUi(ret_str, true);
        }
        // update assistant filed in db ------------
        para_.assistant_field.clear();
        for( int i = 0; i < step_items_.size(); ++i )
        { 
            if( step_items_[i].has_selled ) 
            {
                if( para_.assistant_field.empty() )
                    para_.assistant_field.append(std::to_string(i));
                else
                    para_.assistant_field += ";" + std::to_string(i);
            }
        } 

        app_->db_moudle().UpdateTaskInfo(para_);
        if( is_to_remove )
            this->app_->RemoveTask(this->task_id(), TypeTask::BATCHES_SELL);

        });
    } 

}
