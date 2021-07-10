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
  定义在 <pthread.h> 中，作用是让使用此函数的主体阻塞等待线程，成功是返回 0，失败是返回其它值   
  `__th` 终止  
  __th：线程 ID 
  __thread_return：存储线程返回值的变量的地址，NULL 代表无返回值    

## 线程安全
多个线程同时执行临界区代码的时候，可能会引起问题。而线程安全函数被多个线程同时调用也不会引发问题，而非线程安全函数被同时调用时会引发问题。大部分标准函数都是线程安全函数，但有些就不是了，比如 gethostbyname,而提供同一功能的函数 gethostbyname_r 参数又不尽相同，要么改代码，要么声明头文件前定义 _REENTRANT 宏，当然也可以在编译代码的时候加上这个选项 -D_REENTRANT  
```bash
g++ -D_REENTRANT mythread.cpp -o mthread -lpthread
```

## 临界区问题
多个线程同时对一个数据进行修改的时候可能会出错，这个很好理解：有个数 N=100，线程 A 把这个数减一，线程 B 把这个数加一，理想情况下 N 依然是 100，但是由于线程读取和修改 N 的时间不确定，也就是说，A 算出来 99，但是还没有修改 N，此时 B 对 N 操作，N 就变成了101，然后 A 再把结果写回 N，最后 N 变成了 99。

这个问题在计组、数据库都会存在，解决的办法也不少，本质上都是锁机制（互斥量、信号量），说人话就是这个东西我在用（上锁），其它人用不了，除非我用完了（解锁）
