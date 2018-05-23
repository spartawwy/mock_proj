#include "exchange_calendar.h"

#include <ctime>
#include <chrono>

ExchangeCalendar::ExchangeCalendar() 
{ 
    trade_dates_ = std::make_shared<T_DateMapIsopen>(10*1024);
}

bool ExchangeCalendar::IsTradeDate(int date)
{
     T_DateMapIsopen &date_map_opend = *trade_dates_;
     return date_map_opend.find(date) != date_map_opend.end();
}

// pre n trade date 
int ExchangeCalendar::PreTradeDate(unsigned int n)
{ 
    //using namespace std::chrono;
    //for( int i = 1; i < n; ++i )
    unsigned int count = 0;
    int i = 1;
    T_DateMapIsopen &date_map_opend = *trade_dates_;
    int a = 0;
    while( count < n )
    {
        a = TodayAddDays(-1 * i);  
        if( date_map_opend.find(a) != date_map_opend.end() )
            ++count;
        ++i;
    }
    return a;
}

int ExchangeCalendar::TodayAddDays(int days)
{
    std::time_t day_t = 0;
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    if( days >= 0 )
        day_t = system_clock::to_time_t(now + std::chrono::hours(24*days));
    else
        day_t = system_clock::to_time_t(now - std::chrono::hours(24*abs(days)));
    tm tm_day_t;
    _localtime64_s(&tm_day_t, &day_t);
    return (tm_day_t.tm_year + 1900) * 10000 + (tm_day_t.tm_mon + 1) * 100 + tm_day_t.tm_mday;
}