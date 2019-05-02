#include "Thread.h"
#include "CurrentThread.h"
#include <memory>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <assert.h>

//线程独有数据, 线程局部存储
namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
}

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid)); //调用全局函数得到当前所在的线程的id
}


void CurrentThread::cacheTid()
{
    if(t_cachedTid == 0)
    {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
    }
}


//在线程中保存name, tid这些数据
struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid, CountDownLatch* latch)
    :func_(func),
    name_(name),
    tid_(tid),
    latch_(latch)
    {

    }

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();//这个latch_是
        latch_ = NULL;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);//设置进程的名字

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void *startThread(void* obj)//这是线程函数， obj是ThreadData
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    data = NULL;
}




Thread:: Thread(const ThreadFunc& func, const std::string& n)
    :started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(0),
    func_(func),
    name_(n),
    latch_(1)

{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_){
        pthread_detach(pthreadId_);//将线程变为不可unjoinable，会释放掉结束后会释放资源
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    if(pthread_create(&pthreadId_, NULL, &startThread, data))
    {
        started_ = false;
        delete data;
    }
    else
    {
        latch_.wait();
        assert(tid_ > 0);
    }    
}


int Thread::join()//其他线程调用这个线程的方法?
{
    assert(started_);
    assert(!joined_);
    joined_ =  true;
    return pthread_join(pthreadId_, NULL);
}

//private

void Thread::setDefaultName()
{
    if(name_.empty()){
        name_ = "Thread";
    }
}