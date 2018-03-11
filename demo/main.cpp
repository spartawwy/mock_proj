#include "demo.h"
#include <QtWidgets/QApplication>

#include "ticker.h"

int main(int argc, char *argv[])
{
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
