# UDP 迭代回声 CS 通信
- TCP：有连接、有丢失重传机制、有流量控制
- UDP：无连接、不可靠

UDP 和 TCP 相比，省略去了建立连接的时间，类似与拿着大喇叭喊话，谁在线谁就会听到，听不到就算了。TCP 和 UDP 之间没有绝对的好坏，如果允许数据传输丢包的话可以用 UDP，UDP 多用在视频、音频传输质量要求不高的情况。题外话，QQ 当年起家靠的就是这玩意。 

- [基于UDP的服务器端和客户端](http://c.biancheng.net/cpp/html/3052.html)
- [Linux C/C++ UDP Socket通信实例](https://www.cnblogs.com/zkfopen/p/9382705.html)

## 服务端运行流程
1. socket：建立套接字 socket
2. bind：将套接字绑定到指定接口
3. recvfrom：接受接口收到的数据
4. （可选）sendto：向指定接口发送数据

## 客户端运行流程
1. socket：建立套接字 socket
2. sendto：向指定接口发送数据
3. （可选）recvfrom：接受接口收到的数据

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序

### 如何运行

客户端和服务端的[运行步骤](../basic/README.md)和前面的一致，这里客户端输入 ‘\q’ 则会结束程序，服务端需要 Ctrl + c 来手动关闭。支持多个客户端。

首先运行服务端程序，然后再运行客户端程序，不然客户端就会傻傻的等待服务端返回数据；即使后面一次加入新的服务端、客户端，也无法拯救这个客户端。

```bash
$ ./server 
Recv 9 bytes: 阿斯顿���U from IP 127.0.0.1 , Port 45011
Recv 5 bytes: 22而 from IP 127.0.0.1 , Port 54994
Recv 7 bytes: ada asd from IP 127.0.0.1 , Port 54847
Recv 8 bytes:  asd ads from IP 127.0.0.1 , Port 44257
Recv 6 bytes: as ads from IP 127.0.0.1 , Port 35966
Recv 2 bytes: \q from IP 127.0.0.1 , Port 36946
Recv 2 bytes: \q from IP 127.0.0.1 , Port 40167
^C

$ ./client 
Input: 22而
Recv 5 bytes: 22而 from IP 127.0.0.1 , Port 8080
Input: ada asd
Recv 7 bytes: ada asd from IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

$ ./client 
Input: 阿斯顿
Recv 9 bytes: 阿斯顿 from IP 127.0.0.1 , Port 8080
Input:  asd ads
Recv 8 bytes:  asd ads from IP 127.0.0.1 , Port 8080
Input: as ads
Recv 6 bytes: as ads from IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close
```


### 相关解释
- INADDR_ANY  
    `#define	INADDR_ANY		((in_addr_t) 0x00000000)`  
    定义在 <in.h> 中，用来指接受来自任意地址的消息

- sendto  
    `ssize_t sendto (int __fd, const void *__buf, size_t __n, int __flags, __CONST_SOCKADDR_ARG __addr, socklen_t __addr_len)`  
    定义在 <scoket.h> 中，用来从套接字向目标接口发送缓冲区中的数据。如果调用此函数时尚未分配地址信息，则会在首次调用此函数的时候给相应套接字自动分配 IP 和端口    
    __fd：用来发送数据的套接字的描述符   
    __buf：待发送的数据的地址  
    __n：传输的数据大小，单位是字节  
    __flags：可选参数，无就填 0  
    __addr：目标接口的结构体变量的地址  
    __addr_len：__addr的长度  

- recvfrom  
    ` ssize_t recvfrom (int __fd, void *__restrict __buf, size_t __n, int __flags, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len)`  
    定义在 <scoket.h> 中，用来从套接字读取数据到缓冲区  