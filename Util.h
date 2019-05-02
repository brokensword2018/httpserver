//负责套接字的底层操作

#pragma once
#include <cstdlib>
#include <string>
#include <cstdlib>


//从fd中读取n个字节,放入buff中
ssize_t readn(int fd, void* buff, size_t n);
//向inBuffer里读取数据,读到无法再度,并设置zero标志,时候是读到返回为0
ssize_t readn(int fd, std::string &inBuffer, bool &zero);
//向inBuffer里读取数据,读到无法再度
ssize_t readn(int fd, std::string &inBuffer);
//从buff里取n个字节数据,写入fd
ssize_t writen(int fd, void *buff, size_t n);
//把sbuff里的字符向fd里写
ssize_t writen(int fd, std::string &sbuff);
//当向已关闭的socket发送数据时,会产生SIGPIPE信号中断, 这个处理是忽略掉这个忽略掉这个信号
void handle_for_sigpipe();
//将fd设置为非阻塞
int setSocketNonBlocking(int fd);
//关闭Nagle算法
void setSocketNodelay(int fd);
//设置优雅关闭,套接字选项SO_LINGER设置为一定的延时,套接字关闭时,会阻塞到输出传输完成或则延时的时间,套接字必须为非阻塞
void setSocketNoLinger(int fd);
//关闭套接字的读写
void shutDownWR(int fd);
//根据端口号,创建监听套接字
int socket_bind_listen(int port);

//从指定文件里读取字符串
std::string readFileIntoString(char * filename);

//从请求报文里面解析出数独请求
std::string getSudoDataFromRequest(std::string request);

//将答案填入响应报文
void fillSudoAnsInReponse(std::string& reponse, std::string ans);

//将答案状态填入状态栏
void fillSolution(std::string& reponse, std::string str);

//随机产生几个数填入数独
void fillRandomIntInResponse(std::string& reponse);