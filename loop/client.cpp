#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025
#define IP "127.0.0.1"
#define PORT 8080

int main() {

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // 输出、输入缓冲区
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  while (true) {
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

    // 获取用户输入的字符串并发送给服务端
    std::cout << "Input: ";
    std::cin.getline(bufSend, BUF_SIZE);
    if (write(sock, bufSend, sizeof(bufSend)) < 0) {
      perror("Error: Send fail\n");
      close(sock);
      exit(EXIT_FAILURE);
    }

    // 输入 ‘\q’ ,逐步终止程序
    if (!strcmp(bufSend, "\\q")) {
      close(sock);
      printf("Log: Output close\n");
      break;
    }

    // 读取服务器传回的数据
    int recv_num = read(sock, bufRecv, sizeof(bufRecv));
    if (recv_num == -1) {
      perror("Error: Receive fail");
      close(sock);
      exit(EXIT_FAILURE);
    }
    std::cout << "Recv " << recv_num << " bytes: " << bufRecv << " from IP "
              << inet_ntoa(serv_addr.sin_addr) << " , Port "
              << ntohs(serv_addr.sin_port) << std::endl;
    close(sock);
    // 重置缓冲区
    memset(bufSend, 0, BUF_SIZE);
    memset(bufRecv, 0, BUF_SIZE);
  }

  printf("Client close\n");
  return 0;
}