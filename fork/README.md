# 多进程服务器

 并发服务器的三种办法：多进程、多路复用、多线程。  
 在这里讨论多进程服务器，这种方法不适合在 win 平台。
## 程序
### fork.c
克隆进程和子进程的案例:演示进程是如何裂开的？从哪里裂开的？

### zombie.c
进程在完成工作后应当被销毁，但有的时候没有被即使销毁的程序会变成僵尸进程，这些僵尸进程还占用这操作系统的资源。该程序演示了一个僵尸进程的出生与死亡。

```bash
# 查看用户进程，未运行程序前，无挂进程被我隐去啦
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    25662  0.0  0.0  13336  3480 pts/3    R+   17:16   0:00 ps au

$ ./zombie 
This PID: 66258, parent's pid: 65330, pid -1
Begin to fork
Fork done
This PID: 66258, parent's pid: 65330, pid 66259  from parent proc
Fork done
This PID: 66259, parent's pid: 66258, pid 0  from child proc
This PID: 66259, parent's pid: 66258, pid 0
This PID 66259, pid 0: exit
# 这里会有 10 s 的挂机等待，在这个时候开启另一个终端查看用户进程
This PID: 66258, parent's pid: 65330, pid 66259
This PID 66258, pid 66259: exit
```

可以从下面看到 COMMAND 中与 PID 25709 对应的正是我们的程序 `./zombie`，而 PID 25709 对应的子进程早已运行完成，但它就是没有被销毁，因此变成了僵尸进程，在 COMMAND 被标记为 `[zombie] <defunct>`， STAT 变为 `Z+`。当程序的父子进程都运行结束的时候，这个僵尸进程被父进程销毁，是的，被父进程销毁了，而不是在子进程运行结束的时候被操作系统销毁。

```bash
# 查看用户进程，运行程序中，程序处于那个挂机状态
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    66258  0.0  0.0   2364   740 pts/4    S+   22:17   0:00 ./zombie
kearney    66259  0.0  0.0      0     0 pts/4    Z+   22:17   0:00 [zombie] <defunct>
```

#### 后台运行
有的时候程序并不需要关心其输出，在 COMMAND 后面加上 `&` 来将程序放在后台运行，这样就不用再开一个终端，或者担心进程被关掉（ctrl + c 或者关掉终端）。这里还是以僵尸程序为例。下面将程序放在后台运行，其输出还是输出来，按回车就行，不用太在意（按理说不是输出也被隐藏到后台了吗）。

```bash
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    29565  0.0  0.0  13336  3536 pts/2    R+   17:29   0:00 ps au

$ ./zombie &
[1] 30480

# 这不是我的输入，它自己冒出来的
$ This PID: 30480, parent's pid: 11790, pid -1
Begin to fork
Fork done
This PID: 30480, parent's pid: 11790, pid 30481  from child proc
Fork done
This PID: 30481, parent's pid: 30480, pid 0  from child proc
This PID: 30481, parent's pid: 30480, pid 0
This PID: 30481, parent's pid: 30480, pid 0  from child proc
This PID 30481, pid 0: exit

# ' 这一行我打着玩的

$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    30480  0.0  0.0   2364   672 pts/2    S    17:32   0:00 ./zombie
kearney    30481  0.0  0.0      0     0 pts/2    Z    17:32   0:00 [zombie] <defunct>
kearney    30514  0.0  0.0  13336  3464 pts/2    R+   17:32   0:00 ps au

# 这也不是我的输入，它自己冒出来的
$ This PID: 30480, parent's pid: 11790, pid 30481
This PID: 30480, parent's pid: 11790, pid 30481  from child proc
This PID 30480, pid 30481: exit

[1]+  已完成               ./zombie
```


### wait.c
使用 wait 销毁已经运行完成的子进程，需要注意的是，调用 wait 函数的时候，如果没有已终止的子进程，那么程序将阻塞直到有子进程终止为止！谨慎使用！这个程序与 fork.c 相比，使用了 wait 及时的销毁了子进程，释放了其占用的资源（我也不知道是啥资源），不必再等到主进程结束的时候再来销毁子进程。

