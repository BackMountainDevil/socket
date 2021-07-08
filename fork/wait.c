#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid = -1;
  int status;
  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  pid = fork(); // 克隆进程

  if (pid == 0) { // 子进程
    printf("This PID: %d, parent's pid: %d, pid %d  from child proc\n",
           getpid(), getppid(), pid);
    sleep(5); // 挂机 5 秒，主要是为了凸显出阻塞
    return 11;
  } else { // 父进程
    printf("This PID: %d, parent's pid: %d, pid %d  from parnet proc\n",
           getpid(), getppid(), pid);

    pid = fork();   // 克隆进程
    if (pid == 0) { // 子进程
      printf("This PID: %d, parent's pid: %d, pid %d  from child proc\n",
             getpid(), getppid(), pid);
      sleep(10); // 挂机 10 秒，主要是为了凸显出阻塞
      return 22;
    } else { // 父进程
      printf("This PID: %d, parent's pid: %d, pid %d  from parnet proc\n",
             getpid(), getppid(), pid);

      printf("waiting\n");
      wait(&status);
      if (WIFEXITED(status)) { // 是正常终止的吗
        printf("This child normal terminated, return  %d\n",
               WEXITSTATUS(status)); // 返回值是多少
      }

      printf("waiting\n");
      wait(&status);
      if (WIFEXITED(status)) {
        printf("This child normal terminated,  return  %d\n",
               WEXITSTATUS(status));
      }
      sleep(5); // 挂机 5 秒
    }
  }

  printf("This PID: %d, parent's pid: %d, pid %d exit\n", getpid(), getppid(),
         pid);
  return 0;
}