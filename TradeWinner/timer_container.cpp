#include "timer_container.h"
 
#if 1 
#include "timer_wrapper.h"
// -------------------------

TimerContainner::TimerContainner(bool is_remove) 
    : is_remove_(is_remove) 
{ 
    cur_id = 0;
}

void TimerContainner::InsertTimer(unsigned int ms, TimerTask&&task)
{
    std::shared_ptr<TimerWapper> timer = nullptr;
    {
     std::lock_guard<std::mutex>  locker(timers_mutex_);  
     auto id = AllocId();
     timer = id_timers_.insert(std::make_pair(id, std::make_shared<TimerWapper>(this, id, is_remove_, std::move(task)))).first->second;
    }
    timer->Start(ms);

}
 
void TimerContainner::RemoveTimer(int id)
{
    std::lock_guard<std::mutex>  locker(timers_mutex_);  
    auto iter = id_timers_.find(id);
    if( iter != id_timers_.end() )
        id_timers_.erase(iter);
}
#else

#endif