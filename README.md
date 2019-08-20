## httpserver
一个简单的多线程服务器

## 架构设计
主线程不停地接受新的连接，将新连接的处理加入到工作线程的任务队列；工作线程主要进行数据的读写和数独的求解工作。
关键的类有两个：

```Channel```类：一个```Channel```对象负责一个文件描述符，其中注册了相关文件描述符的读、写、错误处理等回调函数。
```EventLoop```类，每一个线程都有一个```EventLoop```对象，每一个```EventLoop```对象管理一个```epoll``。 主线程中epoll注册的是监听套接字，监听套接字可读时，调用accept从监听队列中取得文件描述符，分配到工作线程的任务队列中；工作线程主要任务为将任务队列里的文件描述符注册到epoll中，从epoll中取得活跃的文件描述符，然后调用相关文件描述符注册的读写回调函数进行读写处理。

## 难点
跨线程的任务分配，参考的muduo网络库，在工作线程中加入一个任务队列的方式来进行跨线程的任务分配。

## 待改进
1.线程任务的分配规则。可以选择拥有最少文件描述符的线程进行任务分配。
2.线程的粒度比较大。工作线程做了所有的东西，可以把数独的求解单独提出来，数独的求解可以考虑采用并行算法。
