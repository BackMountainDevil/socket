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

  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};
  while (true) {
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
      perror("Error: Socket creation failed");
      exit(EXIT_FAILURE);
    }

    /*     // 获取缓冲区大小
        int optval;
        socklen_t tmp = sizeof(optval);
        getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&optval, &tmp);
        printf("Buffer length = %d\n", optval); */

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("Error: Connection creation failed");
      exit(EXIT_FAILURE);
    }
    // 获取用户输入的字符串并发送给服务器
    printf("Input a string: ");
    std::cin.getline(bufSend, BUF_SIZE);
    write(sock, bufSend, sizeof(bufSend));

    //读取服务器传回的数据
    read(sock, bufRecv, sizeof(bufRecv) - 1);
    printf("Message form server: %s\n", bufRecv);

    memset(bufSend, 0, BUF_SIZE); // 重置缓冲区
    memset(bufRecv, 0, BUF_SIZE); // 重置缓冲区
    close(sock);                  // 关闭套接字
  }

  return 0;
}