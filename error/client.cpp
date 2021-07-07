#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025
#define IP "127.0.0.1"
#define PORT 8080

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Error: Socket creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error: Connection creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  printf("Connect to server\n");

  //读取服务器传回的数据
  char bufRecv[BUF_SIZE];

  if (read(sock, bufRecv, sizeof(bufRecv)) < 0) {
    perror("Error: Receive fail");
    close(sock);
    exit(EXIT_FAILURE);
  }
  printf("Message form server: %s\n", bufRecv);

  //关闭套接字
  printf("Client close\n");
  close(sock);
  return 0;
}