```bash
# 稍微计时就会发现程序运行了大约 15s。主程序为了等待那个 10s 的子进程结束，阻塞等待了 10s。
$ ./wait
This PID: 8698, parent's pid: 1947, pid -1
This PID: 8698, parent's pid: 1947, pid 8699  from parnet proc
This PID: 8699, parent's pid: 8698, pid 0  from child proc
This PID: 8698, parent's pid: 1947, pid 8700  from parnet proc
waiting
This PID: 8700, parent's pid: 8698, pid 0  from child proc
# 这里挂机了 5s，等待子进程结束
This child normal terminated, return  11
waiting
# 这里挂机了 5s，等待子进程结束，为什么不是再等 10s，因为子进程已经运行 5s 了，还剩下 5s
This child normal terminated, return  22
# 这里挂机了 5s
This PID: 8698, parent's pid: 1947, pid 8700 exit

# 另一个终端会话中，此时程序正在挂机，但是可以看到下面没有出现僵尸进程。
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     8698  0.0  0.0   2364   676 pts/1    S+   19:22   0:00 ./wait
kearney     8699  0.0  0.0   2364    88 pts/1    S+   19:22   0:00 ./wait
kearney     8700  0.0  0.0   2364    88 pts/1    S+   19:22   0:00 ./wait

[kearney@arch cplus]$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     8698  0.0  0.0   2364   676 pts/1    S+   19:22   0:00 ./wait
kearney     8700  0.0  0.0   2364    88 pts/1    S+   19:22   0:00 ./wait

$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     8698  0.0  0.0   2364   676 pts/1    S+   19:22   0:00 ./wait
```

### waitpid.c
waitpid 函数也能销毁子进程，而且不会引起阻塞。wait 为了确保在子进程结束的时候销毁子进程，引入了阻塞等待；而 waitpid 虽然不会阻塞等待，但是并不能确保在子进程结束的时候销毁它。


```bash
$ ./waitpid 
This PID: 6756, parent's pid: 1947, pid -1
This PID: 6756, parent's pid: 1947, pid 6757  from parnet proc
This PID: 6757, parent's pid: 6756, pid 0  from child proc
3s passed
3s passed
3s passed
This child return  11
This PID: 6756, parent's pid: 1947, pid 6757 exit

# 另一个终端，程序运行 9s 内，父子进程都在 run
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     6756  0.0  0.0   2364   676 pts/1    S+   19:09   0:00 ./waitpid
kearney     6757  0.0  0.0   2364    92 pts/1    S+   19:09   0:00 ./waitpid
kearney     6763  0.0  0.0  13336  3516 pts/3    R+   19:09   0:00 ps au

# 另一个终端，程序运行 9s ～ 14s 内，子进程 6757 已经被销毁，没有变成僵尸进程
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     6756  0.0  0.0   2364   676 pts/1    S+   19:09   0:00 ./waitpid
kearney     6783  0.0  0.0  13336  3596 pts/3    R+   19:09   0:00 ps au
```

### signal.c
上面我们用 `ps au` 查看用户进程中有没有僵尸进程，也就是说操作系统知道子进程能识别子进程是否终止，那么当操作系统发现终止的时候告诉程序，然后销毁子进程，释放其占用资源，就可以避免僵尸进程了。

用 signal 来处理大概是这样 - `signal(SIGCHLD, killchild)`，但是这个函数还不够优秀，这里并不会这么做，只是演示下这个函数的作用。signal.c 演示的是以下两个功能  
- 当用完 alarm 函数注册的时间，调用 timeout 函数  
- 当按下 ctrl+c 时，调用 interrupt 函数

程序中循环了 3 次 休眠（阻塞）100s，实际程序运行也就不到 7s，这是因为触发信号时将唤醒由于调用 sleep 函数而进入阻塞状态的进程。调用函数的主体是操作系统，但是处于阻塞状态下的进程是无法调用其中的函数的。因此，产生信号的时候，为了调用对应的函数，将唤醒处于阻塞状态的进程，而且进程一旦被唤醒，就不会再进入阻塞状态，即使未到达 sleep 函数中设置的时间也是如此。对应到程序中就是每次 for 循环刚进入阻塞状态不久就由于信号被触发而被唤醒，并且不会再次进入阻塞状态，而是继续程序，进入下一次循环。

```bash
# 不输入的情况，就让它自己运行
$ ./signal 
wait..
Time out
wait..
Time out
wait..
Time out

# 当按下 ctrl+c 时，有时候没注意按第三次程序都结束了
$ ./signal 
wait..
^CTime out
wait..
^CTime out
wait..
^CTime out
```

