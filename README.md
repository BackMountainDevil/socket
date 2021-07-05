# socket
基于 Linux 的 socket 编程。

## 最简单的 CS 通信
使用`git checkout basic`签出对应版本的代码。首先运行服务器程序(server.cpp)，其会等待客户端(client.cpp)发起请求。然后运行客户端程序，客户端连接到服务器后，会收到服务器返回的数据，然后各自结束。

[一个简单的Linux下的socket程序](http://c.biancheng.net/cpp/html/3030.html)

## 向服务端发送并接收数据
使用`git checkout basic2`签出对应版本的代码。
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
使用`git checkout basicError`签出对应版本的代码。
- [Socket Programming in C/C++. 31 May, 2019](https://www.geeksforgeeks.org/socket-programming-cc/)
- [inet_pton()和inet_ntop()函数详解. QvQ是惊喜不是哭泣 2017-02-28](https://blog.csdn.net/zyy617532750/article/details/58595700)
- [C 库函数 - perror()](https://www.runoob.com/cprogramming/c-function-perror.html)
- [C语言setsockopt()函数：设置socket状态](http://c.biancheng.net/cpp/html/374.html)

## 迭代服务器端和客户端
使用`git checkout basicCS`签出对应版本的代码。之前的版本不能实现持续连接，刚说完一句话就通话中断了。这里使用循环改进了一下代码，使得服务端一直保持运行状态（等待客户发起连接）直到被终止程序，而客户端方面则是不断的向服务端发起连接，将用户输入发到服务端再接收服务端返回的数据。

需要注意的是，这里的“不中断连接”并非真正意义上的持续性连接，而是不断的连接-关闭-连接，往复循环的连接让用户以为自己是一直和服务端保持连接的。这版的程序支持多个客户端程序同时运行。当多个服务端同时运行的时候，并不会异常报错，而是都会监听端口，至于客户连接到的服务端到底是哪一个，随缘。

- [实现迭代服务器端和客户端](http://c.biancheng.net/cpp/html/3039.html)
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

### 取得socket 状态 - 缓冲区大小
通过 `getsockopt()` 中的第三个参数 `__optname` 可以获取到对于的参数值，下面是一个获取缓存区的例子。way 1 在我pc上获取到的值保持在 16384，而 way 2 获取到的值一直在 3.2k 左右变动，极少数的时候数值会突变（原因有待探究，类型转换？）

```bash
int optval;
socklen_t tmp = sizeof(optval);
getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&optval, &tmp); // way 1
// getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&optval, (socklen_t *)sizeof(optval));  // way 2
printf("Buffer length = %d\n", optval);
```

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