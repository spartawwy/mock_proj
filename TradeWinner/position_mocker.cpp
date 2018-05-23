#include "position_mocker.h"

#include "common.h"
#include "db_moudle.h"

PositionMocker::PositionMocker(DBMoudle *db_moudle, ExchangeCalendar* exchange_calendar)
    : db_moudle_(db_moudle)
    , exchange_calendar_(exchange_calendar)
    , days_positions_(1024)
{ 
}

void PositionMocker::Update()
{
     auto cur_date = std::get<0>(CurrentDateTime());

     /*if( trade_dates_.find(cur_date) == trade_dates_.end() )
         return;*/
     // todo : get pre trade 's position 
}

void PositionMocker::UpdatePosition(const std::string &code, double avaliable, double frozon)
{
    auto date_time = CurrentDateTime();
    // if current position none get pre day position

}