### sigaction
signal 函数足以用来解决僵尸进程问题，但是它在不同的类 UNIX 系统里的实现并不完全相同，而 sigaction 函数作用和前者类似，且其在不同的类 UNIX 系统里的实现完全相同

- sigaction.c：演示了如何使用 sigaction 设置信号和回调函数，运行结果与 signal.c  类似  
- zombie-sig.c:使用 sigaction 解决僵尸进程问题  
   注册子进程终止信号 SIGCHLD 的处理函数，然后用 waitpid 尝试去销毁它  
   ```bash
   # 试着用 `ps au` 看一下有没有僵尸进程
   $ ./zombie-sig 
   This PID: 63942, parent's pid: 53290, pid -1
   This PID: 63942, parent's pid: 53290, pid 63943  from parnet proc
   wait..
   This PID: 63943, parent's pid: 63942, pid 0  from child proc
   wait..
   wait..
   wait..
   This PID: 63942, parent's pid: 53290, pid 63943 exit
   ``` 

### 多进程并发服务端
在 [域名和 IP](../dns/README.md)中使用域名替代 IP 改进了[迭代回声 CS 通信](../loop/README.md)，但依然只能和一个客户端保持正常连接。这里僵尸用多进程实现并发回声服务器。

[server.cpp](server.cpp):多进程并发服务端  
[client.cpp](client.cpp):客户端  

```bash
# 编译服务端、客户端
$ make cs

# 运行服务端，然后打开其它终端
$ ./server 
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 39642
New client：4 , IP 127.0.0.1 , Port 39644
New client：4 , IP 127.0.0.1 , Port 39646
Recv 1025 bytes: c1 . From IP 127.0.0.1 , Port 39642
New client：4 , IP Child proc 80798 normal terminated, return  0
127.0.0.1 , Port 39648
Recv 1025 bytes: c2 . From IP 127.0.0.1 , Port 39644
New client：4 , IP Child proc 80818 normal terminated, return  0
127.0.0.1 , Port 39650
Recv 1025 bytes: c3 . From IP 127.0.0.1 , Port 39646
New client：4 , IP 127.0.0.1 , Port 39652
Child proc 80843 normal terminated, return  0
Recv 1025 bytes: \q . From IP 127.0.0.1 , Port 39648
Child proc 80847 normal terminated, return  0
Accept: Interrupted system call
Recv 1025 bytes: \q . From IP 127.0.0.1 , Port 39650
Child proc 80854 normal terminated, return  0
Accept: Interrupted system call
Recv 1025 bytes: \q . From IP 127.0.0.1 , Port 39652
Child proc 80865 normal terminated, return  0
Accept: Interrupted system call

```

服务端会发生错误 `Accept: Interrupted system call` ，代码中对此错误不是退出而是继续运行，不然服务端就没法继续接受客户端了。  
错误原因有待探究

这是3个客户端同时在线的案例，注意是同时在线！但是排版问题不太好排就列成了竖版。  
<details>
<summary>点击展开案例</summary>

```bash
# client1
$ ./client 
Input: c1
Recv 1025 bytes: c1 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

# client2
$ ./client 
Input: c2
Recv 1025 bytes: c2 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

# client3
$ ./client 
Input: c3
Recv 1025 bytes: c3 . From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

# 查看线程，可以看到服务端程序实现了进程并发
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    80785  0.0  0.0   5992  3472 pts/2    S+   23:16   0:00 ./server
kearney    80797  0.0  0.0   5992  3504 pts/10   S+   23:16   0:00 ./client
kearney    80817  0.0  0.0   5992  3576 pts/11   S+   23:16   0:00 ./client
kearney    80842  0.0  0.0   5992  3500 pts/5    S+   23:16   0:00 ./client
kearney    80847  0.0  0.0   5992   220 pts/2    S+   23:16   0:00 ./server
kearney    80854  0.0  0.0   5992   220 pts/2    S+   23:16   0:00 ./server
kearney    80865  0.0  0.0   5992   220 pts/2    S+   23:16   0:00 ./server
kearney    80892  0.0  0.0  13336  3428 pts/9    R+   23:17   0:00 ps au
```
</details>


## 函数解析
- getpid  
   `__pid_t getpid (void)`  
   定义在 <unistd.h> 中，`__pid_t` 类型相当于 int，作用是获取当前进程的进程号

- getppid  
   `__pid_t getppid (void)`  
   定义在 <unistd.h> 中，作用是获取当前进程的父进程的进程号。

