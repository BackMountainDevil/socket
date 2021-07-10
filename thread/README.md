# 多线程服务端

## 线程的创建和运行
- [1thread.cpp](1thread.cpp):演示线程的创建和被等待

创建线程之后，线程和主进程是并行状态，为了确保线程在进程结束前能正常终止，需要在结束主进程的时候等待线程正常终止


Tip：编译有关线程的代码需要添加 `-lpthread`参数来引入线程库  
```bash
g++ mythread.cpp -o mthread -lpthread
```

- pthread_create  
  ```c
  int pthread_create (pthread_t *__restrict __newthread,
			   const pthread_attr_t *__restrict __attr,
			   void *(*__start_routine) (void *),
			   void *__restrict __arg);
  ```
  定义在 <pthread.h> 中，作用是创建一个新线程  
  __newthread：保存新线程 ID 的变量的地址  
  __attr：传递线程属性的参数， NULL 代表默认属性  
  __start_routine：线程要执行的函数的地址  
  __arg：传递参数信息的变量的地址  

- pthread_join  
  `int pthread_join (pthread_t __th, void **__thread_return)`  
  定义在 <pthread.h> 中，作用是让使用此函数的主体阻塞等待线程，成功时返回 0，失败时返回其它值 
  `__th` 终止  
  __th：线程 ID 
  __thread_return：存储线程返回值的变量的地址，NULL 代表无返回值    


- pthread_detach  
  `int pthread_detach (pthread_t __th)`  
  定义在 <pthread.h> 中，作用是引导线程 `__th` 结束的时候立即销毁，成功时返回 0，失败时返回其它值   
  使用这个函数销毁进程不会引起阻塞，需要注意的时候，对线程 A 用了这个方法就不能再对 A 线程用 pthread_join 方法  

## 线程安全
多个线程同时执行临界区代码的时候，可能会引起问题。而线程安全函数被多个线程同时调用也不会引发问题，而非线程安全函数被同时调用时会引发问题。大部分标准函数都是线程安全函数，但有些就不是了，比如 gethostbyname,而提供同一功能的函数 gethostbyname_r 参数又不尽相同，要么改代码，要么声明头文件前定义 _REENTRANT 宏，当然也可以在编译代码的时候加上这个选项 -D_REENTRANT  
```bash
g++ -D_REENTRANT mythread.cpp -o mthread -lpthread
```

## 临界区问题
多个线程同时对一个数据进行修改的时候可能会出错，这个很好理解：有个数 N=100，线程 A 把这个数减一，线程 B 把这个数加一，理想情况下 N 依然是 100，但是由于线程读取和修改 N 的时间不确定，也就是说，A 算出来 99，但是还没有修改 N，此时 B 对 N 操作，N 就变成了101，然后 A 再把结果写回 N，最后 N 变成了 99。

这个问题在计组、数据库都会存在，解决的办法也不少，本质上都是锁机制（互斥量、信号量），说人话就是这个东西我在用（上锁），其它人用不了，除非我用完了（解锁）

### 互斥量
- [2thread-mutex](2thread-mutex.cpp): 临界区问题演示与互斥量解决办法  

<details>
<summary>点击查看 函数解释 </summary>

- pthread_mutex_init  
  `int pthread_mutex_init (pthread_mutex_t *__mutex,			       const pthread_mutexattr_t *__mutexattr)`  
  定义在 <pthread.h> 中，作用是建立互斥量保存到 __mutex 中。成功时返回 0  
  __mutex：保存互斥量的变量的地址，为此需要先建立一个互斥量 `pthread_mutex_t mutex；`  
  __mutexattr：互斥量属性，没有特别需要就填 NULL

- pthread_mutex_destroy  
  `int pthread_mutex_destroy (pthread_mutex_t *__mutex)`  
  定义在 <pthread.h> 中，作用是销毁 __mutex 中的互斥量。成功时返回 0  

- pthread_mutex_lock  
  `int pthread_mutex_lock (pthread_mutex_t *__mutex)`  
  定义在 <pthread.h> 中，作用给互斥量加锁，如果已经被加锁，则会等待对方解锁。成功时返回 0  
  加锁用完之后一定要记得解锁，不然当别的线程再次加锁就会发生死锁现象，大家都在阻塞等待解锁  

- pthread_mutex_unlock  
  `int pthread_mutex_unlock (pthread_mutex_t *__mutex)`  
  定义在 <pthread.h> 中，作用给互斥量解锁。成功时返回 0    
</details>

<details>
<summary>点击查看 运行案例 </summary>

加 5000 是为了使结果明显，改大更明显。代码中的临界区是 `num += 1;` 和 `num -= 1;`，代码中不是在临界区两边加锁、解锁 而是在 for 循环外面这么做，是因为可以减少加锁、解锁的次数。

```bash
# 编译
$ make

# 对临界资源不加以控制的情况，结果不稳定、很大可能不正确
$ ./2thread-mutex 
The num : 0
The num : 1921

$ ./2thread-mutex 
The num : 0
The num : -5162

# 取消代码中的代码注释（第 8、14、18、24、28、37、65 行），再次编译，结果正确
$ make

$ ./2thread-mutex 
The num : 0
The num : 0
```
</details>

## 回声服务程序
- [server-thread.cpp](server-thread.cpp)：多线程回声服务端  
- [client.cpp](client.cpp):普通回声客户端  

在之前的程序中，为了支持同时服务多个客户，用了各种办法保持套接字，集中管理，如 select 中的 fd_set。这里用 clnt_socks[CLIENTMAX] 保存客户端的全部套接字，用 clnt_cnt 保持在线客户数，这两个就是临界资源，在对临界资源进行操作的时候需要加锁、解锁。

<details>
<summary>点击查看 多线程回声服务端与普通客户端运行案例 </summary>

演示结果表明，支持多个客户端同时在线，互不影响。但是很奇怪的是不是每次客户端掉线都会提示说客户掉线了  
```bash
$ make cs

$ ./server-thread 
Waiting for connecting
New client：4 , IP 127.0.0.1 , Port 35200
New client：5 , IP 127.0.0.1 , Port 35202
New client：6 , IP 127.0.0.1 , Port 35204
4 : c4
5 : c5
6 : c6
6 : c66
5 : c55
4 : c44
Client 4 disconnect

# 下面是三个并行的客户端
$ ./client 
Input: c4
Recv 1025 bytes: c4. From IP 127.0.0.1 , Port 8080
Input: c44
Recv 1025 bytes: c44. From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

$ ./client 
Input: c5
Recv 1025 bytes: c5. From IP 127.0.0.1 , Port 8080
Input: c55
Recv 1025 bytes: c55. From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close

$ ./client 
Input: c6
Recv 1025 bytes: c6. From IP 127.0.0.1 , Port 8080
Input: c66
Recv 1025 bytes: c66. From IP 127.0.0.1 , Port 8080
Input: \q
Log: Output close
Client close
```
</details>