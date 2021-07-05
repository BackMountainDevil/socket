#include <errno.h>
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

  char bufSend[BUF_SIZE] = "I am server!";
  char bufRecv[BUF_SIZE];
  struct sockaddr_in addr_client;

  while (true) {
    printf("Server wait:\n");

    int recv_num = recvfrom(sock_fd, bufRecv, sizeof(bufRecv), 0,
                            (struct sockaddr *)&addr_client, (socklen_t *)&len);

    if (recv_num < 0) {
      perror("Recvfrom error:");
      exit(1);
    }

    bufRecv[recv_num] = '\0';
    printf("Server receive %d bytes: %s\n", recv_num, bufRecv);

    int send_num = sendto(sock_fd, bufSend, recv_num, 0,
                          (struct sockaddr *)&addr_client, len);

    if (send_num < 0) {
      perror("Sendto error:");
      exit(1);
    }
  }

  close(sock_fd);

  return 0;
}