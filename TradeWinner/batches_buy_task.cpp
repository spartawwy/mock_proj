#include "batches_buy_task.h"

#include <TLib/core/tsystem_time.h>
#include <TLib/core/tsystem_utility_functions.h>

#include "winner_app.h"
  
#define  TEMP_DEBUG
/*
--------------------
       alert price 
--------------------
       index 0
--------------------
       index 1
--------------------
       index n
--------------------
*/
BatchesBuyTask::BatchesBuyTask(T_TaskInformation &task_info, WinnerApp *app, T_MockStrategyPara *mock_para)
    : StrategyTask(task_info, app, mock_para)
    , time_point_open_warning_(0) 
    , times_has_buy_(0)
    //, is_wait_trade_result_(false)
    , continue_trade_fail_count_(0)
    , trade_fail_ctr_count_(0)
    , is_ok_(true)
{ 
    step_items_.resize(100);

    //app_->local_logger().LogLocal(utility::FormatStr("construct BatchesBuyTask %d step: %.2f %% ", para_.id, para_.step));
    std::string buyed_field;
    auto pos0 = task_info.assistant_field.find("#");
    if( pos0 != std::string::npos )
    {
        times_has_buy_ = std::stoi(task_info.assistant_field.substr(0, pos0));
        buyed_field = task_info.assistant_field.substr(pos0 + 1, task_info.assistant_field.size() - pos0 - 1);
    }else
        buyed_field = task_info.assistant_field;
     
    auto array_ordered = utility::split(task_info.assistant_field, ";"); //  esction index which is buyed
    app_->local_logger().LogLocal(TagOfCurTask(), 
        utility::FormatStr("task %d BatchesBuyTask assistant_field:%s", para_.id, task_info.assistant_field.c_str()));
    
    for( int i = 0; i < array_ordered.size(); ++i )
    {
        try
        {
            auto index = std::stoi(array_ordered[i]);
            if( index > step_items_.size() - 1 )
            {
                app_->local_logger().LogLocal(TagOfCurTask(), 
                    utility::FormatStr("error: task %d BatchesBuyTask index:%d >= step_items_.size:%d", para_.id, index, step_items_.size()));
                is_ok_ = false;
                return;
            }
            step_items_[index].has_buy = true;
        }catch(...)
        {
            app_->local_logger().LogLocal(utility::FormatStr("error: task %d construct BatchesBuyTask trans assistant_field index %d", para_.id, i));
        }
    }

    for( int i = 0; i < step_items_.size(); ++i )
    {
        if( 100.0 - (i + 1) * para_.step < 0.0 )
            break;
        step_items_[i].up_price = para_.alert_price * (100 - (i + 1) * para_.step) / 100;
        auto bot = 100 - (i + 2) * para_.step;
        bot = bot < 0.0 ? 0 : bot;
        step_items_[i].bottom_price = para_.alert_price * bot / 100;
        if( i < 10 )
            app_->local_logger().LogLocal(TagOfCurTask()
                , utility::FormatStr("%d index:%d step:%.2f up_price:%.2f btm_price:%.2f"
                , para_.id, i, para_.step, step_items_[i].up_price, step_items_[i].bottom_price));
    }
}

BatchesBuyTask::~BatchesBuyTask()
{
    int i = 0;
    i = i;
}

