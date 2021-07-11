#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 100
#define DOMAIN "localhost" // 域名
#define PORT 8080

/* 多线程 I/O 分离 回声客户端 */

// 获取用户输入的字符串并发送给服务端
void *send_msg(void *arg) {
  int sock = *((int *)arg);
  char bufSend[BUF_SIZE] = {0};
  while (true) {
    // std::cout << "Input: ";  // I/O 分离下这句话输出有毛病
    std::cin.getline(bufSend, BUF_SIZE);
    if (!strcmp(bufSend, "\\q")) { // 输入 ‘\q’ ,逐步终止程序
      shutdown(sock, SHUT_WR);
      printf("Log: Output close\n");
      read(sock, bufSend, sizeof(bufSend));
      break;
    } else if (write(sock, bufSend, sizeof(bufSend)) == -1) { // 发送数据
      perror("Error: Send fail");
      close(sock);
      exit(EXIT_FAILURE);
    }
    memset(bufSend, 0, BUF_SIZE);
  }
  close(sock);
  return NULL;
}

// 读取服务器传回的数据
void *recv_msg(void *arg) {
  char bufRecv[BUF_SIZE] = {0};
  int sock = *((int *)arg);
  while (true) {
    int recv_num = read(sock, bufRecv, sizeof(bufRecv));
    if (recv_num == -1) {
      perror("Error: Receive fail");
      close(sock);
      exit(EXIT_FAILURE);
    }
    std::cout << "Recv " << recv_num << " bytes: " << bufRecv << std::endl;
  }
  return NULL;
}

int main() {
  // 由域名获取 IP
  struct hostent *host = gethostbyname(DOMAIN);
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  if (inet_pton(AF_INET, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]),
                &serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // 输出、输入缓冲区
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  // 创建套接字
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Error: Socket creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error: Connection creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  pthread_t send, recv;
  void *thread_return;

  if (pthread_create(&send, NULL, send_msg, (void *)&sock) != 0) { // 创建线程
    perror("error - pthread_create");
    return -1;
  }
  if (pthread_create(&recv, NULL, recv_msg, (void *)&sock) != 0) {
    perror("error - pthread_create");
    return -1;
  }

  if (pthread_join(send, &thread_return) != 0) { // 等待线程终止
    perror("error - pthread_join");
    return -1;
  }
  printf("Thread return : %s\n", (char *)thread_return);

  if (pthread_join(recv, &thread_return) != 0) {
    perror("error - pthread_join");
    return -1;
  }
  printf("Thread return : %s\n", (char *)thread_return);

  close(sock);
  printf("Client close\n");
  return 0;
}