#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void timeout(int sig) {
  if (sig == SIGALRM) {
    puts("Time out");
  }
  alarm(3);
}

void keyinterrupt(int sig) {
  if (sig == SIGINT) { // 如果是 SIGINT 信号
    puts("Time out");
  }
}

int main() {
  signal(SIGALRM, timeout);     // 注册 SIGALRM 信号到 timeout 函数
  signal(SIGINT, keyinterrupt); // 注册 SIGINT 信号到 keycontrol 函数
  alarm(3);                     // 设置 2 s 后触发信号 SIGALRM
  for (int i = 0; i < 3; i++) {
    puts("wait..");
    sleep(100); // 阻塞 100 秒，实际运行不会那么久的
  }

  return 0;
}