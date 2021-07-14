#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025      // 缓冲区大小 = BUF_SIZE - 1
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口
#define CLIENTMAX 100      // 最大连接客户数

pthread_mutex_t mutex;     // 互斥量
int clnt_cnt = 0;          // 客户数，临界资源
int clnt_socks[CLIENTMAX]; // 保存客户套接字的数组，临界资源

/* 多进程群聊服务端 */

// 把消息发送给所有客户
void *send_msg_all(char *msg, int len) {
  int num;
  pthread_mutex_lock(&mutex); // 加锁
  for (int j = 0; j < clnt_cnt; j++) {
    num = write(clnt_socks[j], msg, len);
    if (num < 0) {
      perror("Error: Send fail");
      return NULL;
    }
  }
  pthread_mutex_unlock(&mutex); // 解锁
  return NULL;
}

// 处理新客户连接
void *handle_client(void *arg) {
  int clnt_sock = *((int *)arg);
  char bufSend[BUF_SIZE];
  int recv_num = 0;
  while ((recv_num = read(clnt_sock, bufSend, sizeof(bufSend))) != 0) {
    std::cout << clnt_sock << " : " << bufSend << std::endl;
    send_msg_all(bufSend, recv_num);
  }

  pthread_mutex_lock(&mutex);          // 加锁
  for (int i = 0; i < clnt_cnt; i++) { // 剔除掉线用户
    if (clnt_socks[i] == clnt_sock) {
      clnt_cnt--; // 客户数量减一
      while (i < clnt_cnt) {
        clnt_socks[i] = clnt_socks[i + 1];
        i++;
      }
      std::cout << "Client " << clnt_sock << " disconnect" << std::endl;
      break;
    }
  }
  pthread_mutex_unlock(&mutex); // 解锁
  close(clnt_sock);
  return NULL;
}

int main() {
  struct hostent *host = gethostbyname(DOMAIN); // 由域名获取 IP
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  int serv_sock = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
  if (serv_sock == -1) {
    perror("Error: Socket creation failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Setsockopt: 设置 地址/端口 可复用失败");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充
  serv_addr.sin_family = AF_INET;           // 使用IPv4地址
  serv_addr.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)host->h_addr_list[0])); // IP地址
  serv_addr.sin_port = htons(PORT);                                  // 端口

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("Bind failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  // 进入监听状态，等待用户发起请求
  if (listen(serv_sock, CLIENTMAX) == -1) {
    perror("Listen");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  printf("Waiting for connecting\n");

  pthread_mutex_init(&mutex, NULL); // 建立互斥量
  pthread_t t_id;

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  while (true) {
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
      perror("Accept Error");
      continue;
    } else {
      pthread_mutex_lock(&mutex); // 加锁
      clnt_socks[clnt_cnt++] = clnt_sock;
      pthread_mutex_unlock(&mutex); // 解锁

      // 创建线程
      if (pthread_create(&t_id, NULL, handle_client, (void *)&clnt_sock) != 0) {
        perror("error - pthread_create");
        return -1;
      }
      pthread_detach(t_id); // 销毁进程

      std::cout << "New client：" << clnt_sock << " , IP "
                << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                << ntohs(clnt_addr.sin_port) << std::endl;
    }
  }

  close(serv_sock);
  puts("Server close");
  return 0;
}
