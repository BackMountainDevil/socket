#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025  // 缓冲区大小 = BUF_SIZE - 1
#define IP "127.0.0.1" // IP 地址
#define PORT 8080      // 端口
#define CLIENTMAX 10   // 最大连接客户数

int main() {
  // 创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1) {
    perror("Error: Socket creation failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充

  int opt = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) == -1) {
    perror("Setsockopt:端口/地址 已经被占用");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;            // 使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr(IP); // 具体的IP地址
  serv_addr.sin_port = htons(PORT);          // 端口

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

  // 阻塞等待接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  if (clnt_sock == -1) {
    perror("Accept");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  printf("Here comes a new client\n");

  // 向客户端发送数据
  char bufSend[BUF_SIZE];
  strcpy(bufSend, "This is server speaking!");
  if (write(clnt_sock, bufSend, sizeof(bufSend)) < 0) {
    perror("Error: Send fail\n");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  printf("Server close\n");
  // 关闭套接字
  close(clnt_sock);
  close(serv_sock);

  return 0;
}
