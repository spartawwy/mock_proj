#include "position_mocker.h"

#include "common.h"
#include "db_moudle.h"
#include "exchange_calendar.h"
//#include "winner_app.h"

PositionMocker::PositionMocker(int user_id, DBMoudle *db_moudle, ExchangeCalendar* exchange_calendar)
    : user_id_(user_id)
    , db_moudle_(db_moudle)
    , exchange_calendar_(exchange_calendar)
    , days_positions_(1024)
    , p_cur_capital_(nullptr)
    , last_position_date_(0)
{ 
}

void PositionMocker::Reset()
{
    assert(p_cur_capital_);
     
    days_positions_.clear();
    int target_date = std::get<0>(CurrentDateTime());
    if( !exchange_calendar_->IsTradeDate(target_date) )
        target_date = exchange_calendar_->PreTradeDate(target_date, 1);

    last_position_date_ = target_date;
    auto iter = days_positions_.insert( std::make_pair(target_date, T_CodeMapPosition(256)) ).first;
    T_PositionData pos;
    strcpy_s(pos.code, sizeof(pos.code), CAPITAL_SYMBOL);
    pos.total = CAPITAL_ORI_TOTAL;
    pos.avaliable = CAPITAL_ORI_TOTAL; 

    p_cur_capital_ = std::addressof(iter->second.insert(std::make_pair(CAPITAL_SYMBOL, pos)).first->second);
    
    db_moudle_->ResetPositionMock(user_id_);
    db_moudle_->UpdatePositionMock(*this, last_position_date_, user_id_);

}

// ps: make sure days_positions_ has loaded 
void PositionMocker::DoEnterNewTradeDate(int date)
{ 
    assert(exchange_calendar_->IsTradeDate(date));
    assert(date >= last_position_date_);
     
     auto iter = days_positions_.find(date);
     if( iter == days_positions_.end() )
     {
         // create today 's position and save to db------------ 
         /*if( last_position_date_ == 0 ) 
             last_position_date_ = date;*/
         auto item = days_positions_.find(last_position_date_ == 0 ? date : last_position_date_);
         if( item == days_positions_.end() ) 
         {
             iter = days_positions_.insert( std::make_pair(date, T_CodeMapPosition(256)) ).first;
             T_PositionData pos;
             strcpy_s(pos.code, sizeof(pos.code), CAPITAL_SYMBOL);
             pos.total = CAPITAL_ORI_TOTAL;
             pos.avaliable = CAPITAL_ORI_TOTAL; 
             p_cur_capital_ = std::addressof(iter->second.insert(std::make_pair(CAPITAL_SYMBOL, pos)).first->second);

         }else
         { 
            auto capital_iter = item->second.find(CAPITAL_SYMBOL);
            assert(capital_iter != item->second.end() );

            std::for_each( std::begin(item->second), std::end(item->second), [](T_CodeMapPosition::reference entry)
            {
                entry.second.avaliable = entry.second.total;
            }); 
            if( last_position_date_ != 0 && last_position_date_ != date )
            { 
                days_positions_.insert( std::make_pair(date, item->second) ).first;
                db_moudle_->UpdatePositionMock(*this, last_position_date_, user_id_);
            }
            p_cur_capital_ = std::addressof( capital_iter->second );
         }
         db_moudle_->UpdatePositionMock(*this, date, user_id_);
         last_position_date_ = date;
     } else
     {
         auto item = iter->second.find(CAPITAL_SYMBOL);
         assert(item != iter->second.end() );
         p_cur_capital_ = std::addressof(item->second);
     } 
}

void PositionMocker::UpdatePosition(const std::string &code, double avaliable, double frozon)
{
    auto date_time = CurrentDateTime();
    // if current position none get pre day position

}

void PositionMocker::UnFreezePosition()
{
    if( last_position_date_ == 0 )
        return;
    auto item = days_positions_.find(last_position_date_);
    assert( item != days_positions_.end() );
    std::for_each( std::begin(item->second), std::end(item->second), [](T_CodeMapPosition::reference entry)
    {
        entry.second.avaliable = entry.second.total;
    }); 
    db_moudle_->UpdatePositionMock(*this, last_position_date_, user_id_);
}

T_PositionData * PositionMocker::ReadPosition(int date, const std::string& code)
{ 
    ReadLock locker(pos_mock_mutex_);
    if( date < 19900000 || date > 20600000 )
        return nullptr;
    int target_date = date;
    if( !exchange_calendar_->IsTradeDate(date) )
        target_date = exchange_calendar_->PreTradeDate(date, 1);
    auto iter = days_positions_.find(target_date);
    if( iter != days_positions_.end() )
    {
        auto item = iter->second.find(code);
        if( item != iter->second.end() )
            return std::addressof(item->second);
    }
    return nullptr;
}

T_CodeMapPosition PositionMocker::ReadAllStockPosition(int date)
{
    //std::vector<T_PositionData *>  ret_vector; 
    ReadLock locker(pos_mock_mutex_);
    if( date < 19900000 || date > 20600000 )
        return T_CodeMapPosition();
    int target_date = date;
    if( !exchange_calendar_->IsTradeDate(date) )
        target_date = exchange_calendar_->PreTradeDate(date, 1);
    auto iter = days_positions_.find(target_date);
    if( iter != days_positions_.end() )
    { 
        auto capital_pos_iter = iter->second.find(CAPITAL_SYMBOL);
        if( capital_pos_iter != iter->second.end() )
        {
            T_CodeMapPosition target = iter->second; 
            target.erase(CAPITAL_SYMBOL);
            return target;
        }else
            return iter->second; 
    }
    return T_CodeMapPosition();
}

void PositionMocker::AddTotalPosition(int date, const std::string& code, double val, bool is_freeze)
{
    WriteLock locker(pos_mock_mutex_);
    auto iter = days_positions_.find(date);
    if( iter == days_positions_.end() )
        iter = days_positions_.insert( std::make_pair(date, T_CodeMapPosition(256)) ).first;
    auto item = iter->second.find(code);
    if( item == iter->second.end() )
    {
        T_PositionData pos_data;
        pos_data.SetCode(code);
        pos_data.total += val;
        pos_data.avaliable = is_freeze ? 0 : val;
        item = iter->second.insert( std::make_pair(code, pos_data) ).first;
    }else
    {
        item->second.total += val;
        if( !is_freeze )
            item->second.avaliable += val;
    }
}

bool PositionMocker::SubAvaliablePosition(int date, const std::string& code, double val)
{
    WriteLock locker(pos_mock_mutex_);
    auto iter = days_positions_.find(date);
    if( iter == days_positions_.end() )
        return false;
    auto item = iter->second.find(code);
    if( item == iter->second.end() )
        return false;
    if( item->second.avaliable < val )
        return false;
    item->second.avaliable -= val;
    item->second.total -= val; 
    return true;
}
