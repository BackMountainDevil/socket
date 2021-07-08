# socket
基于 C++ 的 socket 编程。目前仅有 Linux 版，Windows 版随后补上。

- [最简单的 CS 通信](./basic/README.md)：最基本的 Client - Server 通信程序
- [出错处理与函数解析](./error/README.md)：对上一个程序进行出错处理，解释代码中的函数
- [迭代回声 CS 通信](./loop/README.md)：让服务端、客户端保持运行
- [优雅的断开连接](./elegantClose/README.md)：如何确保对方收到全部数据再关闭连接
- [域名和 IP](./dns/README.md)：使用域名替代 IP 改进 迭代回声 CS 通信
- [UDP 迭代回声 CS 通信](./udp/README.md)：将 TCP 改为 UDP
- [Nagle 算法](./nagle/README.md)：是什么，干啥的
- [多进程服务端](./fork/README.md)：改进迭代回声服务端，使其可以同时支持多个客户端
- [I/O 复用服务端](./select-epoll/README.md)
- [多线程服务端](./thread/README.md)

