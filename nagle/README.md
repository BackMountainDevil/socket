# Nagle 算法
Nagle 算法在 1984 年诞生，应用在 TCP 层，为了防止链路上的数据包过多而发生拥堵甚至网络过载。在使用这个算法的时候，在发送数据包之前得等到收到对方确认收到上一个数据包的 ACK 消息。不使用这个算法的话，就刷刷刷往外传，等你确认我再发下一个我菜都凉了。

一般情况下，不使用 Nagle 算法可以提高传输速度，但是大家都不用的话就会发生网络拥塞，反而会影响传输。那什么时候用这个算法，根据传输数据的特性，网络流量未受太大影响时，不使用该算法比用它时要传的快。最典型的是“传输大文件数据”。

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序

程序在[优雅的断开连接](./elegantClose/README.md)的基础修改了一丢丢
### 如何运行

[正常运行步骤](../basic/README.md)和前面的一致，输出结果如下所示，可以看到默认状态下 TCP_NODELAY 的值为 0，也就是说存在 Delay（延迟），说人话就是开启了 Nagle 算法，然后将 TCP_NODELAY 修改为 1 就可以关闭该算法。

```bash
$ ./server 
Nagle status ：0
Buffer length both set to 1025
Nagle status ：1
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 41474
Start to send data 'client.cpp' to client 4
Send data done 
Output stream close 
4：I have recv the data,Thank you
Server close

$ ./client 
Nagle status ：0
Nagle status ：1
Trying to connect server
Connected to server
Recving data
Recv done, data in log.txt
Send to server: I have recv the data,Thank you
Client close
```

### 函数解释
- setsockopt  
    `int setsockopt (int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen)` 
    __level：IPPROTO_TCP 指的是 TCP 层    
    __optname：TCP_NODELAY  指代不要等待直接发，开启是 1, 关闭是 0    
    更多参数解释可参考 [出错处理与函数解析](../error/README.md)中的 setsockopt 

# Q&A
1. Error：Use of undeclared identifier 'TCP_NODELAY'

    没有引入头文件 <netinet/tcp.h>