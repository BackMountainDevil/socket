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

# epoll
- [深入理解 Epoll .Dreamgoing](https://zhuanlan.zhihu.com/p/93609693)

## 优点
- 不需要每次循环一遍来查找是哪个小宝贝触发了
- 不需要每次传递监视对象的信息
## 程序
- [server-epoll.cpp](server-epoll.cpp): 基于 epoll 的 I/O 复用服务端  
回声客户端程序还是[client.cpp](client.cpp)

## 条件触发和边缘触发
条件触发：只要输入缓冲区中还剩有数据，就会再次触发事件，select 和 epoll 默认都是条件触发方式  
边缘触发：输入缓冲区收到数据时仅注册一次事件，即使输入缓冲区还有数据也不会再次触发事件  

如果要修改 epoll 为边缘触发方式，修改一下操作类型即可 - `event.events = EPOLLIN | EPOLLET;`  
<details>
<summary>点击展开运行案例</summary>

为了更加明显的观察的条件触发的效果，最好将服务端的缓冲区改小一点，客户端的缓冲区也适当改小。下面是的案例中服务端、客户端的 `BUF_SIZE` 分别是 4、10。也就是说客户端发来一次长度为 10 B的数据时，服务端因为缓冲区收到数据就会被触发（代码第 75 行），然后调用一次 read 读取缓冲区（代码第 101 行），然而一次读取的最多只有 4 B，然后这个子程序就执行完了，这里并没有使用循环读取到没有数据为止，然后由于采用了条件触发，缓冲区还有数据没有读出来，于是再次被触发继续读取 4 B,然后还没有读完，再触发读取完剩下的 2 B。如果采取边缘触发的话，第一次读取完 4 B就完了，不会再次触发。

```bash
# 条件触发案例，服务端、客户端的 `BUF_SIZE` 分别修改为 4、10
$ ./server-epoll 
Waiting for connecting
epoll_wait awake
New client：5 , IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 4 bytes: 1234 . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 4 bytes: 567 . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 2 bytes:  . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 4 bytes: 123  . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 4 bytes: 567  . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Recv 2 bytes: 9 . From IP 127.0.0.1 , Port 51940
epoll_wait awake
Client 5 disconnect. IP 127.0.0.1 , Port 51940

$ ./client 
Input: 1234567
Recv 8 bytes: 1234567 . From IP 127.0.0.1 , Port 8080
Input: 123 567 9
Recv 4 bytes:  . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

# 边缘触发案例，注释代码 95 行，取消 94 行的注释，S、C缓冲区大小分别是 4、10
# 可以看到这样子数据接收并不是我们想要的模样，虽然看起来都收到了，看不到的在缓冲区里
# 疑点：1. 客户端一上来就输入“\q”才会在服务端看到 disconnet,如果和服务端说了话在退出就不会看到 disconect
# 2. 客户端输入“123456789”时还正常运行，如果输入“1234567890”就会陷入疯狂输出的异常状态
$ ./server-epoll 
Waiting for connecting
epoll_wait awake    # 主套接字触发
New client：5 , IP 127.0.0.1 , Port 53144
epoll_wait awake    # 第一次客户端消息触发
Recv 4 bytes: 1234 . From IP 127.0.0.1 , Port 53144
epoll_wait awake     # 第二次客户端消息触发
Recv 4 bytes: 567 . From IP 127.0.0.1 , Port 53144
epoll_wait awake    # 客户端断开触发
Recv 4 bytes:  . From IP 127.0.0.1 , Port 53144

$ ./client 
Input: 12234567
Recv 4 bytes: 1223 . From IP 127.0.0.1 , Port 8080
Input: 123 567 9
Recv 4 bytes: 4567 . From IP 127.0.0.1 , Port 8080
Input:  \q
Log: Output close
Client close
```
</details>

但是条件触发、边缘触发之间并不说谁绝对的优，而是需要看情况而定，边缘触发它可以分离接收数据和处理数据的时间点，而条件触发想要实现这一点要花费很大成本（触发事件会疯狂冒出来），所以没有人会用条件触发干这种事情，这时候采取边缘触发它不香吗。

实现边缘触发的回声服务器也是可以的，循环读取缓冲区直到缓冲区中没有数据为止，此外，还应该把套接字修改为非阻塞模式，因为边缘触发方式下，以阻塞方式工作的 read/write 可能引起服务端长时间阻塞无响应，边缘触发中务必采用非阻塞的 read/write 


## 结构体与函数
- epoll_create    
    `int epoll_create (int __size)`  
    定义在 <sys/epoll.h> 中，作用是创建 epoll 对像，返回其描述符。需要注意这个描述符最后记得用 close() 来释放它  

- epoll_ctl  
    `int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event *__event)`  
    定义在 <sys/epoll.h> 中，作用是对监听对象进行设置，如添加、删除。错误返回 -1，成功返回 0  
    __epfd：由 epoll_create 创建的 epoll 对像的描述符  
    __op：操作类型，有 EPOLL_CTL_ADD、EPOLL_CTL_DEL、EPOLL_CTL_MOD 三种  
    __fd：需要监听的对象描述符  
    *__event：监听的事件类型

- epoll_wait  
    `int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout)`  
    定义在 <sys/epoll.h> 中，作用是阻塞等待监听对象被触发，错误返回 -1  
    __timeout：超时时间/ms，设置为 -1 表示无限等待

# Q&A
1. select 支持最多多少个客户？如何测试？