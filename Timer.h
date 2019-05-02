#pragma once
#include <unistd.h>
#include <memory>
#include <queue>
#include <vector>
#include "HttpData.h"
class HttpData;


class TimerNode
{
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode& tn);
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted(){
        deleted_ = true;
    }
    bool isDeleted() const{
        return deleted_;
    }
    size_t getExpTime(){
        return expiredTime_;
    }
private:
    bool deleted_;
    long long expiredTime_;
    std::shared_ptr<HttpData> SPHttpData_;
};

struct TimerCmp{
    bool operator()(std::shared_ptr<TimerNode> &a , std::shared_ptr<TimerNode> &b) const{
        return a->getExpTime() > b->getExpTime();//结束时间早的在前面
    }
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();

private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::vector<SPTimerNode>, TimerCmp> timerNodeQueue;
};
