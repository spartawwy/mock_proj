#include "breakup_buy_task.h"
#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"

BreakUpBuyTask::BreakUpBuyTask(T_TaskInformation &task_info, WinnerApp *app)
    : StrategyTask(task_info, app)
	, time_point_bt_open_warning_(0)
{ 
}


void BreakUpBuyTask::HandleQuoteData()
{
    if( is_waitting_removed_ )
        return; 

    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);
    //double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;

    if( iter->cur_price > 0 && iter->cur_price > para_.alert_price )
    {
        if( time_point_bt_open_warning_ != 0 )
        { 
            if( iter->time_stamp - time_point_bt_open_warning_  >= para_.continue_second )
            {
				const auto time_span = iter->time_stamp - time_point_bt_open_warning_;
                time_point_bt_open_warning_ = 0; //reset
                is_waitting_removed_ = true;
                app_->trade_strand().PostTask([iter, time_span, this]()
                {
                 // send order 
                char result[1024] = {0};
                char error_info[1024] = {0};
	            
                // to choice price to buy
                const auto price = GetQuoteTargetPrice(*iter, false);
#ifdef USE_TRADE_FLAG
                assert(this->app_->trade_agent().account_data(market_type_));

                auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
                this->app_->local_logger().LogLocal(TagOfOrderLog()
					, TSystem::utility::FormatStr("触发任务:%d 突破买入 %s 价格:%.2f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, para_.quantity, time_span)); 
                this->app_->AppendLog2Ui("触发任务:%d 突破买入 %s 价格:%.2f 数量:%d 阀值:%d秒", para_.id, this->code_data(), price, para_.quantity, time_span);
                // buy the stock
                this->app_->trade_agent().SendOrder(this->app_->trade_client_id()
					, (int)TypeOrderCategory::BUY, 0
                    , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
                    , price, para_.quantity
                    , result, error_info); 
#endif
                // judge result 
                if( strlen(error_info) > 0 )
                {
                    auto ret_str = new std::string(utility::FormatStr("error %d 突破买入 %s %f %d fail:%s"
                        , para_.id, para_.stock.c_str(), price, para_.quantity, error_info));
                    this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
                    this->app_->AppendLog2Ui(ret_str->c_str());
                    this->app_->EmitSigShowUi(ret_str, true);
                     
                }else
                {
                    auto ret_str = new std::string(utility::FormatStr("执行任务:%d 突破买入 %s %.2f %d 成功!", para_.id, para_.stock.c_str(), price, para_.quantity));
                    this->app_->EmitSigShowUi(ret_str, true);
                }
                 
                this->app_->RemoveTask(this->task_id(), TypeTask::BREAKUP_BUY);

                });
            }
        }else
        {
            time_point_bt_open_warning_ = iter->time_stamp;
        }

    }else
    {
        time_point_bt_open_warning_ = 0; //reset
    }

}