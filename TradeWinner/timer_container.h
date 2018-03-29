#ifndef QTIMER_CONTAINER_SDF23SDFSD_H_
#define QTIMER_CONTAINER_SDF23SDFSD_H_

#include <memory>
#include <functional>
#include <atomic>
#include <unordered_map> 
#include <mutex>

#include <QTimer>

#if 1 

typedef std::function<void()>  TimerTask;
 
class TimerWapper;  
class TimerContainner
{ 
public:

    explicit TimerContainner(bool single_shot);

    void InsertTimer(unsigned int ms, TimerTask&&task );
     
private:

    TimerContainner();

    int AllocId() { return ++ cur_id; }
    void RemoveTimer(int id);

    std::atomic_uint32_t cur_id;
    bool is_remove_;

    std::recursive_mutex timers_mutex_;
    std::unordered_map<int, std::shared_ptr<TimerWapper> > id_timers_;

    friend class TimerWapper;
};
#else

class TimerContainner /*: protected QTimer*/
{
public:
    TimerContainner(bool) : QTimer(nullptr) {}
    void RemoveTimer(int) { }
};
#endif

#endif //QTIMER_CONTAINER_SDF23SDFSD_H_
