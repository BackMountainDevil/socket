# 出错处理与函数解析

有的时候程序不一定能正常运行，这个时候需要我们对异常、错误做一些处理。

- [Socket Programming in C/C++. 31 May, 2019](https://www.geeksforgeeks.org/socket-programming-cc/)
- [C语言技术网 - 网络通信socket。- 码农有道](http://www.freecplus.net/0047ac4059b14d52bcc1d4df6ae8bb83.html)：错误处理比上一个多一个步骤：每次出错都会尝试关闭 socket
- [inet_pton()和inet_ntop()函数详解. QvQ是惊喜不是哭泣 2017-02-28](https://blog.csdn.net/zyy617532750/article/details/58595700)
- [C 库函数 - perror()](https://www.runoob.com/cprogramming/c-function-perror.html)
- [C语言setsockopt()函数：设置socket状态](http://c.biancheng.net/cpp/html/374.html)

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序
### 如何运行

[正常运行步骤](../basic/README.md)和上一节一致，这里主要将错误处理，因此讲不正常的如何运行。

```bash
# 未运行服务端时运行客户端的情况
$ ./client 
Error: Connection creation failed: Connection refused
```

### 函数解析
- perror  
    `void perror (const char *__s)`   
    定义在 <stdio.h> 中，作用是输出参数 `*__s` ，然后输出上一个错误的的描述信息，具体例子上面已经有了。冒号之前的 `Error: Connection creation failed` 是自定义的参数，冒号之后的是错误的具体信息 - `Connection refused`

- socket  
    `int socket (int __domain, int __type, int __protocol)`  
    定义在 <socket.h> 中，作用是创建套接字。创建失败时返回 -1,成功时返回套接字描述符。  
    __domain：协议族。AF_INET 指的是 TCP/IP 协议  
    __type：通信服务类型。SOCK_STREAM：TCP，SOCK_DGRAM: UDP  
    __protocol：协议号。前两个参数基本能确定一个协议，在 TCP/IP 协议中 0 表示使用 TCP 协议

- setsockopt  
    `int setsockopt (int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen)`  
    定义在 <socket.h> 中，作用是设置套接字的某些选项的值。这一步骤不是必须的，只不过可以用来设置一些参数（缓冲区大小）和避免某些错误，如接口已经被占用。失败时返回 -1,成功返回 0  
    __fd：要被设置的套接字的描述符  
    __level：选项的所在的协议层，设置套接字时该参数应为 SOL_SOCKET  
    __optname：要设置的参数的名称，SO_REUSEADDR 指的是允许复用地址，SO_REUSEPORT 代表允许复用端口  
    __optval：要设置的参数值的地址    
    __optlen：参数值的长度

- bind  
    `int bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)`  
    定义在 <socket.h> 中，作用是将套接字关联到本地的某个接口（IP+端口号），出错时返回 -1  
    __fd：套接字描述符，即 socket() 函数的返回值  
    __addr：指向结构体 sockaddr_in 的指针，该结构体包含了接口的信息  
    __len：__addr 的长度

- listen  
    `int listen (int __fd, int __n)`
    定义在 <socket.h> 中，作用是使套接字处于被动监听状态，直到有客户端发起连接请求才会被“唤醒”。失败时返回 -1,成功返回 0  
     __fd: 套接字描述符  
    __n：最大连接数，假设是 3,那么在有 3 个用户已经连接上的时候，当第 4 个客户尝试连接的时候，刚想踏进门，立马被踢出去

- accept  
    `int accept (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len)`  
    定义在 <socket.h> 中，作用是当心里新连接来的时候创建新的套接字。失败时返回 -1,成功返回新套接字的描述符  
    __fd：套接字描述符  
    __addr：指向用来存放新套接字信息的结构体 sockaddr_in 的指针  
    __addr_len：

- write  
    `ssize_t write (int __fd, const void *__buf, size_t __n)`  
    定义在 <unistd.h> 中，作用是向套接字发送 `__buf` 中的 `__n` 个字节的数据。失败时返回 -1,成功返回发送数据的字节数  
    __fd：套接字描述符  
    __buf：要发送的数据的缓冲区  
    __n：发送数据的字节数

- read  
    `ssize_t read (int __fd, void *__buf, size_t __nbytes)`  
    定义在 <unistd.h> 中，作用是从套接字读取 `__nbytes` 个字节的数据到 `__buf` 中。失败时返回 -1, EOF（End Of File） 时返回0，成功返回读取数据的字节数  
    __fd：套接字描述符  
    __buf：要存读取的数据的缓冲区  
    __nbytes：读取数据的字节数


# Q&A
1. 为什么我这里的代码只引入了四个头文件？ perror 的头文件都没有引入，是不是写错了？
    
    其实这是后面我发现这些头文件引入太多了，多的时候有十几个，然后我一个一个注释掉再去编译运行，发现不影响编译运行的我就删掉了，至于表面上看似没有引入，但是又能编译、运行成功，那就可以推测这几个头文件里悄咪咪的互相包含了那些看上去没有引起的头文件吧。

