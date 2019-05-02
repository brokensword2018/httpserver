#include "Channel.h"
#include "Util.h"
#include <unistd.h>
#include <queue>
#include <cstdlib>
#include <iostream>
#include "base/CurrentThread.h"
using namespace std;

Channel::Channel(EventLoop* loop)
    :loop_(loop),
    events_(EPOLLIN)
    {

    }

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
    fd_(fd),
    events_(0)
    {

    }

Channel::~Channel(){

}

int Channel::getFd(){
    return fd_;
}

void Channel::setFd(int fd){
    fd_ = fd;
}

void Channel::handleRead(){
    if(readHandler_){
        readHandler_();
    }
}

void Channel::handleWrite(){
    if(writeHandler_){
        writeHandler_();
    }
}

void Channel::handleConn(){
    
    if(connHandler_){
        connHandler_();
    }else{
        cout << "connHandler_ is empty" << endl;
    }
}

void Channel::handleEvents(){
    //cout << "Channel:: handleEvents " << CurrentThread::tid() << endl;


    if(revents_ & EPOLLERR){
        if(errorHandler_){
            errorHandler_();
        }

        return;
    }
    if(revents_ & (EPOLLIN | EPOLLRDHUP | EPOLLPRI)){
        //cout << "Channel::handleRead : "  << fd_<< endl;
        handleRead();
    }

    handleConn();
}