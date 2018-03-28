#include "qtimer_container.h"

#include "qtimer_wrapper.h"

#if 1 
// -------------------------

QTimerContainner::QTimerContainner(bool is_remove) 
    : is_remove_(is_remove) 
{ 
    cur_id = 0;
}

void QTimerContainner::InsertTimer(unsigned int ms, TimerTask&&task)
{
    std::shared_ptr<QTimerWapper> timer = nullptr;
    {
     std::lock_guard<std::mutex>  locker(timers_mutex_);  
     auto id = AllocId();
     timer = id_timers_.insert(std::make_pair(id, std::make_shared<QTimerWapper>(this, id, is_remove_, std::move(task)))).first->second;
    }
    timer->Start(ms);

}
 
void QTimerContainner::RemoveTimer(int id)
{
    std::lock_guard<std::mutex>  locker(timers_mutex_);  
    auto iter = id_timers_.find(id);
    if( iter != id_timers_.end() )
        id_timers_.erase(iter);
}
  
#endif