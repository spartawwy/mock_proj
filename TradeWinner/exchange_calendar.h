#ifndef EXCHANGE_CALENDAR_SDF3SDFS_H_
#define EXCHANGE_CALENDAR_SDF3SDFS_H_

#include <vector>
#include <memory>

#include "common_base.h"

//class T_CalendarDate 
//{
//public:
//    T_CalendarDate(int d, bool is) : date(d), is_trade_day(is) { }
//    int  date;
//    bool is_trade_day;
//};

class ExchangeCalendar
{
public:

    ExchangeCalendar();

    bool IsTradeDate(int date);
    int PreTradeDate(unsigned int n);

    // return: yyyymmdd
    int TodayAddDays(int days=0);

private:

    std::shared_ptr<T_DateMapIsopen> trade_dates_;

    //std::vector<T_CalendarDate>  calendar_date_;

    friend class DBMoudle;
};

#endif