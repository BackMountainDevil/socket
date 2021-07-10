#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcpy
#include <unistd.h> // sleep

void *thread_main(void *arg) {

  int cnt = *((int *)arg);
  char *msg = (char *)malloc(sizeof(char) * 50);
  strcpy(msg, "Hello, I am thread\n");

  for (int i = 0; i < cnt; i++) {
    sleep(1);
    puts("thread running");
  }
  return (void *)msg;
}

int main() {
  pthread_t t_id;
  int thread_param = 5;
  void *thr_ret;

  // 创建线程
  if (pthread_create(&t_id, NULL, thread_main, (void *)&thread_param) != 0) {
    perror("error - pthread_create");
    return -1;
  }

  // 等待线程终止，注释掉这段代码看看会发生什么？
  if (pthread_join(t_id, &thr_ret) != 0) {
    perror("error - pthread_join");
    return -1;
  }

  printf("Thread return : %s\n", (char *)thr_ret);
  free(thr_ret);
  return 0;
}