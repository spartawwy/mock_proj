#ifndef MYTHREAD_H   
#define MYTHREAD_H   

#include <QThread>   

class Handler; 
class MyThread : public QThread  
{  
public:  

    MyThread(Handler &handler);  
    void run();  

    void stop_flag(bool val) { stop_flag_ = val; }

private:  

    int count;  
    bool stop_flag_;
    Handler &handler_;
};  

#endif // MYTHREAD_H  
