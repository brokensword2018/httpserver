#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>

class EventLoop;//使用class 声明可以避免编译依赖
class HttpData;

class Channel
{
private:
    typedef std::function<void()> CallBack;
    EventLoop* loop_;
    int fd_;
    __uint32_t events_; //关心的事件  bit pattern
    __uint32_t revents_; //正在活动的事件
 

    //方便查找上层拥有该Channel的对象
    std::weak_ptr<HttpData> holder_;//

    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;

public:
    Channel(EventLoop* loop);
    Channel(EventLoop* loop, int fd);
    ~Channel();

    int getFd();
    void setFd(int fd);

    void setHolder(std::shared_ptr<HttpData> holder){
        holder_ = holder;
    }

    std::shared_ptr<HttpData> getHolder(){
        std::shared_ptr<HttpData> ret(holder_.lock());//提升成shared_ptr对象
        return ret;
    }

    void setReadHandler(CallBack&& readHeadler){//右值引用
        readHandler_ = readHeadler;
    }
    void setWriteHandler(CallBack&& writeHandler){
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack&& errorHandler){
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack&& connhandler){
        connHandler_ = connhandler;
    }

    void handleRead();
    void handleWrite();
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn();
    void handleEvents();

    void setRevents(__uint32_t rv){
        revents_ = rv;
    }
    __uint32_t& getEvents(){
        return events_;
    }

    void setEvents(__uint32_t ev){
        events_ = ev;
    }




};

typedef std::shared_ptr<Channel> SP_Channel;  //指向Channel的智能指针