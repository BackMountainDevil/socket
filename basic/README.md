# 最简单的 socket 程序

这里实现一个最简单的客户端-服务端（C-S）的 TCP socket 通信。

- [一个简单的Linux下的socket程序](http://c.biancheng.net/cpp/html/3030.html)
## 服务端运行流程
1. socket：建立套接字 socket
2. bind：将套接字绑定到指定端口
3. listen：设置套接字为被动接收请求模式，直到有请求连接才进入下一步，否则一直等待
4. accept：接收连接请求，创建新的套接字
5. 基于新的套接字进行数据收发
6. 若交流结束，关闭套接字，程序结束

## 客户端运行流程
1. socket：建立套接字 socket
2. connect：向指定的套接字发起连接请求
3. 连接成功则进行数据收发
4. 若交流结束，关闭套接字，程序结束

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序
### 如何运行

首先运行服务器程序(server.cpp)，其会等待客户端(client.cpp)发起请求。然后运行客户端程序，客户端连接到服务器后，会收到服务器返回的数据，然后各自结束。

```bash
# 编译两个源代码
$ make all
# 先运行服务端程序
$ ./server

# 新开一个终端，再运行服务端程序
$ ./client
```

输出结果

```bash
$ ./server 
Waiting for connecting
Here comes a new client
Server close

$ ./client 
Connect to server
Message form server: This is server speaking!
Client close
```