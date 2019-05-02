#pragma once
#include "noncopyable.h"
#include "MutexLock.h"
#include <pthread.h>
#include <errno.h>
#include <cstdint>
#include <time.h>

class Condition : noncopyable
{
public:
    explicit Condition(MutexLock& mutex)
    : mutex_(mutex)
    {
        pthread_cond_init(&cond_, NULL);//同一个进程内多个线程可用
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_, mutex_.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond_);
    }

    //到达时间也返回,时间到达也返回
    bool waitForSecond(int second)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(second);
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.get(), &abstime);
    }

private:
    MutexLock&  mutex_;
    pthread_cond_t  cond_;
};