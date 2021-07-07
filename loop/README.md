# 迭代回声 CS 通信

之前的服务端、客户端允许一下就结束了，这里再修改一下，能让两者都保持允许。实现两者保持运行只要加个循环即可，除非输入特定指令结束程序。这个程序实现了 CS 一对一回声，还不支持多个客户端，而且一个客户端也是由不断杀死新生的小小 socket 组成的。
- 迭代服务端：循环 accept, read/write, close
- 回声：服务端将收到的数据发回给客户端
- 迭代客户端：循环 socket, connect, write/read, close；直到用户输入 “\q” 才结束程序，否则就一直将客户输入的信息发给服务端

- [实现迭代服务器端和客户端](http://c.biancheng.net/cpp/html/3039.html)

## Linux socket 
- server.cpp: 服务端程序
- client.cpp: 客户端程序
### 如何运行

[正常运行步骤](../basic/README.md)和前面的一致，这里客户端输入 ‘\q’ 则会结束程序，服务端需要 Ctrl + c 来手动关闭。一个可能的输出结果如下

```bash
$ ./server 
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 38030
4：asd
New client：4 , IP 127.0.0.1 , Port 38032
4：asdv
New client：4 , IP 127.0.0.1 , Port 38034
4：\q
```

另一个终端

```bash
$ ./client 
Input: asd
Recv: asd
Input: asdv
Recv: asdv
Input: \q
Log: Output close
Client close
```

### 函数解释

- memset  
    `void *memset (void *__s, int __c, size_t __n)`  
    定义在 <string.h> 中，作用是将 `__s` 的前 `__n` 个字节重置为 `__c`。

- cin.getline  
    `__istream_type& getline(char_type* __s, streamsize __n)`  
    定义在 <istream> 中，作用是将输入流的 `__n` 个字节的数据存到 ` __s` 中。由于scanf() 读取到空格时认为输入结束，考虑下 fgets() 或者 std::cin.getline()。gets() 已经被 C11 标准抛弃，不建议使用  
    ```bash
    scanf("%s", buffer);
    fgets(buffer,fgets(buffer,BUF_SIZE,stdin);,stdin);    //  C: fgets() 会读取换行符'\n'，除非缓存区不够大
    std::cin.getline(buffer, BUF_SIZE); // C++
    ```

    - [c语言gets()函数与它的替代者fgets()函数](https://www.cnblogs.com/qingergege/p/5925811.html)
    - [PAT B1009 error: ‘gets’ was not declared in this scope gets(s)之解决办法 一只小菜猪 2019-01-24](https://blog.csdn.net/qq_36525099/article/details/86631881)
