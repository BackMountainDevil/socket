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

$ ./zombie This PID: 25709, parent's pid: 11790, pid -1
Begin to fork
Fork done
This PID: 25709, parent's pid: 11790, pid 25710  from child proc
Fork done
This PID: 25710, parent's pid: 25709, pid 0  from child proc
This PID: 25710, parent's pid: 25709, pid 0
This PID: 25710, parent's pid: 25709, pid 0  from child proc
This PID 25710, pid 0: exit
# 这里会有 10 s 的挂机等待，在这个时候开启另一个终端查看用户进程
This PID: 25709, parent's pid: 11790, pid 25710
This PID: 25709, parent's pid: 11790, pid 25710  from child proc
This PID 25709, pid 25710: exit
```

可以从下面看到 COMMAND 中与 PID 25709 对应的正是我们的程序 `./zombie`，而 PID 25709 对应的子进程早已运行完成，但它就是没有被销毁，因此变成了僵尸进程，在 COMMAND 被标记为 `[zombie] <defunct>`， STAT 变为 `Z+`。当程序的父子进程都运行结束的时候，这个僵尸进程被父进程销毁，是的，被父进程销毁了，而不是在子进程运行结束的时候被操作系统销毁。

```bash
# 查看用户进程，运行程序中，程序处于那个挂机状态
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    25709  0.0  0.0   2364   672 pts/2    S+   17:16   0:00 ./zombie
kearney    25710  0.0  0.0      0     0 pts/2    Z+   17:16   0:00 [zombie] <defunct>
kearney    25735  0.0  0.0  13336  3596 pts/3    R+   17:16   0:00 ps au

# 查看用户进程，程序已经运行结束
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney    25873  0.0  0.0  13336  3528 pts/3    R+   17:17   0:00 ps au
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
$ ./wait This PID: 2442, parent's pid: 1947, pid -1
This PID: 2442, parent's pid: 1947, pid 2443  from parnet proc
This PID: 2443, parent's pid: 2442, pid 0  from child proc
This PID: 2442, parent's pid: 1947, pid 2444  from parnet proc
This child normal terminated,  return  11
This PID: 2444, parent's pid: 2442, pid 0  from child proc
This child normal terminated,  return  22
# 这里程序挂机了 10 s 
This PID: 2442, parent's pid: 1947, pid 2444 exit

# 另一个终端会话中，此时程序正在挂机，但是可以看到下面没有出现僵尸进程。
$ ps au
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
kearney     2442  0.0  0.0   2364   676 pts/1    S+   18:37   0:00 ./wait
kearney     2448  0.0  0.0  13336  3572 pts/3    R+   18:37   0:00 ps au
```

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