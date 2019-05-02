#include "Server.h"
#include "base/Logging.h"
#include "Util.h"
#include <functional>
#include <sys/socket.h>
#include <arpa/inet.h>




Server::Server(EventLoop* loop, int threadNum, int port)
    :loop_(loop),
    threadNum_(threadNum),
    eventLoopThreadPool_(new EventLoopThreadPool(loop_, threadNum)),
    started_(false),
    acceptChannel_(new Channel(loop_)),
    port_(port),
    listenFd_(socket_bind_listen(port_))
{
    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();//
    if(setSocketNonBlocking(listenFd_) < 0){
        perror("set socket non block error");
        abort();
    }
}

void Server::start(){
    eventLoopThreadPool_->start();//创建线程池里的线程，并且开始循环
    acceptChannel_->setReadHandler(bind(&Server::handNewConn, this));//listenfd可读时
    acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
    loop_->addToPoller(acceptChannel_, 0);
    started_ = true;
}

void Server::handNewConn(){  
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(listenFd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
        EventLoop* loop= eventLoopThreadPool_->getNextLoop();//主线程采用轮训的方法分配到各个线程
        LOG << "New connection from" << inet_ntoa(client_addr.sin_addr) << " : " << ntohs(client_addr.sin_port);
        cout << "-----------------new connection come---------------" << accept_fd << endl;
        //限制服务器的最大连接数目
        if(accept_fd >= MAXFDS){
            close(accept_fd);
            continue;
        }
        
        if(setSocketNonBlocking(accept_fd) < 0){
            perror("set non block failed !");
            return;
        }

        setSocketNodelay(accept_fd);

        shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
        req_info->getChannel()->setHolder(req_info);
        
        loop->queueInLoop(bind(&HttpData::newEvent, req_info));//跨线程分配任务，向loop所属的线程注册channle, 
        //向timerManager里写HttpData
    }
}

