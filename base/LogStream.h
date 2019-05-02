#pragma once
#include "noncopyable.h"
#include <assert.h>
#include <string.h>
#include <string>

class AsyncLogging;

const int kSmallBuffer = 4000;         //4kb
const int kLargeBuffer = 4000*1000;    //4mb

template<int SIZE>
class FixedBuffer:noncopyable
{
public:
    FixedBuffer()
    : cur_(data_)
    {

    }

    ~FixedBuffer()
    {

    }

    void append(const char* buf, int len)
    {
        if(avail() > static_cast<int>(len))
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const
    {
        return data_;
    }

    int length() const
    {
        return static_cast<int>(cur_ - data_);
    }

    char* current()
    {
        return cur_;
    }

    int avail() const 
    {
        return static_cast<int>(end() - cur_);
    }

    void add(size_t len)
    {
        cur_ += len;
    }

    void reset()
    {
        cur_ = data_;
    }

    void bzero()
    {
        memset(data_, 0, sizeof data_);
    }



private:
    const char* end( ) const
    {
        return data_ + sizeof(data_);
    }

    char data_[SIZE];
    char* cur_;
};






//LogStream的功能是重载了输出运算符
class LogStream : noncopyable
{
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;
    LogStream& operator<<(bool v);

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    LogStream& operator<<(double);
    LogStream& operator<<(long double);
    LogStream& operator<<(float);

    LogStream& operator<<(char);
    LogStream& operator<<(const char* str);
    LogStream& operator<<(const unsigned char* str);
    LogStream& operator<<(const std::string& v);

    void append(const char* data, int len){
        buffer_.append(data, len);
    }
    const Buffer& buffer() const{
        return buffer_;
    }
    void resetBuffer(){
        buffer_.reset();
    }



private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;

};