#include "HttpData.h"
#include "time.h"
#include "Channel.h"
#include "Util.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <sys/mman.h>
#include "Sudo.h"
#include "Util.h"
using namespace std;


pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;



const __uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET ;//EPOLLONESHOT只会触发一次,想要重新触发,重新注册
const int DEFAULT_EXPIRED_TIME = 2000; //ms
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms

char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};

void MimeType::init()
{
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
    pthread_once(&once_control, MimeType::init);
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];

}


HttpData::HttpData(EventLoop* loop, int connfd)
    :loop_(loop),
    channel_(new Channel(loop, connfd)),
    fd_(connfd),
    error_(false),
    connectionState_(CONNECTED),
    method_(METHOD_GET),
    HTTPVersion_(HTTP_11),
    nowReadPos_(0),
    state_(STATE_PARSE_URI),
    hState_(H_START),
    keepAlive_(false),
   sudo_data_()

{   
    channel_->setEvents(EPOLLIN  | EPOLLET);
    channel_->setReadHandler(bind(&HttpData::handleRead, this));
    channel_->setWriteHandler(bind(&HttpData::handleWrite, this));
    channel_->setConnHandler(bind(&HttpData::handleConn, this));
    
}

HttpData::~HttpData()
{
    cout << "--------close conn------" <<  fd_ << endl;
    close(fd_);//析构的时候
}

