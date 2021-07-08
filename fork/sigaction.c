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

int main() {
  struct sigaction act;
  act.sa_handler = timeout; // 注册函数
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  sigaction(SIGALRM, &act, 0); // 注册 SIGALRM 信号到结构体 act
  alarm(3);                    // 设置 3 s 后触发信号 SIGALRM
  for (int i = 0; i < 3; i++) {
    puts("wait..");
    sleep(100); // 阻塞 100 秒，实际运行不会那么久的
  }

  return 0;
}