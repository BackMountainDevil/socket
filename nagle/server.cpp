#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025         // 缓冲区大小 = BUF_SIZE - 1
#define IP "127.0.0.1"        // IP 地址
#define PORT 8080             // 端口
#define CLIENTMAX 10          // 最大连接客户数
#define FILENAME "client.cpp" // 要发送的数据文件

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
  int opt;
  // 获取默认 Nagle 算法状态
  int opts;
  socklen_t tmp = sizeof(opts);
  if (getsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opts, &tmp) ==
      -1) {
    perror("Error getsockopt fail");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Nagle status ：" << opts << std::endl;

  // 关闭 Nagle 算法
  opts = 1;
  if (setsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, &opts, sizeof(opts)) ==
      -1) {
    perror("Error setsockopt fail");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  // 查看 Nagle 算法状态
  if (getsockopt(serv_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opts, &tmp) ==
      -1) {
    perror("Error getsockopt fail");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Nagle status ：" << opts << std::endl;
  opt = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
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
  int clnt_sock = 0;

  clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  if (clnt_sock == -1) {
    perror("Accept");
    close(clnt_sock);
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "New client：" << clnt_sock << " , IP "
            << inet_ntoa(clnt_addr.sin_addr) << " , Port "
            << ntohs(clnt_addr.sin_port) << std::endl;

  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  FILE *fp = fopen(FILENAME, "rb"); // 以二进制方式打开文件
  if (fp == NULL) {                 // 先检查文件是否存在
    perror("Cannot open file");
    exit(EXIT_FAILURE);
  }
  std::cout << "Start to send data '" << FILENAME << "' to client " << clnt_sock
            << std::endl;
  int nCount;
  while ((nCount = fread(bufSend, 1, BUF_SIZE, fp)) > 0) {
    write(clnt_sock, bufSend, nCount);
  }
  fclose(fp); // 关闭文件
  std::cout << "Send data done " << std::endl;
  shutdown(clnt_sock, SHUT_WR); //文件读取完毕，断开输出流，向客户端发送FIN包
  std::cout << "Output stream close " << std::endl;

  // 阻塞，等待客户端接收完毕
  if (read(clnt_sock, bufRecv, sizeof(bufSend)) < 0) {
    perror("Error: Receive fail");
    close(clnt_sock);
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  std::cout << clnt_sock << "：" << bufRecv << std::endl;

  close(clnt_sock);
  close(serv_sock);
  std::cout << "Server close" << std::endl;
  return 0;
}
