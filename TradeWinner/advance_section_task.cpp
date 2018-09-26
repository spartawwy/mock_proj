/*
 use assistant_field as pre trade price, if < 0.01 means it's original 
 todo: consider adding AdvanceSectionTask::Init 
 1.if first run, use cur_price as reb_base_price
 2.else only  really trade , set trade price as reb_base_price
*/
#include "advance_section_task.h"

#include <TLib/core/tsystem_utility_functions.h>
#include <TLib/core/tsystem_time.h>

#include "winner_app.h"

AdvanceSectionTask::Portion::Portion(int index,double bottom, double top, PortionState state) 
	: index_(index)
	, bottom_price_(bottom)
	, top_price_(top)
	, state_(state)  
{ 
	mid_price_ = Round((bottom_price_ + top_price_) / 2, 2);
}

AdvanceSectionTask::AdvanceSectionTask(T_TaskInformation &task_info, WinnerApp *app, T_MockStrategyPara *mock_para)
    : StrategyTask(task_info, app, mock_para)
    , app_(app)
    , reb_bottom_price_(MAX_STOCK_PRICE)
    , reb_top_price_(MIN_STOCK_PRICE)
    , is_not_enough_capital_continue_(0)
    , is_not_position_continue_(0)
    //, is_wait_trade_result_(false)
{ 
    assert(para_.advance_section_task.portion_sections.size() > 2 );
    assert(para_.rebounce > 0.0);
     
	// setup portions_  ------------------
    auto str_portion_vector = utility::split(para_.advance_section_task.portion_sections, ";");
    if( str_portion_vector.size() && str_portion_vector.at(str_portion_vector.size()-1) == "" )
        str_portion_vector.pop_back();
     
    if( str_portion_vector.size() < 2 )
    {
        ShowError(utility::FormatStr("error: AdvanceSectionTask %d str_portion_vector.size() < 2", para_.id));
        is_waitting_removed_ = true;
        return;
    }
    std::vector<double> price_vector;
    for( int i = 0 ; i < str_portion_vector.size(); ++i ) 
        price_vector.push_back(std::stod(str_portion_vector[i]));

    std::vector<std::string> str_portion_stat_vector = utility::split(para_.advance_section_task.portion_states, ";");

    int portion_num = 0;
    if( !para_.advance_section_task.is_original )
    {  
        if( str_portion_stat_vector.size() != str_portion_vector.size() )
        {
            ShowError(utility::FormatStr("error: AdvanceSectionTask %d portion_states.size != portion_sections.size", para_.id));
            is_waitting_removed_ = true;
            return;
        }
        try
        { 
            for( portion_num = 0; portion_num < str_portion_vector.size() - 1; ++portion_num ) 
            {  
                portions_.emplace_back(portion_num, price_vector[portion_num]
                         , price_vector[portion_num + 1]
                         , (PortionState)std::stoi(str_portion_stat_vector[portion_num]));
            }
        }
        catch (...)
        {
            ShowError(utility::FormatStr("error: AdvanceSectionTask %d illegal content in portion_sections or portion_states", para_.id));
            is_waitting_removed_ = true;
            return;
        } 
    }else // original
    { 
        try
        { 
            for( portion_num = 0; portion_num < str_portion_vector.size() - 1; ++portion_num ) 
            {  
                portions_.emplace_back(portion_num, price_vector[portion_num]
                    , price_vector[portion_num + 1]
                    , PortionState::UNKNOW);
            }
        }
        catch (...)
        {
            ShowError(utility::FormatStr("error: AdvanceSectionTask %d illegal content in portion_sections or portion_states", para_.id));
            is_waitting_removed_ = true;
            return;
        } 
    }
    
}

