# I/O 复用服务端
在[多进程服务端](../fork/README.md)中使用了多进程技术来使得服务端可以服务多个客户端，但是这样并不是一个很好的解决方案，因为创建进程需要付出很大代价，占用大量的运算和内存空间，而且进程间通信（IPC）也不是很好写。这里采用复用技术来替代多进程来同时为多个客户端提供服务。

复用技术是啥？数电、计网里都有过相关解释，常见的有时分复用、频分复用。就是一个电话线，可以简单实现两个人通信，那四个人通信呢？再假设一条电话线当然可以，或者还是使用那根电话线，采用时分复用传输数据，由于每个时间片很短，所以这四个人感觉不出来其实这跟线路实际上在某个确定的时刻只能两个人进行通信，交换通信的频率快到感觉不出来。

在这里我们使用一个进程来实现多路服务端，它能够同时服务多个客户端，要实现这个目地，我们需要同时监听每一个套接字，而且当这个套接字活跃的时候能把它选出来做出对于的处理。

# select
- [C++ socket 网络编程 简单聊天室. Thanks_up.  2019-03-13](https://www.cnblogs.com/DCD112358/p/10522172.html):从基本的CS 到 CSCs（select）,最后谈到优化（epoll、线程池）: select + keyNode
- [Socket Programming in C/C++: Handling multiple clients on server without multi threading. 30 May, 2018](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/): select + fd_set

## 使用流程
- 设置文件描述符、指定监视范围、设置超时  
- 调用 select 函数  
- 查看调用结果

用啥来存放所有在连接中的套接字呢？简单的用个数组吧，有一个常用的是定义在 <sys/select.h> 中的 `fd_set` 结构体，它是存有 0 和 1 的位数组，哪一位的值是 1 就代表哪一个套接字是监视对象之一。并且这个结构体有配套函数方便操作

```cpp
// 将 fdsetp 变量是所有位全部置 0
FD_ZERO(fdsetp)

// 向 fdsetp 中对应位置注册文件描述符 fd，由 0 置 1
FD_SET(fd, fdsetp) 

// 从 fdsetp 中对应位置清楚文件描述符 fd，由 1 置 0
FD_CLR(fd, fdsetp)

// 获取 fdsetp 中文件描述符 fd 的值， 0 或 1
FD_ISSET(fd, fdsetp)
```

## 程序
### select.cpp
[select.cpp](select.cpp)程序演示了 select 的基本用法，设置文件描述符和监听对象，对象在超时时间内触发 或者 超时。

1. 为什么要复制监视集 reads？

    这是因为调用 select 后，除了发生变化的文件描述符的对应位不变，剩下的所有位都会被重置为 0，这一点可以对比一下 temp 在 select 前后的变化。因此为了记住初始值，毕竟用某种方式记录原来的值，代码中呈现的是用最简单的复制来实现，用其它办法记住也行，比如数组

2. 为什么不把设置超时时间放在循环外面？放里面每次循环都要重新赋值

    这是因为调用 select 后，结构体 timeval 的属性值都会被替换为超时前生于时间，比如超时时间是 10 s ，还有 2s 即将超时，如果此时触发了，那么超时时间将会被更新为 2s,而不是我们设置的初始值了，因此我们需要重新设置这个超时时间。

### server-select.cpp
- server-select.cpp:基于 select 的 I/O 复用服务端
- client.cpp：回声客户端

<details>
<summary>点击展开运行案例</summary>

```bash
$ ./server-select
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 47056
New client：5 , IP 127.0.0.1 , Port 47058
New client：6 , IP 127.0.0.1 , Port 47060
Recv 1025 bytes: c1 . From IP 127.0.0.1 , Port 47060
Recv 1025 bytes: c2 . From IP 127.0.0.1 , Port 47060
Recv 1025 bytes: c3 . From IP 127.0.0.1 , Port 47060
Client 6 disconnect. IP 127.0.0.1 , Port 47060
Client 5 disconnect. IP 127.0.0.1 , Port 47060
Client 4 disconnect. IP 127.0.0.1 , Port 47060

# 下面是三个同时在线的客户端
$ ./client 
Input: c1
Recv 1025 bytes: c1 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

$ ./client 
Input: c2
Recv 1025 bytes: c2 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

$ ./client 
Input: \q
Log: Output close
Client close
[kearney@arch select-epoll]$ ./client 
Input: c3
Recv 1025 bytes: c3 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close
```
</details>

## 函数注解  
- select  
    `int select (int __nfds, fd_set *__restrict __readfds, fd_set *__restrict __writefds, fd_set *__restrict __exceptfds, struct timeval *__restrict __timeout)`  
    定义在 <sys/select.h> 中，作用是将 fd_set 中没有触发的都置为 0,触发的保持 1 不变。错误时返回 -1，成功时返回大于 0 的值  
    __nfds：套接字描述符  
    __readfds：监视 “是否存在带读取数据” 的 fd_set 集合地址   
	__writefds：监视 “是否可以传输无阻塞状态” 的 fd_set 集合地址   
	__exceptfds,：监视 “是否发生异常” 的 fd_set 集合地址 
	__timeout：超时时间的结构体， 0/NULL 的话就无限阻塞等待，设置具体数值可以避免无限阻塞，超时无反应会返回 0  

## 优点
- 程序兼容性比 epoll好，适合 win、linux 平台，epoll 只适合 linux

## 缺点
- 并发数最多几百，如果要求不高这都不是事
- 速度比 epoll 慢  
    这是因为每次都要循环一遍才知道到底是哪个监视对象，还有每次都要重新传递监视对象。第一感觉前者很浪费时间，但是实际上后者负担更大，把监视对象传递给操作系统可不容易，而且无法通过代码优化。套接字是由操作系统管理的，所以避免不了和操作系统打交道。

可不可以只向操作系统传递一次监视对象，然后当对象发生变化时再通知呢？既然这个问题存在那么多年了，肯定是可以的，Linux 下的方案是 epoll, Widnows 下是 IOCP
