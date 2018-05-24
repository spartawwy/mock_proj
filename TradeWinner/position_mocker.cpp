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
void PositionMocker::UpdateToDb()
{ 
     auto cur_date = std::get<0>(CurrentDateTime());
     if( !exchange_calendar_->IsTradeDate(cur_date) )
         return;
     auto today_iter = days_positions_.find(cur_date);
     if( today_iter == days_positions_.end() )
     {
         //exchange_calendar_->PreTradeDate(last_position_date_) 
         auto item = days_positions_.find(last_position_date_);
         if( item == days_positions_.end() ) 
             today_iter = days_positions_.insert( std::make_pair(cur_date, T_CodeMapPosition(256)) ).first;
         else
             today_iter = days_positions_.insert( std::make_pair(cur_date, item->second) ).first;
     } 
     db_moudle_->UpdatePositionMock(*this, cur_date, user_id_);
}

void PositionMocker::UpdatePosition(const std::string &code, double avaliable, double frozon)
{
    auto date_time = CurrentDateTime();
    // if current position none get pre day position

}