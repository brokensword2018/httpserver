#pragma once
#include "noncopyable.h"
#include <pthread.h>
#include <cstdio>

class MutexLock : noncopyable
{
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex_, NULL); //初始化
    }
    ~MutexLock()
    {
        pthread_mutex_destroy(&mutex_);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }
    pthread_mutex_t* get()
    {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_; //pthread_mutex_t是一个union

    friend class Condition;
};

class MutexLockGurad: noncopyable
{
public:
    explicit MutexLockGurad(MutexLock& mutex)//因为它不支持拷贝构造函数所以用引用,并且在初始化列表中初始化,不可赋值
    : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGurad()
    {
        mutex_.unlock();
    }
private:
    MutexLock &mutex_;
};