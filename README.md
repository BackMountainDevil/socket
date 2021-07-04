# socket
基于 Linux 的 socket 编程。

## 最简单的 CS 通信
使用`git checkout basic`签出对于版本的代码。首先运行服务器程序(server.cpp)，其会等待客户端(client.cpp)发起请求。然后运行客户端程序，客户端连接到服务器后，会收到服务器返回的数据，然后各自结束。

[一个简单的Linux下的socket程序](http://c.biancheng.net/cpp/html/3030.html)

## 向服务端发送并接收数据
使用`git checkout basic2`签出对于版本的代码。
在上一步的基础上增加了向服务端发生自定义数据的功能，然后服务端也增加了接收数据的功能，接收之后再将数据返回给客户端。

scanf() 读取到空格时认为输入结束，考虑下 fgets() 或者 std::cin.getline()。gets() 已经被抛弃，不建议使用。
```bash
scanf("%s", buffer);
fgets(buffer,fgets(buffer,BUF_SIZE,stdin);,stdin);    //  C: fgets() 会读取换行符'\n'，除非缓存区不够大
std::cin.getline(buffer, BUF_SIZE); // C++
```

- [c语言gets()函数与它的替代者fgets()函数](https://www.cnblogs.com/qingergege/p/5925811.html)
- [PAT B1009 error: ‘gets’ was not declared in this scope gets(s)之解决办法 一只小菜猪 2019-01-24](https://blog.csdn.net/qq_36525099/article/details/86631881)

## 错误处理
使用`git checkout basicError`签出对于版本的代码。
- [Socket Programming in C/C++. 31 May, 2019](https://www.geeksforgeeks.org/socket-programming-cc/)
- [inet_pton()和inet_ntop()函数详解. QvQ是惊喜不是哭泣 2017-02-28](https://blog.csdn.net/zyy617532750/article/details/58595700)
- [C 库函数 - perror()](https://www.runoob.com/cprogramming/c-function-perror.html)
- [C语言setsockopt()函数：设置socket状态](http://c.biancheng.net/cpp/html/374.html)