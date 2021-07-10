#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // strcpy
#include <unistd.h>   // sleep
#define THREAD_NUM 20 // 线程数，这里要为偶数
long long num = 0;    //全局变量，演示用途
// pthread_mutex_t mutex; //互斥量

/* 演示解决临界区问题的办法之一：互斥量 */

void *increase(void *arg) {
  // 使 num 加 5000
  // pthread_mutex_lock(&mutex); // 加锁
  for (int i = 0; i < 5000; i++) {
    num += 1;
  }
  // pthread_mutex_unlock(&mutex); // 解锁
  return NULL;
}

void *decrease(void *arg) {
  // 使 num 减 5000
  // pthread_mutex_lock(&mutex); // 加锁
  for (int i = 0; i < 5000; i++) {
    num -= 1;
  }
  // pthread_mutex_unlock(&mutex); // 解锁
  return NULL;
}

int main() {
  pthread_t t_id[THREAD_NUM];
  printf("The num : %lld\n", num);

  // 建立互斥量
  // pthread_mutex_init(&mutex, NULL);

  // 创建线程, 一半加、一半减，按理说最后结果中 num 的值不会发生变化
  for (int i = 0; i < THREAD_NUM; i++) {
    if (i % 2) {
      if (pthread_create(&t_id[i], NULL, increase, NULL) != 0) {
        perror("error - pthread_create");
        return -1;
      }
    } else {
      if (pthread_create(&t_id[i], NULL, decrease, NULL) != 0) {
        perror("error - pthread_create");
        return -1;
      }
    }
  }

  // 等待线程终止
  for (int i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(t_id[i], NULL) != 0) {
      perror("error - pthread_join");
      return -1;
    }
  }

  printf("The num : %lld\n", num);

  // 释放互斥量
  // pthread_mutex_destroy(&mutex);
  return 0;
}