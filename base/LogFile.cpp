#include "LogFile.h"
#include "FileUtil.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN)
    : basename_(basename),
    flushEveryN_(flushEveryN),
    count_(0),
    mutex_(new MutexLock)
{
    file_.reset(new AppendFile(basename));
}

LogFile::~LogFile()
{
    //通过智能指针管理对象,所以什么多不做
}

void LogFile::append(const char* logline, int len)
{
    MutexLockGurad lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush()
{
    MutexLockGurad lock(*mutex_);
    file_->flush();
}


//private

void LogFile::append_unlocked(const char* logline, int len)
{
    file_->append(logline, len);
    ++count_;
    if(count_ >= flushEveryN_)
    {
        count_ = 0;
        file_->flush();
    }
}








