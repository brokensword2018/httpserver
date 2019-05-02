#pragma once
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"
#include <memory>
#include <string>

//对FileUtil进一步包装, 每写flushEveryN_ 次就flush一次
class LogFile : noncopyable
{
public:
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();


private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;//也是filename
    const int flushEveryN_;  //每写flushEveryN_ 次就flush一次

    int count_;
    std::unique_ptr<MutexLock> mutex_; //限制这个锁由一个对象所有，没必要加锁
    std::unique_ptr<AppendFile> file_;
};