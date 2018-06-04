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
    int PreTradeDate(int date, unsigned int n);
    int NextTradeDate(int date, unsigned int n);

    // return: yyyymmdd
    int TodayAddDays(int days=0);
    int DateAddDays(int date, int days);

private:

    std::shared_ptr<T_DateMapIsopen> trade_dates_;

    //std::vector<T_CalendarDate>  calendar_date_;
    int min_trade_date_;
    int max_trade_date_;
    friend class DBMoudle;
};

#endif