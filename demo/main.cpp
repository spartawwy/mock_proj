#include "demo.h"

#include <functional>
#include <iostream>

#include <QtWidgets/QApplication>
 
#include <TLib/core/tsystem_utility_functions.h>

#include "ticker.h"

void test();

int main(int argc, char *argv[])
{ 
    // test();

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