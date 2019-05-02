
//timer.h头文件没写, TimeManger类成员没做

#pragma once
#include "Channel.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>
#include "Timer.h"

class HttpData;

class Epoll
{
public:
    Epoll();
    ~Epoll();
    
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel request);
    std::vector<SP_Channel> poll();
    void handleExpired();
    std::vector<SP_Channel> getEventsRequest(int event_num);
    void add_timer(SP_Channel request_data, int timeout);
    int getEpollFd(){
        return epollFd_;
    }
private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::shared_ptr<Channel> fd2chan_[MAXFDS];
    std::shared_ptr<HttpData> fd2http_[MAXFDS];
    TimerManager timerManager_;
};