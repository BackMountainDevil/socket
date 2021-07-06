#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUF_SIZE 100
#define PORT 8080
#define IP "127.0.0.1"

int main() {
  /* socket文件描述符 */
  int sock_fd;

  /* 建立udp socket */
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("Socket failed");
    close(sock_fd);
    exit(EXIT_FAILURE);
  }

  /* 设置address */
  struct sockaddr_in addr_serv;
  memset(&addr_serv, 0, sizeof(addr_serv));
  addr_serv.sin_family = AF_INET;
  addr_serv.sin_addr.s_addr = inet_addr(IP);
  addr_serv.sin_port = htons(PORT);
  int len = sizeof(addr_serv);

  char bufSend[BUF_SIZE];
  char bufRecv[BUF_SIZE];

  while (true) {
    printf("Input a string: "); // 获取用户输入
    std::cin.getline(bufSend, BUF_SIZE);
    if (!strcmp(bufSend, "exit")) { // 输入 ‘exit’ 终止程序
      break;
    }
    int send_num = sendto(sock_fd, bufSend, strlen(bufSend), 0,
                          (struct sockaddr *)&addr_serv, len);

    if (send_num < 0) {
      perror("Sendto error:");
      close(sock_fd);
      exit(EXIT_FAILURE);
    }
    bufSend[send_num] = '\0';
    printf("Client send %d bytes: : %s\n", send_num, bufSend);

    // 等待服务端返回的数据
    int recv_num = recvfrom(sock_fd, bufRecv, sizeof(bufRecv), 0,
                            (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if (recv_num < 0) {
      perror("Recvfrom error:");
      close(sock_fd);
      exit(EXIT_FAILURE);
    }

    bufRecv[recv_num] = '\0';
    printf("Client receive %d bytes: %s\n", recv_num, bufRecv);
    memset(bufSend, 0, BUF_SIZE); // 重置缓冲区
    memset(bufRecv, 0, BUF_SIZE); // 重置缓冲区
  }

  close(sock_fd);
  printf("Connection close successfully\n");
  return 0;
}