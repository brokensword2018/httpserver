#pragma once
#include <unordered_map>
#include <string>
#include <map>
#include "Timer.h"
#include <memory>

class EventLoop;
class TimerNode;
class Channel;

enum ProcessState
{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};

enum URIState
{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum ParseState // CR carriage Return 回车符  LF Line Feed 换行符
{
    H_START = 0,
    H_KEY,
    H_COLON,//冒号,解析header
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};

enum ConnectionState
{
    CONNECTED = 0,
    DISCONNECTING,
    DISCONNECTED    
};

enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};

enum HttpVersion
{
    HTTP_10 = 1,
    HTTP_11
};


//存储MimeType
class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

public:
    static std::string getMime(const std::string &suffix);//通过后缀得到MimeType

private:
    static pthread_once_t once_control; //用于控制init()只执行一次
};




//这个类负责处理http数据
//enable_shared_from_this , this 指针可以变身为shared_ptr
class HttpData : public std::enable_shared_from_this<HttpData>{
public:
    HttpData(EventLoop* loop, int fd);
    ~HttpData();
    void reset();
    void seperateTimer();
    void linkTimer(std::shared_ptr<TimerNode> timer){
        timer_ = timer;
    }
    std::shared_ptr<Channel> getChannel(){ return channel_; }
    EventLoop* getLoop(){return loop_; }
    void handleClose();
    void newEvent();

    void test();//做测试用的

private:
    EventLoop* loop_;
    std::shared_ptr<Channel> channel_;
    int fd_;
    std::string inBuffer_;
    std::string outBuffer_;
    bool error_;
    ConnectionState  connectionState_;
    
    HttpMethod method_;
    HttpVersion HTTPVersion_;
    std::string fileName_;
    std::string path_;
    int nowReadPos_;
    ProcessState state_;
    ParseState hState_;
    bool keepAlive_;
    std::map<std::string, std::string> headers_;
    std::weak_ptr<TimerNode> timer_;
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);

    //std::string sudo_data_;
    std::string sudo_data_;
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
};