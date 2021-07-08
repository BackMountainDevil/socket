# 域名和 IP

NDS：域名系统。用处是通过域名查询对于的 IP地址

为什么要用 DNS？因为域名好记，IP 地址都是数字谁记得住。。而且域名对应的 IP 可以随时修改（域名解析），而且通常来说，域名变动的次数大于 IP 变更的概率。

- [在socket中使用域名](http://c.biancheng.net/cpp/html/3048.html)
- [gethostbyname()函数详解.带鱼兄 2016-07-18](https://blog.csdn.net/daiyudong2020/article/details/51946080?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522162546433016780261960009%2522%252C%2522scm%2522%253A%252220140713.130102334..%2522%257D&request_id=162546433016780261960009&biz_id=0)
> gethostname可能存在性能瓶颈问题


## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序
- getIP.cpp: 由 域名 获取对于的 IP 地址

服务端程序和客户端程序相对于[迭代回声 CS 通信](../loop/README.md)中的代码并未作太多更改，只是使用加入了由域名获取 IP 这一步，删除了原有的写死 IP 代码

### 如何运行

客户端和服务端的[运行步骤](../basic/README.md)和前面的一致，这里客户端输入 ‘\q’ 则会结束程序，服务端需要 Ctrl + c 来手动关闭。这里演示下 getIP

```bash
$ ./getIP 
Official: www.a.shifen.com
Aliases 1: www.baidu.com
Address type: AF_INET
IP addr 1: 110.242.68.3
IP addr 2: 110.242.68.4
```

### 函数解释
- gethostbyname  
    `struct hostent *gethostbyname (const char *__name)`  
    定义在 <netdb.h> 中，作用是从域名数据库中获取域名对于的解析信息     
