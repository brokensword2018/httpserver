#include "Timer.h"
#include <sys/time.h>
#include <iostream>
using namespace std;

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
    :deleted_(false),
    SPHttpData_(requestData)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    //以毫秒为计算单位,计算结束时间
    expiredTime_ = (now.tv_sec * 1000) + (now.tv_usec / 1000) + timeout;
}

TimerNode::~TimerNode(){
    //关闭超时连接
    if(SPHttpData_){
        SPHttpData_->handleClose();
    }
}

bool TimerNode::isValid(){
    struct timeval now;
    gettimeofday(&now, NULL);
    long long time_now_ms = (now.tv_sec * 1000) + (now.tv_usec / 1000);
    if(time_now_ms >= expiredTime_){
        this->setDeleted();
        return false;;
    }else{
        return true;
    }
    
}

void TimerNode::clearReq(){
    SPHttpData_.reset();
    this->setDeleted();
}

TimerManager::TimerManager(){

}

TimerManager::~TimerManager(){

}

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout){
    SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
    //std::cout << "the timer queue size is :" << timerNodeQueue.size() << std::endl;
}

void TimerManager::handleExpiredEvent(){
    while(!timerNodeQueue.empty()){
        SPTimerNode ptimer_node = timerNodeQueue.top();
        if(ptimer_node->isDeleted()){
            timerNodeQueue.pop();
        }else if(ptimer_node->isValid() == false){
            timerNodeQueue.pop();
        }else{
            break;
        }
    }
}
