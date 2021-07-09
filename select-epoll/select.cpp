#include <cstdio>
#include <iostream>
#include <sys/select.h>
#include <unistd.h>

#define BUF_SIZE 1025

int main() {
  fd_set reads, temps;
  int res, len;
  char buf[BUF_SIZE];
  struct timeval timeout;
  /*   timeout.tv_sec = 5;  // 在此处设置超时时间不一定合理
    timeout.tv_usec = 0; */

  FD_ZERO(&reads); // 将监视集 reads 全部初始化为 0，即啥也不监视
  // 将监视集 reads 中的 第 0 位置为 1
  // 0 是指控制台的标准输入，即监视标准输入的变化
  FD_SET(0, &reads);
  FD_SET(11, &reads); // 对照，11 是啥不重要（我也不知道）
  printf("reads: fd[0] = %d, fd[11] = %d\n", FD_ISSET(0, &reads),
         FD_ISSET(11, &reads));

  for (int i = 0; i < 10; i++) {
    temps = reads; // 为什么要复制？
    printf("\ntemps initial: fd[0] = %d, fd[11] = %d\n", FD_ISSET(0, &temps),
           FD_ISSET(11, &temps)); // 对照实验

    timeout.tv_sec = 3; // 为什么不放在循环外面？放里面每次循环都要重新赋值
    timeout.tv_usec = 0;

    res = select(1, &temps, 0, 0, &timeout);
    if (res == -1) {
      perror("Select Error");
      break;
    } else if (res == 0) { // 超时
      puts("Time out");
    } else {
      if (FD_ISSET(0, &temps)) { // 是不是标准输入触发
        len = read(0, buf, BUF_SIZE);
        buf[len] = 0;
        // 末尾不带 \n 是因为控制台输入的时候有个回车
        printf("res = %d. Got msg from console: %s", res, buf);

        printf("temps at last: fd[0] = %d, fd[11] = %d\n", FD_ISSET(0, &temps),
               FD_ISSET(11, &temps)); // 对照实验
      }
    }
  }

  return 0;
}