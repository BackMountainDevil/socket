#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 8080

int main() {
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

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
      read(sock, bufRecv, sizeof(bufRecv));
      break;
    }

    //读取服务器传回的数据
    if (read(sock, bufRecv, sizeof(bufRecv)) < 0) {
      perror("Error: Receive fail");
      break;
    }
    printf("Recv: %s\n", bufRecv);
    memset(bufSend, 0, BUF_SIZE);
    memset(bufRecv, 0, BUF_SIZE);
  }
  close(sock); // 关闭套接字
  printf("Log: Socket close\n");
  return 0;
}