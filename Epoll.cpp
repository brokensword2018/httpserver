#include "Epoll.h"
#include "Util.h"
#include "base/Logging.h"
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <queue>
#include <deque>
#include <assert.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;



const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SP_Channel;

Epoll::Epoll()
    :epollFd_(epoll_create1(EPOLL_CLOEXEC)),//新进程不会拥有该描述符
    events_(EVENTSNUM) //使events_ vector的容量为EVENTSNUM
    {
        assert(epollFd_);
    }

Epoll::~Epoll(){

}

//向Epoll中注册事件
void Epoll::epoll_add(SP_Channel request, int timeout){
    int fd = request->getFd();
    if(timeout > 0){
        add_timer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();



    fd2chan_[fd] = request;
    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0){
        perror("epoll_add error");
    }
}

//修改描述符状态
void Epoll::epoll_mod(SP_Channel request, int timeout){
    if(timeout > 0 ){
        add_timer(request, timeout);
    }

    
    // int fd = request->getFd();
    // struct epoll_event event;
    // event.data.fd = fd;
    // event.events = request->getEvents();
    // if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0){
    //     perror("epoll_mod error");
    // }
    
}

//删除描述符
void Epoll::epoll_del(SP_Channel request){
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0){
        perror("epoll_del error");
    }
    fd2chan_[fd].reset();//该文件描述符删除掉了
    fd2http_[fd].reset();
}

//返回需要处理的Channel, 活跃事件
std::vector<SP_Channel> Epoll::poll(){
    while(true){
        int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), -1);
        if(errno == EINTR){
            continue;
        }
        else if(event_count < 0){
            perror("epoll wait error");
        }
        //cout << "event_count " << event_count << endl;
        std::vector<SP_Channel> req_data = getEventsRequest(event_count);
        if(req_data.size() > 0){
            return req_data;
        }
        
    }
}

void Epoll::handleExpired(){
    timerManager_.handleExpiredEvent();
}


std::vector<SP_Channel> Epoll::getEventsRequest(int event_num){
    std::vector<SP_Channel> req_data;
    for(int i = 0; i < event_num; ++i){//epoll_wait把活动事件填入了events_ vector中
        int fd = events_[i].data.fd;

        SP_Channel cur_req = fd2chan_[fd];

        if(cur_req){
            cur_req->setRevents(events_[i].events);//感兴趣的事件发生了,把活动事件设置为感兴趣的事件
            req_data.push_back(cur_req);
        }else{
            LOG << "SP cur_req is invalid";
        }
    }
    return req_data;
}



void Epoll::add_timer(SP_Channel request_data, int timeout){
    shared_ptr<HttpData> t = request_data->getHolder();
    if(t){
        timerManager_.addTimer(t, timeout);
    }else{
        LOG << "timer add fail";
    }
}
