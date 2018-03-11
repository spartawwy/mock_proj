#include "mutex_wrapper.h"

#include <chrono>
#include "common.h"

TimedMutexWrapper::TimedMutexWrapper() 
{
    is_locked_ = false;
}

TimedMutexWrapper::~TimedMutexWrapper() 
{ 
    is_locked_ = false;
}

bool TimedMutexWrapper::try_lock_for(unsigned int milli_sec)
{ 
    if( !is_locked_ )
    {
        is_locked_ = true;
        return true;
    }
  
    auto start = std::chrono::system_clock::now();
    int delay_autom_val = 50;
    if( milli_sec  > 10*1000 )
    {
        delay_autom_val = 50;

    }else if( milli_sec  > 1*1000 )
    {
        delay_autom_val = 20;
    }else if( milli_sec  > 500 )
    {
        delay_autom_val = 10;
    }else if( milli_sec  > 100 )
    {
        delay_autom_val = 5;
    }else
    {
        delay_autom_val = 2;
    }

    auto dur = std::chrono::system_clock::now() - start;
    while( is_locked_ && (unsigned int)int(dur.count()*1000) < milli_sec ) 
    {
        unsigned int dis =  milli_sec - (unsigned int)int(dur.count()*1000);
        Delay( dis > delay_autom_val ? delay_autom_val : dis );
        dur = std::chrono::system_clock::now() - start;
    }
    if( !is_locked_ )
    {
        is_locked_ = true;
        return true;
    }else
       return false;
     
}

void TimedMutexWrapper::unlock()
{
    is_locked_ = false;
}

