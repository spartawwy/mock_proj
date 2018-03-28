#ifndef QTIMER_CONTAINER_SDF23SDFSD_H_
#define QTIMER_CONTAINER_SDF23SDFSD_H_

#include <memory>
#include <functional>
#include <atomic>
#include <unordered_map> 
#include <mutex>

#include <QTimer>
 

typedef std::function<void()>  TimerTask;
 
class QTimerWapper;  
class QTimerContainner
{

public:

    explicit QTimerContainner(bool single_shot);

    void InsertTimer(unsigned int ms, TimerTask&&task );
     
private:

    QTimerContainner();

    int AllocId() { return ++ cur_id; }
    void RemoveTimer(int id);

    std::atomic_uint32_t cur_id;
    bool is_remove_;

    std::mutex timers_mutex_;
    std::unordered_map<int, std::shared_ptr<QTimerWapper> > id_timers_;

    friend class QTimerWapper;
};

 
#endif //QTIMER_CONTAINER_SDF23SDFSD_H_
