#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

//倒计时,时间到了,notifyAll
class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    mutable MutexLock mutex_;    //一个对象要提供锁的功能的话,就要有mutex_这个成员
    Condition condition_;
    int count_;
};