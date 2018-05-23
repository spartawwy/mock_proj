#include "demo.h"

#include <functional>
#include <iostream>

#include <QtWidgets/QApplication>
 
#include <TLib/core/tsystem_utility_functions.h>

#include <TLib/core/tsystem_time.h>

#include "ticker.h"

void test();

void test_time();
int TodayAddDays(int days);

int main(int argc, char *argv[])
{ 
    // test();
    test_time();
    int ch_val = 0;
     ch_val = TodayAddDays(1)  ;
      ch_val = TodayAddDays(2)  ;
      ch_val = TodayAddDays(10) ;
                                ;
      ch_val = TodayAddDays(-1) ;
      ch_val = TodayAddDays(-2) ;
      ch_val = TodayAddDays(-10);
      ch_val = TodayAddDays(-40);
    getchar();
    QApplication a(argc, argv);
#if 1
    demo w;
    w.show();
#endif

#if 0 
    StockTicker  tick_obj;
    tick_obj.Init();

    tick_obj.test();
#endif

    return a.exec();
}

void test()
{
    using namespace TSystem;
    //std::string  str_tmp = "12.2;22.5;25";
    std::string  str_tmp = "12.2;22.5;25;";
     auto str_porion_vector = utility::split(str_tmp, ";");
}

void test_time()
{
    auto val = TSystem::Today();

    using namespace std::chrono;

    system_clock::time_point now = system_clock::now();

    std::time_t pre_day_t = system_clock::to_time_t(now - std::chrono::hours(24));
    std::time_t next_day_t = system_clock::to_time_t(now + std::chrono::hours(24));

    /*std::cout << "One day ago, the time was "<< std::put_time(std::localtime(&last), "%F %T") << '\n';
    std::cout << "Next day, the time was "<< std::put_time(std::localtime(&next), "%F %T") << '\n';*/
    tm pre_t, next_t;
     _localtime64_s(&pre_t, &pre_day_t);
     _localtime64_s(&next_t, &next_day_t);
    int pre_date = (pre_t.tm_year + 1900) * 10000 + (pre_t.tm_mon + 1) * 100 + pre_t.tm_mday;
    int next_date = (next_t.tm_year + 1900) * 10000 + (next_t.tm_mon + 1) * 100 + next_t.tm_mday;
}

int TodayAddDays(int days)
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