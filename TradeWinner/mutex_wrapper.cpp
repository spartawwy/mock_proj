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

bool TimedMutexWrapper::try_lock_for(__int64 milli_sec)
{ 
    assert(milli_sec >= 0);
    if( !is_locked_ )
    {
        is_locked_ = true;
        return true;
    }
  
    auto start = std::chrono::steady_clock::now();
    
    unsigned int delay_autom_val = 50;
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

    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
 
    while( is_locked_ && dur.count() < milli_sec ) 
    { 
        auto gap = milli_sec - dur.count();
        //auto ms_to_delay = gap > delay_autom_val ? delay_autom_val : gap;
        Delay(gap > delay_autom_val ? delay_autom_val : gap);
         
        dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
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