void AdvanceSectionTask::HandleQuoteData()
{  
	if( is_waitting_removed_ )
		return;
    
	assert( !quote_data_queue_.empty() );
	auto data_iter = quote_data_queue_.rbegin();
	std::shared_ptr<QuotesData> & iter = *data_iter;
	assert(iter);

	const double pre_price = quote_data_queue_.size() > 1 ? (*(++data_iter))->cur_price : iter->cur_price;
	if( IsPriceJumpDown(pre_price, iter->cur_price) || IsPriceJumpUp(pre_price, iter->cur_price) )
	{
		//app_->local_logger().LogLocal(cst_rebounce_debug, TSystem::utility::FormatStr("%d AdvanceSectionTask price jump %.2f to %.2f", para_.id, pre_price, iter->cur_price));
		return;
	};
    if( is_back_test_ && !has_set_ori_bktest_price_)
    {  
        has_set_ori_bktest_price_ = true;
        ori_bktest_price_ = iter->cur_price;
    } 
     
    //bool is_reb_base_price_need_change = false;
    if( reb_top_price_ < iter->cur_price ) 
    {
        reb_top_price_ = iter->cur_price;
        reb_base_price_ = reb_top_price_;
    }
	if( reb_bottom_price_ > iter->cur_price )
    {
        reb_bottom_price_ = iter->cur_price;
        reb_base_price_ = reb_bottom_price_;
    }

    
    int ms_for_wait_lock = 1000;
    if( is_back_test_ ) ms_for_wait_lock = 5000;
    if( !timed_mutex_wrapper_.try_lock_for(ms_for_wait_lock) )  
    {
        //DO_LOG_BKTST(TagOfCurTask(), *ret_str);
        DO_LOG_BKTST(TagOfCurTask(), TSystem::utility::FormatStr("%d EqualSectionTask price %.2f timed_mutex wait fail", para_.id, iter->cur_price));
        app_->local_logger().LogLocal("mutex", "timed_mutex_wrapper_ lock fail"); 
        return;
    };
    if( is_waitting_removed_ )
        return;

    TypeAction action = TypeAction::NOOP;
    TypeOrderCategory order_type = TypeOrderCategory::SELL;
    int qty = 0;
    int qty_can_buy = 0;
    int avaliable_pos = 0;

	// judge in which section  --------------

	int cur_index = 0; 
	auto cur_portion_iter = portions_.end();

	if( iter->cur_price < portions_.begin()->bottom_price() ) // in clear section 
	{ 
        action = TypeAction::CLEAR;
        order_type = TypeOrderCategory::SELL;
        cur_index = -1;
        qty = GetAvaliablePosition();
        DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("task:%d %s price:%.2f trigger clearing position ", para_.id, para_.stock.c_str(), iter->cur_price));
        goto BEFORE_TRADE;
	}
	else if( iter->cur_price >= portions_.rbegin()->top_price() )
    { 
		action = TypeAction::NOOP;
        cur_index = portions_.size();
        reb_top_price_ = iter->cur_price;
        reb_base_price_ = portions_.at(portions_.size()-1).mid_price();
        //DO_LOG("AdvanceSec", utility::FormatStr("task:%d %s price:%.2f in stop trade section ", para_.id, para_.stock.c_str(), iter->cur_price));
        goto NOT_TRADE;
    }
    if( reb_top_price_ > portions_.rbegin()->top_price() )
        reb_top_price_ = portions_.rbegin()->top_price() - 0.001;
    if( reb_bottom_price_ < portions_.begin()->bottom_price() )
        reb_bottom_price_ = portions_.begin()->bottom_price() + 0.001;

	cur_portion_iter = std::find_if( std::begin(portions_), std::end(portions_),[&iter, this](Portion &entry)
	{
		if( iter->cur_price >= entry.bottom_price() && iter->cur_price < entry.top_price() ) return true;
		else return false;
	});
    if( cur_portion_iter == std::end(portions_) )
    {
        assert(false);
        app_->local_logger().LogLocal(utility::FormatStr("error: task %d AdvanceSectionTask::HandleQuoteData can't find portions!", para_.id));
        goto NOT_TRADE;
    }  
	cur_index = cur_portion_iter->index();
    
    // if original only consider buy 
    const double up_rebounce = Get2UpRebouncePercent(reb_base_price_, reb_bottom_price_, iter->cur_price);
    if( para_.advance_section_task.is_original )
    {
        if( up_rebounce < para_.rebounce )
            goto NOT_TRADE;
        // create position --------------
        double capital = 0.0;
        if( is_back_test_ )
            capital = bktest_para_.capital;
        else
            capital = this->app_->QueryCapital().available;
          
        qty_can_buy = int(capital / iter->cur_price) / 100 * 100;
        if( is_back_test_ && qty_can_buy > 0 && bktest_para_.capital < iter->cur_price * qty_can_buy + CaculateFee(iter->cur_price * qty_can_buy, order_type == TypeOrderCategory::BUY) )
            qty_can_buy -= 100;
        if( qty_can_buy < 100 )
        {
            if( is_not_enough_capital_continue_++ % 300 == 0 )
                DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("warning:cur_p:%d  to create position but capital:%.2f not enough! | %.2f %.2f %.2f", cur_index, capital, reb_base_price_, reb_bottom_price_, iter->cur_price));
            goto NOT_TRADE;
        }else
            is_not_enough_capital_continue_ = 0;

        // judge if any portion need buy  
        order_type = TypeOrderCategory::BUY; 
        auto tup_val = judge_any_pos2buy(iter->cur_price, cur_index, qty_can_buy, false);
        qty = std::get<0>(tup_val);
        if( qty > 100 )
            goto BEFORE_TRADE;
        else
            goto NOT_TRADE;
    }
    //------------------ not is_original------------------------
     
    // when rebounce up in down ward : consider buy --------------------------------- 
    if( iter->cur_price < para_.advance_section_task.pre_trade_price ) 
    {
        if( up_rebounce < para_.rebounce )
            goto NOT_TRADE;
         
        double capital = 0.0; 
        if( is_back_test_ )
            capital = bktest_para_.capital;
        else 
            capital = this->app_->QueryCapital().available;
         
        qty_can_buy = int(capital / iter->cur_price) / 100 * 100;
        if( is_back_test_ && qty_can_buy > 0 && bktest_para_.capital < iter->cur_price * qty_can_buy + CaculateFee(iter->cur_price * qty_can_buy, order_type == TypeOrderCategory::BUY) )
            qty_can_buy -= 100;
        if( qty_can_buy < 100 )
        {
            if( is_not_enough_capital_continue_++ % 300 == 0 )
            {
                DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("cur_p:%d downward, up rebounce trigger judge buy :%.2f | %.2f %.2f %.2f", cur_index, up_rebounce, reb_base_price_, reb_top_price_, iter->cur_price));
                DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("warning:cur_p:%d to buy but capital:%.2f not enough! | %.2f %.2f %.2f", cur_index, capital, reb_base_price_, reb_bottom_price_, iter->cur_price));
            }
            goto NOT_TRADE;
        }else
        {
            is_not_enough_capital_continue_ = 0;
            DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("cur_p:%d downward, up rebounce trigger judge buy :%.2f | %.2f %.2f %.2f", cur_index, up_rebounce, reb_base_price_, reb_top_price_, iter->cur_price));
        }
        
        // judge if any portion need buy  
        order_type = TypeOrderCategory::BUY; 
        qty = std::get<0>(judge_any_pos2buy(iter->cur_price, cur_index, qty_can_buy, false));
        if( qty > 100 )
            goto BEFORE_TRADE;
        else
            reset_flag_price(iter->cur_price);
         
    }else if( iter->cur_price > para_.advance_section_task.pre_trade_price ) 
    {
        // when rebounce down in up ward: consider sell ------------------
        const double down_rebounce = Get2DownRebouncePercent(reb_base_price_, reb_top_price_, iter->cur_price);
        if( down_rebounce < para_.rebounce )
            goto NOT_TRADE;
          
        order_type = TypeOrderCategory::SELL;
        //int qty_sell = 0;
        avaliable_pos = GetAvaliablePosition();
        if( avaliable_pos <= 0 )
        {
            if( is_not_position_continue_++ % 300 == 0 )
            {
                DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("cur_p:%d upward, down rebounce trigger judge sell :%.2f | %.2f %.2f %.2f", cur_index, down_rebounce, reb_base_price_, reb_top_price_, iter->cur_price));
                DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("warning: cur_p:%d to sell but avaliable pos is 0 ", cur_index));
            }
            reset_flag_price(iter->cur_price);
            goto NOT_TRADE;
        }else
        {
            is_not_position_continue_ = 0;
            DO_LOG_BKTST(TagOfCurTask(), utility::FormatStr("cur_p:%d upward, down rebounce trigger judge sell :%.2f | %.2f %.2f %.2f", cur_index, down_rebounce, reb_base_price_, reb_top_price_, iter->cur_price));
        }
 
        // judge if any portion need sell 
        qty = std::get<0>(judge_any_pos2sell(iter->cur_price, cur_index, avaliable_pos, false));
        if( qty > 0 )
            goto BEFORE_TRADE;
        else
            reset_flag_price(iter->cur_price);
    }
     
