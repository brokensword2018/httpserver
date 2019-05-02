#include "AsyncLogging.h"
#include "LogFile.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <functional>


AsyncLogging::AsyncLogging(std::string logFileName, int flushInterval)
    :flushInterval_(flushInterval),
    running_(false),
    basename_(logFileName),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_(),
    latch_(1)
    {
        assert(logFileName.size() > 1);
        currentBuffer_->bzero();
        nextBuffer_->bzero();
        buffers_.reserve(16);
    }

void AsyncLogging::append(const char* logline, int len)//前端往buffers_里写
{
    MutexLockGurad lock(mutex_);
    if(currentBuffer_->avail() > len){//自定义的缓冲区所以不存在缓冲区写满的情况
        currentBuffer_->append(logline, len);
    }
    else{
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if(nextBuffer_){
            currentBuffer_ = std::move(nextBuffer_);
        }else{
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline, len);
        cond_.notify(); //写满了会通知一次
    }
}


void AsyncLogging::threadFunc()//这是AsyncLogging开的另一个线程, 同时也是后端往文件里写
{
    assert(running_);
    latch_.countDown();     //通知AsyncLogging线程, 的start方法结束
    LogFile output(basename_); //在后端线程主函数里，生命周期与程序一样长
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite; //后端往文件里写的缓冲区
    buffersToWrite.reserve(16);


    while(running_){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {//缓冲区会有前端和后端多个线程访问,所以需要加锁，临界区只有交换数据，比较短
            MutexLockGurad lock(mutex_);
            if(buffers_.empty()){
                cond_.waitForSecond(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);

            if(!nextBuffer_){
                nextBuffer_ = std::move(newBuffer2);
            }

        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25){//数据太多了，减少一部分，调试用
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        //写入到文件
        for(size_t i = 0; i < buffersToWrite.size(); ++i){
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if(buffersToWrite.size() > 2){
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset(); //把buffer清空
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();

}