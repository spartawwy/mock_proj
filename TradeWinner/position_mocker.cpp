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
    , last_position_date_(0)
{ 
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
             iter->second.insert(std::make_pair(CAPITAL_SYMBOL, pos));
         }else
         { 
            std::for_each( std::begin(item->second), std::end(item->second), [](T_CodeMapPosition::reference entry)
            {
                entry.second.avaliable = entry.second.total;
            }); 
            if( last_position_date_ != 0 && last_position_date_ != date )
            { 
                days_positions_.insert( std::make_pair(date, item->second) ).first;
                db_moudle_->UpdatePositionMock(*this, last_position_date_, user_id_);
            }
         }
         db_moudle_->UpdatePositionMock(*this, date, user_id_);
         last_position_date_ = date;
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