- fork  
    `__pid_t fork (void)`   
   定义在 <unistd.h> 中，作用是克隆一份调用这个函数的进程。失败返回 -1，成功成功时返回子进程 ID，子进程则返回 0

- wait  
   `__pid_t wait (int *__stat_loc)`  
   定义在 <sys/wait.h> 中，作用销毁已经运行完成的子进程，失败时返回-1,成功时返回终止的子进程 ID，然后将子进程状态写入 `__stat_loc`  
   调用 wait 函数的时候，如果没有已终止的子进程，那么程序将阻塞直到有子进程终止为止！谨慎使用！  
   __stat_loc：指向要存储子进程返回状态的变量的指针

- waitpid  
   `__pid_t waitpid (__pid_t __pid, int *__stat_loc, int __options)`  
   定义在 <sys/wait.h> 中，作用是尝试销毁子进程 `__pid`  
   __pid：要销毁的子进程的进程号，__pid 大于零就只尝试销毁对于的子进程，__pid 为 -1 就尝试销毁任意子进程，__pid 为 0 就尝试销毁和当前进程在同一个进程组下的任意子进程，如果是 -1 则尝试销毁进程组号为其绝对值的任意子进程  
   __options：如果传入了常量 WNOHANG，失败时返回-1，成功时返回终止的子进程 ID，子进程还在运行则返回 0。如果传入常量 WUNTRACED，则返回以停止的进程的状态  

- alarm  
   `unsigned int alarm (unsigned int __seconds)`  
   定义在 <unistd.h> 中，作用是在 `__seconds` 秒之后触发 `SIGALRM` 信号

- signal  
   `__sighandler_t signal (int __sig, __sighandler_t __handler)`  
   定义在 <signal.h> 中，作用设置 `__sig` 信号触发时的处理函数为 `__handler`  
   __sig：<bits/signum-generic.h>定义的十五种触发信号之一；SIGALRM 是指已经用完了用过 alarm 函数注册的时间，SIGINT 是输入 ctrl+c， SIGCHLD 是子进程终止或者暂停    
   __handler：当 `__sig` 触发时要调用的函数

- sigaction  
   `int sigaction (int __sig, const struct sigaction *__restrict __act, struct sigaction *__restrict __oact)`  
   定义在 <signal.h> 中，作用是设置当 `__sig` 信号触发时的处理函数，或者获取之前的设置  
   __sig：触发信号  
   __act：触发信号时要调用的动作函数
   __oact：通过此参数获取之前注册的信号处理函数指针，不需要就填 0

# Q&A
1. 什么是进程（process）?

   一个程序可以包含多个进程。程序是进程吗？不是，运行的程序才是进程。系统监视器中可以看到进程表。  
   使用 `ps` 指令可以查看当前运行的所有进程以及进程号（PID）
   ```bash
   $ ps au
   USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
   root         464  4.2  0.7 1653664 123480 tty1   Rsl+ 15:51   1:11 /usr/lib/Xorg -nolisten tcp -background non
   kearney     1780  0.0  0.0  10680  4272 pts/1    Ss+  15:57   0:00 /bin/bash
   kearney     6558  0.0  0.0  10680  4368 pts/2    Ss   16:16   0:00 /bin/bash
   kearney     7587  0.0  0.0  13336  3596 pts/2    R+   16:19   0:00 ps au
   # 查看系统上运行的每一个进程
   $ ps axu
   ```

2. 什么是线程（thread）？

   ？

3. 进程和线程的异同？

   一个进程可以包含多个线程。

- [进程和线程 - 廖雪峰的官方网站](https://www.liaoxuefeng.com/wiki/1016959663602400/1017627212385376)
- [什么是进程？什么是线程？进程和线程之间的区别是什么？](https://www.cnblogs.com/aaronthon/p/9824396.html)

4. 在 [signal.c](signal.c) 中，回调函数 timeout 在注册的时候已经注册为 SIGALRM 的处理函数，为什么在回调函数内部还要再次对激活信号进行判断呢？

   `signal(SIGALRM, timeout)` 确实是注册 timeout 到 SIGALRM 上，当信号被激活时也会调用回调函数 timeout，函数中再一次判断信号是因为这个函数可以绑定到不同的信号上，如果想为不同的信号做不同的处理，则需要判断信号类型，如果只是单一处理的话，就不需要再判断信号类型。