NOT_TRADE:
   
    timed_mutex_wrapper_.unlock();
    return;

BEFORE_TRADE:   

    reb_top_price_ = iter->cur_price;
    reb_bottom_price_ = iter->cur_price;
    
    app_->trade_strand().PostTask([iter, action, order_type, qty, cur_index, qty_can_buy, avaliable_pos, this]()
    {
        char result[1024] = {0};
        char error_info[1024] = {0};

        // to choice price to order
        auto price = 0.0;
        if( action == TypeAction::CLEAR )
            price = iter->price_b_3;
        else
            price = GetQuoteTargetPrice(*iter, order_type == TypeOrderCategory::BUY ? true : false);
        std::string cn_order_str = order_type == TypeOrderCategory::BUY ? "买入" : "卖出";

        //------------------back test -------------
        if( is_back_test_ )
        {
            if( order_type == TypeOrderCategory::BUY )
            {
                if( bktest_para_.capital < price * qty + CaculateFee(price*qty, order_type == TypeOrderCategory::BUY) )
                {
                    strcpy_s(error_info, "capital not enough!");
                    DO_BKTST_DETAIL(utility::FormatStr("port:%d Order Data Buy %d %.2f fail:%s", cur_index, qty, price, error_info));
                }else{
                    DO_BKTST_DETAIL(utility::FormatStr("port:%d Order Data Buy %d %.2f", cur_index, qty, price));
                    bktest_para_.frozon_position += qty;
                    bktest_para_.capital -= price * qty + CaculateFee(price*qty, order_type == TypeOrderCategory::BUY);
                }
            }else
            { 
                DO_BKTST_DETAIL(utility::FormatStr("port:%d Order Data Sell %d %.2f", cur_index, qty, price));
                assert(bktest_para_.avaliable_position >= qty);
                bktest_para_.avaliable_position -= qty;
                bktest_para_.capital += price * qty - CaculateFee(price*qty, order_type == TypeOrderCategory::SELL);
            }
        }else
        {
#ifdef USE_TRADE_FLAG
        assert(this->app_->trade_agent().account_data(market_type_));

        //auto sh_hld_code  = const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code;
        this->app_->local_logger().LogLocal(TagOfOrderLog(), 
            TSystem::utility::FormatStr("贝塔任务:%d %s %s 价格:%.2f 数量:%d ", para_.id, cn_order_str.c_str(), this->code_data(), price, qty)); 
        this->app_->AppendLog2Ui("贝塔任务:%d %s %s 价格:%.2f 数量:%d ", para_.id, cn_order_str.c_str(), this->code_data(), price, qty);

        // order the stock
        this->app_->trade_agent().SendOrder(app_->trade_client_id(), (int)order_type, 0
            , const_cast<T_AccountData *>(this->app_->trade_agent().account_data(market_type_))->shared_holder_code, this->code_data()
            , price, qty
            , result, error_info); 
#endif 
        }

        // judge result 
        if( strlen(error_info) == 0 ) // trade success
        {
            if( !is_back_test_ )
            {
                auto ret_str = new std::string(utility::FormatStr("贝塔任务:%d %s %s %.2f %d 成功!", para_.id, cn_order_str.c_str(), para_.stock.c_str(), price, qty));
                this->app_->local_logger().LogLocal(TagOfOrderLog(), *ret_str);
                this->app_->EmitSigShowUi(ret_str, true);
            }
           
            para_.advance_section_task.is_original = false;
            para_.advance_section_task.pre_trade_price = price;

            if( action != TypeAction::CLEAR )
            {  
                // update portions state -------
                if( order_type == TypeOrderCategory::BUY )
                    judge_any_pos2buy(iter->cur_price, cur_index, qty_can_buy, true);
                else 
                    judge_any_pos2sell(iter->cur_price, cur_index, avaliable_pos, true);
                // reset ----- 
                reset_flag_price(price);
                is_not_enough_capital_continue_ = 0;

                // translate portions state into para.advancesection.portion_states 
                para_.advance_section_task.portion_states.clear(); 
                for(auto item: this->portions_)
                {
                    switch( item.state() )
                    {
                    case PortionState::WAIT_BUY: para_.advance_section_task.portion_states.append(std::to_string(int(PortionState::WAIT_BUY)));break;
                    case PortionState::WAIT_SELL: para_.advance_section_task.portion_states.append(std::to_string(int(PortionState::WAIT_SELL)));break;
                    case PortionState::UNKNOW: para_.advance_section_task.portion_states.append(std::to_string(int(PortionState::UNKNOW)));break;
                    default: assert(false); break;
                    }
                }
                para_.advance_section_task.pre_trade_price = price;
                para_.advance_section_task.is_original = false;
                if( !is_back_test_ )
                {
                    // save to db: save cur_price as start_price in assistant_field 
                    app_->db_moudle().UpdateAdvanceSection(para_);
                    AddFill2DB(price, qty, order_type == TypeOrderCategory::BUY);
                } 

            }else
            { 
                ShowError(utility::FormatStr("贝塔任务:%d %s 已破底清仓! 将移除任务!", para_.id, para_.stock.c_str()));
                if( !is_back_test_ )
                {
                    is_waitting_removed_ = true;
                    timed_mutex_wrapper_.unlock();
                    this->app_->RemoveTask(this->task_id(), TypeTask::ADVANCE_SECTION); // invoker delete self
                    return;
                }
            }
        }else // trade fail
        {  
            ShowError(utility::FormatStr("error %d %s %s %.2f %d error:%s"
                , para_.id, cn_order_str.c_str(), para_.stock.c_str(), price, qty, error_info));
        }
        timed_mutex_wrapper_.unlock();
    });

    return;

}

