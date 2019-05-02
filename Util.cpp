#include "Util.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include <fstream>
#include <sstream>

const int MAX_BUFF = 4096;
ssize_t readn(int fd, void* buff, size_t n){
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0){
        if((nread = read(fd, ptr, nleft)) < 0){
            if(errno == EINTR){
                nread = 0;
            }
            else if(errno == EAGAIN){
                return readSum;
            }else{
                return -1;
            }
        }else if(nread == 0){
            break;
        }
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t readn(int fd, std::string& inBuffer, bool& zero){
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true){
        char buff[MAX_BUFF];
        if((nread = read(fd, buff, MAX_BUFF)) < 0){
            if(errno == EINTR){//被中断了继续读
                continue;
            }else if(errno == EAGAIN){//现在读不到了,请稍后再试
                return readSum;
            }else{
                perror("read error");
                return -1;
            }
        }else if(nread == 0){
            zero = true;
            break;
        }

        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer)
{
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while (true)
    {
        char buff[MAX_BUFF];
        if ((nread = read(fd, buff, MAX_BUFF)) < 0)
        {
            if (errno == EINTR)
                continue;
            else if (errno == EAGAIN)
            {
                return readSum;
            }  
            else
            {
                perror("read error");
                return -1;
            }
        }
        else if (nread == 0)
        {
            //printf("redsum = %d\n", readSum);
            break;
        }
        //printf("before inBuffer.size() = %d\n", inBuffer.size());
        //printf("nread = %d\n", nread);
        readSum += nread;
        //buff += nread;
        inBuffer += std::string(buff, buff + nread);
        //printf("after inBuffer.size() = %d\n", inBuffer.size());
    }
    return readSum;
}

ssize_t writen(int fd, void* buff, size_t n){
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char* ptr = (char*)buff;
    while(nleft > 0 ){
        if((nwritten = write(fd, ptr, nleft)) <= 0){
            if(nwritten < 0){
                if(errno == EINTR){
                    nwritten = 0;
                    continue;
                }else if(errno == EAGAIN){
                    return writeSum;
                }else{
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string& sbuff){
    size_t nleft = sbuff.size();
    ssize_t nwriten = 0;
    size_t writeSum = 0;
    const char* ptr = sbuff.c_str();
    while(nleft > 0){
        if((nwriten = write(fd, ptr, nleft)) <= 0){
            if(nwriten < 0){
                if(errno == EINTR){
                    nwriten = 0;
                    continue;
                }else if(errno == EAGAIN){
                    break;
                }else{
                    return -1;
                }
            }
        }
        writeSum += nwriten;
        nleft -= nwriten;
        ptr += nwriten;
    }
    if(writeSum == static_cast<int>(sbuff.size())){
        sbuff.clear();
    }else{
        sbuff = sbuff.substr(writeSum);
    }
    return writeSum;
}

void handle_for_sigpipe(){
    struct  sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}

int setSocketNonBlocking(int fd){
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1)
        return -1;
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}

void setSocketNodelay(int fd){
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}


void shutDownWR(int fd){
    shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port){
    if(port < 0 || port > 65535)
        return -1;
    
    //创建套接字
    int listenfd = 0;
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;
    
    //允许地址重用,会处于time_wait状态
    int optval = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        return -1;

    //设置服务器IP和Port,并和描述符绑定
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short)port);
    if(bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        return -1;
    }

    //转变为监听套接字
    if(listen(listenfd, 2048) == -1){
        close(listenfd);
        return -1;
    }

    return listenfd;
}

std::string readFileIntoString(char * filename){

    std::ifstream ifile(filename);
    //将文件读入到ostringstream对象buf中
    std::ostringstream buf;
    char ch;
    while(buf && ifile.get(ch))
    buf.put(ch);
    //返回与流对象buf关联的字符串
    return buf.str();
}

std::string getSudoDataFromRequest(std::string request){
    std::string ret(81, '0');
    ssize_t pos = 0;
    for(int i = 0; i < 81; ++i){
        pos = request.find("input=", pos);
        ret[i] = request[pos+6] == '-' ? '0' : request[pos+6];
        pos += 6;
    }

    return ret;
}

void fillSudoAnsInReponse(std::string& reponse, std::string ans){
    ssize_t pos = 0;
    for(int i = 0; i < 81; ++i){
        pos = reponse.find("value=\"-\"", pos);
        reponse[pos+7] = ans[i];
        pos += 7;
    }
}

void fillSolution(std::string& reponse, std::string str){
    ssize_t pos = reponse.find("value=\"            \"");
    for(int i = pos + 7, j = 0; i < pos + 18 && j < str.size(); ++i, ++j){
        reponse[i] = str[j];
    }
}

void fillRandomIntInResponse(std::string& reponse){
    std::string ret(81, '-');
    for(int i = 0; i < 3; ++i){
        int num = rand() % 9 + 1;
        int place = rand() % 81;
        ret[place] = '0' + num;
    }
    fillSudoAnsInReponse(reponse, ret);
}








