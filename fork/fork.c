#include <stdio.h>
#include <unistd.h>

/* 演示创建子进程。运行结果：
This PID: 22086, parent's pid: 11790, pid -1
Begin to fork
Fork done
This PID: 22086, parent's pid: 11790, pid 22087
This PID: 22086, parent's pid: 11790, pid 22087 : [9, 23] from parent proc
This PID 22086, pid 22087: exit
Fork done
This PID: 22087, parent's pid: 22086, pid 0
This PID: 22087, parent's pid: 22086, pid 0 : [13, 27] from child proc
This PID 22087, pid 0: exit */

int gval = 10;
int main() {
  pid_t pid = -1;
  int lval = 20;
  gval++, lval += 5;
  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  printf("Begin to fork\n");
  pid = fork(); // 克隆进程
  printf("Fork done\n");

  if (pid == 0) { // 子进程
    gval += 2, lval += 2;

  } else { // 父进程
    gval -= 2, lval -= 2;
  }
  printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(), pid);

  if (pid == 0) { // 子进程
    printf(
        "This PID: %d, parent's pid: %d, pid %d : [%d, %d] from child proc\n",
        getpid(), getppid(), pid, gval, lval);

  } else { // 父进程
    printf(
        "This PID: %d, parent's pid: %d, pid %d : [%d, %d] from parent proc\n",
        getpid(), getppid(), pid, gval, lval);
  }
  printf("This PID %d, pid %d: exit\n", getpid(), pid);
  return 0;
}