void AdvanceSectionTask::reset_flag_price(double cur_price)
{
    reb_bottom_price_ = MAX_STOCK_PRICE;
    reb_top_price_ = MIN_STOCK_PRICE;  
    reb_base_price_ = cur_price; 
}

std::tuple<int, double, bool> AdvanceSectionTask::judge_any_pos2buy(double cur_price, int cur_index, int para_qty_can_buy,  bool is_do_change) 
{
    bool is_on_border = true;
    int local_qty_buy = 0;
    if( cur_price < portions_[cur_index].mid_price()
        && (portions_[cur_index].state() == PortionState::UNKNOW || portions_[cur_index].state() == PortionState::WAIT_BUY) )
    {
        if( local_qty_buy + para_.quantity <= para_qty_can_buy)
        {
            if( is_do_change ) 
            {
                DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_SELL", cur_index));
                portions_[cur_index].state(PortionState::WAIT_SELL);
            }
            local_qty_buy += para_.quantity;
        }
    }
    for( int i = cur_index + 1; i < portions_.size(); ++i )
    { 
        assert(cur_price < portions_[i].mid_price());
        if( local_qty_buy + para_.quantity <= para_qty_can_buy )
        {
            if( portions_[i].state() == PortionState::UNKNOW || portions_[i].state() == PortionState::WAIT_BUY )
            {
                if( is_do_change ) 
                {
                    DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_SELL", i));
                    portions_[i].state(PortionState::WAIT_SELL);
                }
                local_qty_buy += para_.quantity;
                is_on_border = false;
            }
        }
        else
            break;
    }
    return std::make_tuple(local_qty_buy, portions_[cur_index].mid_price(), is_on_border);
}


