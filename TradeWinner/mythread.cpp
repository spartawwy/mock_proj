 
#include "handler.h"
#include "mythread.h"
#include "QThread"
#include <QDebug>

static const int update_interval = 2000;  //ms

MyThread::MyThread(Handler &handler)
    : handler_(handler)
    , stop_flag_(false)
{
}


void MyThread::run()
{
    while( !stop_flag_ )
    {
        //qDebug()<< QThread::currentThread()->objectName()<<this->count++;
        handler_.Procedure();

        msleep(update_interval);
    }
}
