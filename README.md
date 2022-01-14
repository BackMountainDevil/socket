# socket
基于 C++ 的 socket 编程。目前仅有 Linux 版，Windows 版不在计划中。

- [最简单的 CS 通信](./basic/README.md)：最基本的 Client - Server 通信程序
- [出错处理与函数解析](./error/README.md)：对上一个程序进行出错处理，解释代码中的函数
- [迭代回声 CS 通信](./loop/README.md)：让服务端、客户端保持运行
- [优雅的断开连接](./elegantClose/README.md)：如何确保对方收到全部数据再关闭连接
- [域名和 IP](./dns/README.md)：使用域名替代 IP 改进 迭代回声 CS 通信
- [UDP 迭代回声 CS 通信](./udp/README.md)：将 TCP 改为 UDP
- [Nagle 算法](./nagle/README.md)：是什么，干啥的
- [多进程服务端](./fork/README.md)：改进迭代回声服务端，使其可以同时支持多个客户端
- [I/O 复用服务端](./select-epoll/README.md)：使用复用技术改进回声服务端
- [多线程服务端](./thread/README.md)：多线程实现回声服务端、客户端
- [多线程群聊](./groupchat/README.md)：多线程实现的群聊
- [多播和广播](./broadcast/README.md)

## [TCP的粘包问题以及数据的无边界性](http://c.biancheng.net/cpp/html/3041.html)
数据的“粘包”问题：由于缓冲区和阻塞模式的存在，read 读取缓冲区的时候并不是说缓冲区一有数据就读取，而是阻塞等待到缓冲区快满了的时候才去读取缓冲区的数据（为了尽可能多的接收数据）。换句话说，我本来是分批次给你发消息，然而你以为我是连起来说的，或者认为我说话前言不搭后语。
> 例如，write()/send() 重复执行三次，每次都发送字符串"abc"，那么目标机器上的 read()/recv() 可能分三次接收，每次都接收"abc"；也可能分两次接收，第一次接收"abcab"，第二次接收"cabc"；也可能一次就接收到字符串"abcabcabc"。

服务端延迟 10s 再原样返回数据 且 客户端一下子发生好多条数据 的办法按照网站上的修改应该会出现“粘包”现象，然而它的结果既不是“粘包”也不是返回多条数据，而是只返回了一条数据。。

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
