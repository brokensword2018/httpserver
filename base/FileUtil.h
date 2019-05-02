#pragma once

#include "noncopyable.h"
#include <string>

class AppendFile : noncopyable{
public:
    explicit AppendFile (std::string filename);
    ~AppendFile();

    //append方法向文件写内容
    void append(const char* logline, const size_t len);

    //flush缓冲区
    void flush();


private:
    //向文件里写内容
    size_t write(const char* logline, size_t len);
    
    FILE* fp_;
    char buffer_[64*1024];
};
