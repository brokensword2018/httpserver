#pragma once
#include "EventLoop.h"
#include "EventLoopTheadPool.h"
#include "Channel.h"
#include <memory>
#include "HttpData.h"


class Server
{
public:
    Server(EventLoop* loop, int threadNum, int port);
    ~Server() { };
    EventLoop* getLoop() const {return loop_;}
    void start();
    void handNewConn();
    void handThisConn(){
        loop_->updatePoller(acceptChannel_);//修改主线程poller_的Channel,应为时oneshot
    }
private:
    EventLoop* loop_;//主线程的EventLoop
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;//全局只有一个线程池
    bool started_;
    std::shared_ptr<Channel> acceptChannel_;
    int port_;
    int listenFd_;
    static const int MAXFDS = 100000;
};