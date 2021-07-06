#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 8080
// 一对一自由交流客户端
int main() {
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  // 将 IP 地址从文本转换为二进制格式并存储到 sin_addr 中
  if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

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
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  while (true) {
    // 获取用户输入的字符串并发送给服务端
    printf("Input a string: ");
    std::cin.getline(bufSend, BUF_SIZE);
    if (write(sock, bufSend, sizeof(bufSend)) < 0) {
      perror("Error: Send fail\n");
      break;
    }
    if (!strcmp(bufSend, "exit")) { // 输入 ‘exit’ ,逐步终止程序
      shutdown(sock, SHUT_WR);      // 关闭输出流
      printf("Log: Output close\n");
      read(sock, bufRecv, sizeof(bufRecv) - 1);
      break;
    }

    //读取服务器传回的数据
    if (read(sock, bufRecv, sizeof(bufRecv) - 1) < 0) {
      perror("Error: Receive fail");
      break;
    }
    printf("Message form server: %s\n", bufRecv);
    if (!strcmp(bufRecv, "exit")) { // 收到 ‘exit’ ,逐步终止程序
      break;
    }
  }
  close(sock); // 关闭套接字
  printf("Log: Socket close\n");
  return 0;
}