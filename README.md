# socket
基于 C++ 的 socket 编程。目前仅有 Linux 版，Windows 版随后补上。

- [最简单的 CS 通信](./basic/README.md)：最基本的 Client - Server 通信程序
- [出错处理与函数解析](./error/README.md)：对上一个程序进行出错处理，解释代码中的函数
- [迭代回声 CS 通信](./loop/README.md)：让服务端、客户端保持运行
- [优雅的断开连接](./elegantClose/README.md)：如何确保对方收到全部数据再关闭连接
- [域名和 IP](./dns/README.md)：使用域名替代 IP

## [TCP的粘包问题以及数据的无边界性](http://c.biancheng.net/cpp/html/3041.html)
数据的“粘包”问题：由于缓冲区和阻塞模式的存在，read 读取缓冲区的时候并不是说缓冲区一有数据就读取，而是阻塞等待到缓冲区快满了的时候才去读取缓冲区的数据（为了尽可能多的接收数据）。换句话说，我本来是分批次给你发消息，然而你以为我是连起来说的，或者认为我说话前言不搭后语。
> 例如，write()/send() 重复执行三次，每次都发送字符串"abc"，那么目标机器上的 read()/recv() 可能分三次接收，每次都接收"abc"；也可能分两次接收，第一次接收"abcab"，第二次接收"cabc"；也可能一次就接收到字符串"abcabcabc"。

服务端延迟 10s 再原样返回数据 且 客户端一下子发生好多条数据 的办法按照网站上的修改应该会出现“粘包”现象，然而它的结果既不是“粘包”也不是返回多条数据，而是只返回了一条数据。。
使用`git checkout stuck`签出对应版本的代码。
```bash
$ ./server
Buffer length = 16384
Message form client: 123 456 789 asdxc asd

$ ./client  # 客户端中有循环会连续发生 20 次数据，10s 后收到服务器返回的数据
Input a string: 123 456 789 asdxc asd
Message form server: 123 456 789 asdxc asd
Input a string: asd
Message form server: asd
```


## UDP
UPD 和 TCP 相比，省略去了建立连接的时间，类似与拿着大喇叭喊话，谁在线谁就会听到。

使用`git checkout udp`签出对应版本的代码。首先运行服务端程序，然后再运行客户端程序，不然客户端就会傻傻的等待服务端返回数据；即使后面一次加入新的服务端、客户端，也无法拯救这几个傻子。

- [基于UDP的服务器端和客户端](http://c.biancheng.net/cpp/html/3052.html)
- [Linux C/C++ UDP Socket通信实例](https://www.cnblogs.com/zkfopen/p/9382705.html)


## 聊天室 CSC
在多客户聊天室中，服务端作为消息的转发者，接受来自不同客户的消息，随后将消息转发至对应的客户。为了达到这一目的，服务端需要存储所有客户端的连接和标识。

### select

```cpp
// #include <sys/time.h> 
fd_set readfds;

// 清空文件描述符集合（全部置零）
FD_ZERO(&readfds);  

// 向集合中对于位置添加文件描述符
FD_SET(master_sock, &readfds);   

// 从集合中清除文件描述符
FD_CLR(master_sock, &readfds); 

// 检查关心的文件描述符是否有读事件发生
FD_ISSET(master_sock, &readfds); 
```

`select()` 命令可以从多个客户连接中选出活跃的那个（如果有的话），像中断控制器。

- [C++ socket 网络编程 简单聊天室. Thanks_up.  2019-03-13](https://www.cnblogs.com/DCD112358/p/10522172.html):从基本的CS 到 CSCs（select）,最后谈到优化（epoll、线程池）: select + keyNode
- [Socket Programming in C/C++: Handling multiple clients on server without multi threading. 30 May, 2018](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/): select + fd_set

# 参考
- [C/C++ socket编程教程：1天玩转socket通信技术](http://c.biancheng.net/cpp/socket/):主要是 windows 版本的，但是基本原理差不多
- [Socket Programming in C/C++. 31 May, 2019](https://www.geeksforgeeks.org/socket-programming-cc/)：Linux 版本的 socket，照此修改了不少，学习了不少错误处理
- [C语言技术网 - 网络通信socket。- 码农有道](http://www.freecplus.net/0047ac4059b14d52bcc1d4df6ae8bb83.html)