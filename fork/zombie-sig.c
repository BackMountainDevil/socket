#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void killzombie(int sig) {
  if (sig == SIGCHLD) {
    int status;
    int pid = waitpid(-1, &status, WNOHANG); // 销毁子进程
    if (WIFEXITED(status)) {                 // 如果是正常终止的话
      printf("Child proc %d normal terminated, return  %d\n", pid,
             WEXITSTATUS(status));
    }
  }
}

int main() {
  struct sigaction act;
  act.sa_handler = killzombie; // 注册函数
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  // 注册 SIGCHLD 信号到结构体 act，注释掉下面一行将会产生僵尸进程
  sigaction(SIGCHLD, &act, 0);

  pid_t pid = -1;
  int status;
  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  pid = fork();   // 克隆进程
  if (pid == 0) { // 子进程
    printf("This PID: %d, parent's pid: %d, pid %d  from child proc\n",
           getpid(), getppid(), pid);
    sleep(4); // 挂机 4 秒
    return 11;
  } else { // 父进程
    printf("This PID: %d, parent's pid: %d, pid %d  from parnet proc\n",
           getpid(), getppid(), pid);
    for (int i = 0; i < 4; i++) {
      puts("wait..");
      sleep(2); // 阻塞 2 秒
    }
  }

  printf("This PID: %d, parent's pid: %d, pid %d exit\n", getpid(), getppid(),
         pid);
  return 0;
}