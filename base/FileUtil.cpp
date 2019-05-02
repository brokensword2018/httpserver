#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string filename)
:   fp_(fopen(filename.c_str(), "ae"))  //ae 模式打开 append   e :fork之后不继承fd 
{ 
    // 用户提供缓冲区，文件描述符的缓冲区
    setbuffer(fp_, buffer_, sizeof(buffer_) ); //sizeof 不是函数是运算符
}

AppendFile::~AppendFile()
{
    fclose(fp_);
}

void AppendFile::append(const char* logline, const size_t len)
{
    size_t n = this->write(logline, len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t x = this->write(logline+n, remain);
        if(x == 0)
        {
            int err = ferror(fp_);
            if(err)
            {
                fprintf(stderr, "AppendFile::append() failed ! \n");
            }
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush()
{
    fflush(fp_);
}

size_t AppendFile::write(const char* logline, const size_t len)
{
    return fwrite_unlocked(logline, 1, len, fp_);//因为之后一个后端线程向文件写日志，所以不加锁
}