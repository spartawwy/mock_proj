#include "qtimer_wrapper.h"
 
#include "qtimer_container.h"
 

QTimerWapper::QTimerWapper(void *container, int id, bool is_single_shot, TimerTask &&task)
        : QTimer(nullptr), p_container_(container), id_(id), is_single_shot_(is_single_shot), task_(std::move(task))
{ 
    this->setSingleShot(is_single_shot_);
    bool ret = connect(this, SIGNAL(timeout()), this, SLOT(DoTimeout()));
}

void QTimerWapper::DoTimeout()
{
    task_();
    if( is_single_shot_ && p_container_ )
        ((QTimerContainner*)p_container_)->RemoveTimer(id_);
}
 