void HttpData::reset(){//将HttpData对象重置
    fileName_.clear();
    path_.clear();
    nowReadPos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
    if(timer_.lock()){//判断所指向的对象还在不在,先弄新的时间节点
        shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
}


//你不指向我,我不指向你
void HttpData::seperateTimer(){
    if(timer_.lock()){
        std::shared_ptr<TimerNode> myTimer(timer_.lock());
        myTimer->clearReq();//Timer不指向我
        timer_.reset();//我不指向timer
    }
}

//处理读, 也是解析URI 和header的过程
void HttpData::handleRead()
{
    __uint32_t& events_ = channel_->getEvents();
    do
    {
        bool zero = false;
        int read_num = readn(fd_, inBuffer_, zero);
        LOG << "Request(inBuffer_): " << inBuffer_;
        cout << "------------------request is : --------------"<<fd_ << endl << inBuffer_ << endl;
        if(zero){//关闭中 是只对方已经发送关闭了
            inBuffer_.clear();
            break;
        }
        if (read_num < 0)
        {
            perror("1");
            error_ = true;
            handleError(fd_, 400, "Bad Request");
            break;
        }

        if(state_ == STATE_PARSE_URI){
            URIState flag = this->parseURI();
            if(flag == PARSE_URI_ERROR){
                inBuffer_.clear();
                error_ = true;
                handleError(fd_, 401, "Bad Request");
                break;
            }
            state_ = STATE_PARSE_HEADERS;
        }

        if(state_ == STATE_PARSE_HEADERS){
            HeaderState flag = this->parseHeaders();
            if(flag == PARSE_HEADER_ERROR){
                inBuffer_.clear();
                error_ = true;
                handleError(fd_, 402, "Bad Request");
                break;
            }   
            state_ = STATE_ANALYSIS;
        }

        if(state_ == STATE_ANALYSIS){
            AnalysisState flag = this->analysisRequest();
            if(flag == ANALYSIS_ERROR){
                inBuffer_.clear();
                error_ = true;
                handleError(fd_, 403, "Bad Request");
                break;
            }else{
                state_ = STATE_FINISH;
            }
        }
    } while (false);

    if(!error_){
        if(!outBuffer_.empty())
        {
            inBuffer_.clear();
            handleWrite();
        }
        this->reset();
    }
}

void HttpData::handleWrite(){

    if(!error_ && connectionState_ != DISCONNECTED){
        
        
        if(writen(fd_, outBuffer_) < 0){
            perror("writen");
            error_ = true;
        }
        
        
        outBuffer_.clear();
    }

}



void HttpData::handleConn(){
    seperateTimer();//每一个链接都要重新处理, 用于处理长时间不活跃的连接

    if(keepAlive_){
        connectionState_ = CONNECTED;
    }else{
        connectionState_ = DISCONNECTING;
    }
    
    

    if(!error_  && keepAlive_)
    {   //cout << "long conn" << endl;         
        int timeout = DEFAULT_KEEP_ALIVE_TIME;
        loop_->updatePoller(channel_, timeout);//更新计时器
    }else if(!keepAlive_){
        //cout << "short conn" << endl;
        loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
    }

}

//向客户端发送错误信息
void HttpData::handleError(int fd, int err_num, std::string short_msg){
    short_msg = " " + short_msg;
    char send_buff[4096];
    string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += to_string(err_num) + short_msg;
    body_buff += "<hr><em> hejilin's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: hejilin's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

void HttpData::handleClose(){
    //guard自己定义的指针,指向自己,保证这个对象在完成这条对象之前不消失
    connectionState_ = DISCONNECTED;
    shared_ptr<HttpData> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
}

void HttpData::newEvent(){
    loop_->addToPoller(channel_, DEFAULT_EXPIRED_TIME);
}


URIState HttpData::parseURI(){
    string& str = inBuffer_;
    size_t pos = str.find('\r', nowReadPos_);
    //读到完整的行才开始解析
    if(pos == string::npos){
        return PARSE_URI_AGAIN;
    }




    //method
    size_t posGet = str.find("GET");
    size_t posPost = str.find("POST");
    size_t posHead = str.find("HEAD");

    if(posGet != string::npos){
        pos = posGet;
        method_ = METHOD_GET;
    }else if(posPost != string::npos){
        pos = posPost;
        method_ = METHOD_POST;
    }else if(posHead != string::npos){
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    //cout << "method :" << method_ << endl;

    //fileName_
    pos = str.find('/', pos);
    
    if(pos == string::npos){
        fileName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }else{
        size_t _pos = str.find(' ', pos);
        if(_pos == string::npos){
            return PARSE_URI_ERROR;
        }           
        if(_pos - pos == 1){
            fileName_ = "index.html";
        }else{
            fileName_ = str.substr(pos + 1, _pos - pos - 1);
            size_t __pos = fileName_.find('?');
            if(__pos != string::npos){
                fileName_ = fileName_.substr(0, __pos);
            }
        }
        pos = _pos;
    }
    
    //cout <<"filename : "<< fileName_ << endl;

    //HTTPVersion
    pos = str.find('/', pos);
    if(pos == string::npos)
        return PARSE_URI_ERROR;
    else{
        if(str.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else{
            string ver = str.substr(pos+1, 3);
            if(ver == "1.0")
                HTTPVersion_ = HTTP_10;
            else if(ver == "1.1")
                HTTPVersion_ = HTTP_11;
            else
                return PARSE_URI_ERROR;
            
        }
    }
    //cout << "HTTPVersion " << HTTPVersion_ << endl;
    return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parseHeaders(){//解析连接方式
    string& str = inBuffer_;
    size_t pos = str.find("Connection:");
    if(pos == string::npos){
        return PARSE_HEADER_SUCCESS;
    }
    size_t end_pos = str.find('\r', pos);
    string conn = str.substr(pos+12, end_pos - pos - 12);
    headers_["Connection"] = conn;
    
    if(conn == "keep-Alive" || conn == "keep-alive" || conn == "Keep-Alive"){
        //cout << "keep alive is true" << endl;
        keepAlive_ = true;
    }
      
    else if(conn == "close" || conn == "Close")
        keepAlive_ = false;
        
}

AnalysisState HttpData::analysisRequest(){
    if(method_ == METHOD_POST){      
        //进行数独的求解, 先是filename: sudo 然后get, 读取文件, 返回数独界面，求解才是post
        string& str = inBuffer_;
        
        sudo_data_ = getSudoDataFromRequest(str);
        cout << "-----------------sudo data is----------- : " << sudo_data_ << endl;
        string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end() && 
        (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive"))
        {
            header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" 
                    + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        
        Sudo sudo(sudo_data_);
        string ans = sudo.solve();
        cout << "--------------ans------------- :" << ans << endl;

        if(fileName_ == "sudo"){
        
            //读取sudo.html里的数据发送给前端
            char* filename = "../sudo.html";
            string body = readFileIntoString(filename);
            if(ans == "noSolution"){
                cout << "---------noSolution--------" << endl;
                fillSudoAnsInReponse(body, sudo_data_);
                //填充无解
                fillSolution(body, "noSolution");
            }else{
                cout << "---------solved--------" << endl;
               fillSudoAnsInReponse(body, ans);
               //填充有解
               fillSolution(body, "solved");
            }
            header += "Content-Type: text/html\r\n";
            header += "Content-Length: " + to_string(body.size()) + "\r\n";
            header += "Server: hejlin's Web Server\r\n";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += body;
            
            return ANALYSIS_SUCCESS;
        }

    }
    if(method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end() && 
        (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive"))
        {
            header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" 
                    + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        
        size_t dot_pos = fileName_.find('.');
        string filetype;
        if(dot_pos == string::npos){
            filetype = MimeType::getMime("default");
        }else{
            filetype = MimeType::getMime(fileName_.substr(dot_pos));
        }

        //echo test, 映射hello有问题
        if (fileName_ == "hello")
        {
            outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\Content-Length: 11n\r\n\r\nHello World";
            return ANALYSIS_SUCCESS;
        }
        if (fileName_ == "favicon.ico")
        {
            
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + to_string(sizeof favicon) + "\r\n";
            header += "Server: hejilin's Web Server\r\n";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += string(favicon, favicon + sizeof favicon);;
            return ANALYSIS_SUCCESS;
        }
        if(fileName_ == "sudo"){
        
            //读取sudo.html里的数据发送给前端
            char* filename = "../sudo.html";
            string body = readFileIntoString(filename);
            fillRandomIntInResponse(body);

            header += "Content-Type: text/html\r\n";
            header += "Content-Length: " + to_string(body.size()) + "\r\n";
            header += "Server: hejlin's Web Server\r\n";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += body;
            
            
            return ANALYSIS_SUCCESS;
        }
        
        return ANALYSIS_ERROR;
    }
}







