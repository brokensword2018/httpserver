#include "EventLoop.h"
#include "base/Logging.h"
#include "Util.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <iostream>
using namespace std;

__thread EventLoop* t_loopInThisThread = 0; //线程局部变量, 标记这个线程所拥有的EventLoop对象


int createEventFd(){
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        LOG << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    :looping_(false),
    poller_(new Epoll()),
    wakeupFd_(createEventFd()),//wakeup没有定时
    quit_(false),
    eventHandleing_(false),
    callingPendingFunctor_(false),
    threadId_(CurrentThread::tid()),
    pwakeupChannel_(new Channel(this, wakeupFd_))
{
    if(t_loopInThisThread){
        perror("this thread already has a eventloop object");
    }else{
        t_loopInThisThread = this;
    }
    pwakeupChannel_->setEvents(EPOLLIN);
    pwakeupChannel_->setReadHandler(bind(&EventLoop::handleRead, this));
    pwakeupChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
    poller_->epoll_add(pwakeupChannel_, 0);
}

EventLoop::~EventLoop(){
    close(wakeupFd_);
    t_loopInThisThread = NULL;
}
void EventLoop::handleConn(){
    updatePoller(pwakeupChannel_, 0);
}

void EventLoop::wakeup(){
    //cout << "start wakeup " << CurrentThread::tid() << endl;
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, &one, sizeof one);
    if(n != sizeof one){
        perror("EventLoop::wakeup error");
    }
}

void EventLoop::handleRead(){
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, (char*)&one, sizeof one);
    if(n != sizeof(one)){
        perror("EventLoop::handleRead error");
    }
}

void EventLoop::runInLoop(Functor&& cb){
    if(isInLoopThread())
        cb();
    else{
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor&& cb){

    {
        MutexLockGurad lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    
    //如果不在所属线程就唤醒,    可能调用的Functor会queueInLoop, 这样才会让新的cb及时调用
    if(!isInLoopThread() || callingPendingFunctor_){
        //cout << "queueInloop()  wakeup()  thread" << CurrentThread::tid() << endl;
        wakeup();
    }
        
}

void EventLoop::loop(){
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::vector<SP_Channel> ret;
    while(!quit_){
        ret.clear();
        ret = poller_->poll();//wakeup()也会产生一个事件,
        cout << "EventLoop::loop  " << CurrentThread::tid() << endl;
        eventHandleing_ = true;
        for(auto &it : ret){
            it->handleEvents();
        }
        eventHandleing_ = false;
        doPendingFunctors();
        poller_->handleExpired();//处理到时的
    }
    looping_ = false;
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    callingPendingFunctor_ = true;

    {
        MutexLockGurad lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(size_t i = 0; i < functors.size(); ++i){
        functors[i]();
    }
    callingPendingFunctor_ = false;
}

void EventLoop::quit(){
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}
