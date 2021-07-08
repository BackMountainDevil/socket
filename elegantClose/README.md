# 优雅的断开连接

## socket缓冲区以及阻塞模式

- [socket缓冲区以及阻塞模式](http://c.biancheng.net/cpp/html/3040.html)
>   每个 socket 被创建后，都会分配两个缓冲区，输入缓冲区和输出缓冲区。   
    write()/send() 并不立即向网络中传输数据，而是先将数据写入缓冲区中，再由TCP协议将数据从缓冲区发送到目标机器。一旦将数据写入到缓冲区，函数就可以成功返回，不管它们有没有到达目标机器，也不管它们何时被发送到网络，这些都是TCP协议负责的事情。  
    TCP协议独立于 write()/send() 函数，数据有可能刚被写入缓冲区就发送到网络，也可能在缓冲区中不断积压，多次写入的数据被一次性发送到网络，这取决于当时的网络情况、当前线程是否空闲等诸多因素，不由程序员控制。  
    read()/recv() 函数也是如此，也从输入缓冲区中读取数据，而不是直接从网络中读取。   
    - 即使关闭套接字也会继续传送输出缓冲区中遗留的数据；  
    - 关闭套接字将丢失输入缓冲区中的数据。

阻塞模式主要是缓冲区大小有限、数据大小放不放的下、缓冲区有没有正在被使用（锁定）。
1. 数据大小比缓冲区可用空间大一丢丢，阻塞等待可用空间足够且没有被锁定的时候才写入缓冲区；
2. 数据大小远大于缓冲区大小，分批次写入；
3. 缓冲区正在被使用（锁定-发送、写入）

## 取得socket 状态 - 缓冲区大小
通过 `getsockopt()` 中的第三个参数 `__optname` 可以获取到对于的参数值，下面是一个获取缓存区的例子。way 1 在我pc上获取到的值保持在 16384，而 way 2 获取到的值一直在 3.2k 左右变动，极少数的时候数值会突变（原因有待探究，类型转换？）


## 为什么需要优雅的断开连接
c - s TCP socket 中会存在两条连接：  
1. client 的输入流 与 server 的输出流的连接通道
2. client 的输出流 与 server 的输入流的连接通道

调用 close() 函数关闭套接字意味着完全断开连接，即不能发送数据，也不能接收数据。但是某些时候我只想听你说，不想说话，会有一种半连接状态存在。

以文件传输为一个例子， server 要给 client 发一个文件，但是 client 并不知道要保持接收数据到什么时候，究竟什么时候传输完成？client 要是循环打开

- [socket文件传输功能的实现](http://c.biancheng.net/cpp/html/3045.html)
- [段错误 (核心已转储) + free(): double free detected in tcache 2 已放弃 (核心已转储). Kearney form An idea 2021-07-05](https://blog.csdn.net/weixin_43031092/article/details/118487981)

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序
### 如何运行

[正常运行步骤](../basic/README.md)和前面的一致，输出结果如下

客户端和服务端建立连接之后，服务端会向客户端发送文件，文件发完之后关闭输出流，告诉客户端我发完啦，只保留输入流等待客户端说“我全部收到了”；客户端收到文件的全部数据之后就会向服务端发送接收完毕的消息，之后双方断开连接，结束程序。

两个程序都演示了如何获取和设置缓冲区大小，但是缓冲区大小设置明显可以从输出中看到没有设置的值有些不对劲，但是没有报错也就是说明设置生效了，但是看起来数值明显对不上啊。

```bash
$ ./server 
Send buffer length ：16384
Recv buffer length ：131072
Buffer length both set to 1025
Send buffer length ：4608
Recv buffer length ：2304
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 41108
Start to send data 'client.cpp' to client 4
Send data done 
Output stream close 
4：I have recv the data,Thank you
Server close

$ ./client 
Send buffer length ：16384
Recv buffer length ：131072
Buffer length both set to 1025
Send buffer length ：4608
Recv buffer length ：2304
Trying to connect server
Connected to server
Recving data
Recv done, data in log.txt
Send to server: I have recv the data,Thank you
Client close
```

### 函数解释

- shutdown  
    `int shutdown (int __fd, int __how)`  
    定义在 <socket.h> 中，作用是将套接字（__fd）的某些功能关闭。失败时返回 -1,成功时返回 0  
    __how：SHUT_RD 代表关闭输入流，SHUT_WR 关闭输出流， SHUT_RDWR 则是都关了  
    断开输出流会像对方发送 EOF

- getsockopt  
    `int getsockopt (int __fd, int __level, int __optname, void *__restrict __optval,      socklen_t *__restrict __optlen)`  
    定义在 <socket.h> 中，作用是获取套接字某个设置项的值，失败返回 -1,成功返回 0  
    参数解释可参考 [出错处理与函数解析](../error/README.md)中的 setsockopt  

# Q&A
1. 为什么设置缓冲区大小成功了，但是读取出来的参数值不是我设置的大小呢？