std::tuple<int, double, bool> AdvanceSectionTask::judge_any_pos2sell(double cur_price, int cur_index, int para_avaliable_pos,  bool is_do_change)
{
    bool is_on_border = true;
    int local_qty_sell = 0;
#if 0 
    for( int i = 0; i < cur_index - 1; ++i )
    { 
        assert(cur_price > portions_[i].mid_price());
        if( local_qty_sell + para_.quantity <= para_avaliable_pos )
        {
            if( portions_[i].state() == PortionState::WAIT_SELL )
            {
                if( is_do_change )
                {
                    DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_BUY", i));
                    portions_[i].state(PortionState::WAIT_BUY);
                }
                local_qty_sell += para_.quantity;
                is_on_border = false;
            }
        }else
            break;
    }
    if( cur_price > portions_[cur_index].mid_price() )
    {
        if( cur_index - 1 >= 0 && local_qty_sell + para_.quantity <= para_avaliable_pos )
        { 
            if( portions_[cur_index-1].state() == PortionState::WAIT_SELL )
            {
                if( is_do_change )
                {
                    DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_BUY", cur_index-1));
                    portions_[cur_index-1].state(PortionState::WAIT_BUY);
                }
                local_qty_sell += para_.quantity;
            }
        }
    }
#else
    if( cur_price > portions_[cur_index].mid_price() )
    {
        if( cur_index - 1 >= 0 && local_qty_sell + para_.quantity <= para_avaliable_pos )
        { 
            if( portions_[cur_index-1].state() == PortionState::WAIT_SELL )
            {
                if( is_do_change )
                {
                    DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_BUY", cur_index-1));
                    portions_[cur_index-1].state(PortionState::WAIT_BUY);
                }
                local_qty_sell += para_.quantity;
            }
        }
    }
    for( int i = cur_index-1; i >= 1; --i )
    {
        assert(cur_price > portions_[i].mid_price());
        if( local_qty_sell + para_.quantity > para_avaliable_pos )
            break;
        if( portions_[i-1].state() == PortionState::WAIT_SELL )
        {
            if( is_do_change )
            {
                DO_LOG(TagOfCurTask(), TSystem::utility::FormatStr("set portion %d WAIT_BUY", i));
                portions_[i].state(PortionState::WAIT_BUY);
            }
            local_qty_sell += para_.quantity;
            is_on_border = false;
        }
    }
#endif 
    return std::make_tuple(local_qty_sell, portions_[cur_index].mid_price(), is_on_border);
}
 

std::string AdvanceSectionTask::TagOfCurTask()
{ 
    return TSystem::utility::FormatStr("AdvSec_%d_%s_%d", para_.id, para_.stock.c_str(), TSystem::Today());
}