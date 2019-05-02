#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread()//线程函数是执行loop_.loop()
    :loop_(NULL),
    exiting_(false),
    thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_)
{
    
}

EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    if(loop_ != NULL){
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop(){
    assert(!thread_.started());
    thread_.start();//EventLoopThread::threadFunc()函数开始执行
    {
        MutexLockGurad lock(mutex_);//加锁时避免信号丢失
        while(loop_ == NULL){
            cond_.wait();
        }
    }
    return loop_;
}


void EventLoopThread::threadFunc(){
    EventLoop loop;//线程池里面各个loop

    {
        MutexLockGurad lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    
    loop.loop();
    loop_ = NULL;//标记这个线程的结束
}