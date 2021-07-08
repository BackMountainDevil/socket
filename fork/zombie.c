#include <stdio.h>
#include <unistd.h>

int main() {
  pid_t pid = -1;

  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  printf("Begin to fork\n");
  pid = fork(); // 克隆进程
  printf("Fork done\n");

  if (pid == 0) { // 子进程
    printf("This PID: %d, parent's pid: %d, pid %d  from child proc\n",
           getpid(), getppid(), pid);
  } else { // 父进程
    printf("This PID: %d, parent's pid: %d, pid %d  from parent proc\n",
           getpid(), getppid(), pid);
    sleep(10); // 挂机 10 秒
  }
  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  printf("This PID %d, pid %d: exit\n", getpid(), pid);
  return 0;
}