void BatchesBuyTask::HandleQuoteData()
{
    static auto in_which_part = [](BatchesBuyTask* tsk, double price) ->int
    { 
        if( price < tsk->step_items_[tsk->step_items_.size()-1].bottom_price )
            return tsk->step_items_.size()-1;
        for( int i = 0; i < tsk->step_items_.size(); ++i )
        { 
            if( 100 - (i + 1) * tsk->para_.step < 0 )
                break;
            if( (price > tsk->step_items_[i].bottom_price || Equal(price, tsk->step_items_[i].bottom_price))
                && price < tsk->step_items_[i].up_price
              )
                return i;
        }
        return -1;
    };
 
    int ms_for_wait_lock = 1000;
    if( is_back_test_ ) ms_for_wait_lock = 5000; 
    if( !timed_mutex_wrapper_.try_lock_for(ms_for_wait_lock) )  
    {
        //DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("%d EqualSectionTask price %.2f timed_mutex wait fail", para_.id, iter->cur_price));
        app_->local_logger().LogLocal("mutex", "timed_mutex_wrapper_ lock fail"); 
        return;
    } 
 
    if( is_waitting_removed_ )
        return;
 
    if( !is_ok_ )
    {
        is_waitting_removed_ = true;
        this->app_->RemoveTask(this->task_id(), TypeTask::BATCHES_BUY);
        return;
    }
       
    assert( !quote_data_queue_.empty() );
    auto data_iter = quote_data_queue_.rbegin();
    std::shared_ptr<QuotesData> & iter = *data_iter;
    assert(iter);
    if( is_back_test_ && !has_set_ori_bktest_price_)
    {  
        has_set_ori_bktest_price_ = true;
        ori_bktest_price_ = iter->cur_price;
    } 
    double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;
      
    if( !(iter->cur_price < para_.alert_price) )
        goto NO_TRADE;

    if( continue_trade_fail_count_ >= 3 )
    {
        if( ++trade_fail_ctr_count_ % 60 != 0 )
        {
            app_->local_logger().LogLocal(TagOfCurTask(), "BatchesBuyTask::HandleQuoteData continue trade fail, do return!");
            goto NO_TRADE;
        }
    }

    int index = in_which_part(this, iter->cur_price);
    if( index < 0 )
        goto NO_TRADE;
    if( step_items_[index].has_buy )
        goto NO_TRADE; 
    // prepare buy ----------
    double capital = 0.0; 
    if( is_back_test_ )
        capital = bktest_para_.capital;
    else 
        capital = app_->QueryCapital().available;

    // check upper indexs which has't buy, if hasn't buy, buy it together
    int qty = 0;
    for( int i = index, count = 1; i >= 0; --i )
    {
        if( iter->cur_price * (qty + para_.quantity) > capital - 10.0 ) // an charge at least 5.00 yuan
            break;
        if( !step_items_[i].has_buy )
        {
            qty += para_.quantity;
            if( (times_has_buy_ + count++) >= para_.bs_times ) // check max buy times
                break;
        }
    }
    if( qty < 100 )
    {
        app_->local_logger().LogLocal(TagOfCurTask(), utility::FormatStr(" trigger step index %d: but capital:%.2f not enough", index, capital));
        goto NO_TRADE;
    } 
    goto BEFORE_TRADE;

NO_TRADE:
 
    timed_mutex_wrapper_.unlock();
    return;
  
BEFORE_TRADE:

    app_->local_logger().LogLocal(TagOfCurTask()
        , utility::FormatStr(" trigger step index %d: bottom: %.2f up:%.2f %s", index, step_items_[index].bottom_price, step_items_[index].up_price, ( qty > para_.quantity ? "supplement buy other index" : "")));
 
    //is_wait_trade_result_ = true;
    app_->trade_strand().PostTask([iter, index, qty, capital, this]()
    {  
    // send order 
    char result[1024] = {0};
    char error_info[1024] = {0};
	            
    // to choice price to buy
    auto price = 0.0;
    if( is_back_test_ )
    {
        price = iter->cur_price;
    }else
    {
        price = GetQuoteTargetPrice(*iter, false);
        if( IsPriceJumpUp(iter->cur_price, price) )
            price = iter->cur_price;
    }
    assert(this->app_->trade_agent().account_data(market_type_));

    //auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;     
	this->app_->local_logger().LogLocal(TagOfOrderLog(), 
        TSystem::utility::FormatStr("触发任务:%d 分批买入 %s 价格:%.2f 数量:%d ", para_.id, this->code_data(), price, qty)); 
    this->app_->AppendLog2Ui("触发任务:%d 分批买入 %s 价格:%.2f 数量:%d ", para_.id, this->code_data(), price, qty);
      
	// buy the stock
    this->app_->trade_agent().SendOrder(this->app_->trade_client_id(), (int)TypeOrderCategory::BUY, 0
        , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
        , price, qty
        , result, error_info); 
    // judge result 
    if( strlen(error_info) > 0 )
    {
        ++continue_trade_fail_count_;

        auto ret_str = new std::string(utility::FormatStr("error %d 分批买入 %s %.2f %d error:%s"
                    , para_.id, para_.stock.c_str(), price, qty, error_info));
        this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
        this->app_->AppendLog2Ui(ret_str->c_str());
        this->app_->EmitSigShowUi(ret_str, true);
    }else
    {   
        continue_trade_fail_count_ = 0;
        trade_fail_ctr_count_ = 0;
        // update state ----------------
        int quantity = 0;
        for( int i = index; i >= 0; --i )
        {
            if( iter->cur_price * (quantity + para_.quantity) > capital - 10.0 ) // an charge at least 5.00 yuan
                break;
            if( !step_items_[i].has_buy )
            {
                step_items_[i].has_buy = true;
                quantity += para_.quantity;
                if( ++times_has_buy_ >= para_.bs_times )
                {
                    is_waitting_removed_ = true;
                    break;
                }
            }
        }
 
        auto ret_str = new std::string(utility::FormatStr("执行任务:%d 分批买入 %s %.2f %d 成功!", para_.id, para_.stock.c_str(), price, qty));
        this->app_->EmitSigShowUi(ret_str, true);
    }
    // update assistant filed in db ------------
    para_.assistant_field.clear();
    for( int i = 0; i < step_items_.size(); ++i )
    {
        if( 100.0 - (i + 1) * para_.step < 0.0 )
            break;
        if( step_items_[i].has_buy ) 
        {
            if( para_.assistant_field.empty() )
            {
                para_.assistant_field.append(std::to_string(times_has_buy_));
                para_.assistant_field.append("#");
                para_.assistant_field.append(std::to_string(i));
            }else
            { 
                para_.assistant_field += ";" + std::to_string(i);
            }
        }
    } 
    if( !this->is_back_test_ )
    {
        app_->db_moudle().UpdateTaskInfo(para_);
        AddFill2DB(price, qty, true);
    }

    if( times_has_buy_ >= para_.bs_times )
    { 
        auto info_str = utility::FormatStr("分批买入任务:%d %s 已买 %d 次,任务结束!", para_.id, para_.stock.c_str(), times_has_buy_);
        this->app_->local_logger().LogLocal(info_str);
        this->app_->AppendLog2Ui(info_str.c_str());
 
        if( !is_back_test_ )
        {
            is_waitting_removed_ = true;
            timed_mutex_wrapper_.unlock();
            this->app_->RemoveTask(this->task_id(), TypeTask::BATCHES_BUY);
        }
    }else
    {
        timed_mutex_wrapper_.unlock();
    }
     
    }); 
 
}

std::string BatchesBuyTask::TagOfCurTask()
{ 
    return TSystem::utility::FormatStr("BatBuy_%s_%d", para_.stock.c_str(), TSystem::Today());
}