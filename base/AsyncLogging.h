#pragma once
#include "CountDownLatch.h"
#include "MutexLock.h"
#include "Thread.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <functional>
#include <string>
#include <vector>


class AsyncLogging : noncopyable
{
public:
    AsyncLogging(const std::string basename, int flushInterval = 2);
    ~AsyncLogging()
    {
        if(running_)
            stop();
    }
    void append(const char*, int);

    void start()
    {
        running_ = true;
        thread_.start();//后端线程函数开始执行
        latch_.wait();//后端线程函数已经开始
    }

    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }


private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;

    const int flushInterval_;
    bool running_;
    std::string basename_;
    Thread thread_;//后端线程
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountDownLatch latch_;//一般用于子线程准备好了
};