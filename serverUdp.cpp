#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUF_SIZE 100
#define IP INADDR_ANY
#define PORT 8080
/* INADDR_ANY表示不管是哪个网卡接收到数据，只要目的端口是PORT，就会被该应用程序接收到
 */

int main() {
  /* sock_fd --- socket文件描述符 创建udp套接字*/
  // . SOCK_DGRAM 表示使用 UDP 协议
  int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  /* 将套接字和IP、端口绑定 */
  struct sockaddr_in addr_serv;
  memset(&addr_serv, 0, sizeof(struct sockaddr_in)); // 每个字节都用0填充
  addr_serv.sin_family = AF_INET;                    // 使用IPV4地址
  addr_serv.sin_port = htons(PORT);                  // 端口
  addr_serv.sin_addr.s_addr = htonl(IP);             // 自动获取IP地址
  int len = sizeof(addr_serv);

  /* 绑定socket */
  if (bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  char buffer[BUF_SIZE];
  struct sockaddr_in addr_client;

  while (true) {
    int recv_num = recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&addr_client, (socklen_t *)&len);

    if (recv_num < 0) {
      perror("Recvfrom error:");
      exit(1);
    }

    buffer[recv_num] = '\0';
    printf("Server receive %d bytes: %s\n", recv_num, buffer);

    int send_num = sendto(sock_fd, buffer, recv_num, 0,
                          (struct sockaddr *)&addr_client, len);
    if (send_num < 0) {
      perror("Sendto error:");
      exit(1);
    }
    memset(buffer, 0, BUF_SIZE); //重置缓冲区
  }

  close(sock_fd);

  return 0;
}