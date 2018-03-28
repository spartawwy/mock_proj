#ifndef QTIMER_WRAPPER_SDFW_HSDFSD
#define QTIMER_WRAPPER_SDFW_HSDFSD

#include <mutex>
#include <QTimer>

typedef std::function<void()>  TimerTask;
  
class TimerWapper : public QTimer
{
    Q_OBJECT

public:

    TimerWapper(void *container, int id, bool is_single_shot, TimerTask &&task);

    void Start(int millisec)
    {
        this->stop();
        this->setInterval(millisec);
        this->start(); 
    }

public slots:

   void DoTimeout();

private:
      
    void *p_container_;

    int id_;
    TimerTask  task_;

    bool is_single_shot_;
};


#endif