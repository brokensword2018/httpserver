// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"
#include <getopt.h>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int threadNum = 4; //调试用只开一个线程
    int port = 8888;
    std::string logPath = "WebServer.log";

    // parse args
    int opt;
    const char *str = "t:p:";
    while ((opt = getopt(argc, argv, str))!= -1)//解析命令行参数
    {
        switch (opt)
        {
            case 't':
            {
                threadNum = atoi(optarg);
                break;
            }
            case 'p':
            {
                port = atoi(optarg);
                break;
            }
            default: break;
        }
    }
    Logger::setLogFileName(logPath);
    // STL库在多线程上应用

    EventLoop mainLoop;
    cout << "mainLoop : " << CurrentThread::tid() << endl;
    Server myHTTPServer(&mainLoop, threadNum, port);
    myHTTPServer.start(); //MainLoop里面只有一个acceptChannel
    mainLoop.loop();
    return 0;
}
