# socket
基于 Linux 的 socket 编程。

## 最简单的 CS 通信
使用`git checkout basic`签出对于版本的代码。首先运行服务器程序(server.cpp)，其会等待客户端(client.cpp)发起请求。然后运行客户端程序，客户端连接到服务器后，会收到服务器返回的数据，然后各自结束。

[一个简单的Linux下的socket程序](http://c.biancheng.net/cpp/html/3030.html)

## 向服务端发送并接收数据
使用`git checkout basic2`签出对于版本的代码。
在上一步的基础上增加了向服务端发生自定义数据的功能，然后服务端也增加了接收数据的功能，接收之后再将数据返回给客户端。