#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define IP "127.0.0.1"

int main() {
  /* socket文件描述符 */
  int sock_fd;

  /* 建立udp socket */
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  /* 设置address */
  struct sockaddr_in addr_serv;
  memset(&addr_serv, 0, sizeof(addr_serv));
  addr_serv.sin_family = AF_INET;
  addr_serv.sin_addr.s_addr = inet_addr(IP);
  addr_serv.sin_port = htons(PORT);
  int len = sizeof(addr_serv);

  char send_buf[20] = "hey, who are you?";
  char recv_buf[20];

  printf("Client send: %s\n", send_buf);

  int send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0,
                        (struct sockaddr *)&addr_serv, len);

  if (send_num < 0) {
    perror("Sendto error:");
    exit(EXIT_FAILURE);
  }

  int recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0,
                          (struct sockaddr *)&addr_serv, (socklen_t *)&len);

  if (recv_num < 0) {
    perror("Recvfrom error:");
    exit(EXIT_FAILURE);
  }

  recv_buf[recv_num] = '\0';
  printf("Client receive %d bytes: %s\n", recv_num, recv_buf);

  close(sock_fd);
  return 0;
}