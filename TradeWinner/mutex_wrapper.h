#ifndef MUTEX_WRAPPER_H_W32SDFDS_
#define MUTEX_WRAPPER_H_W32SDFDS_

//#include <mutex>
#include <atomic>

class TimedMutexWrapper
{
public:
    TimedMutexWrapper();
    ~TimedMutexWrapper();

    bool try_lock_for(unsigned int milli_sec);
    void unlock();

private:

    class TimedMutext
    {
    public:
        TimedMutext();
        ~TimedMutext();
    private: 
        std::atomic_bool is_locked_;
    };

private:

     //std::timed_mutex  timed_mutex_;
     std::atomic_bool is_locked_;
};
#endif // MUTEX_WRAPPER_H_W32SDFDS_
