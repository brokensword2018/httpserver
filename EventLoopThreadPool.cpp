#include "EventLoopTheadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int numThreads)
    :baseLoop_(baseLoop),//主循环，在这个类里面主要起一个assert作用，调试用的
    started_(false),
    numThreads_(numThreads),
    next_(0)
    {
        if(numThreads_ <= 0){
            LOG << "numThreads_ <= 0";
            abort();
        }
    }

void EventLoopThreadPool::start()
{
    baseLoop_->assertInLoopThread();
    started_ = true;
    for(int i = 0; i < numThreads_; ++i){
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = baseLoop_;
    if(!loops_.empty()){
        loop = loops_[next_];
        next_ = (next_ + 1) % loops_.size();
    }
    return loop;
}