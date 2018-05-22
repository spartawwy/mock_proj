#ifndef EXCHANGE_CALENDAR_SDF3SDFS_H_
#define EXCHANGE_CALENDAR_SDF3SDFS_H_

#include <vector>

class T_CalendarDate 
{
public:
    T_CalendarDate(int d, bool is) : date(d), is_trade_day(is) { }
    int  date;
    bool is_trade_day;
};

class ExchangeCalendar
{
public:

    ExchangeCalendar();

private:

    std::vector<T_CalendarDate>  calendar_date_;

    friend class DBMoudle;
};

#endif