#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025 // 缓冲区大小 = BUF_SIZE - 1
#define IP INADDR_ANY // 自动获取IP地址
#define PORT 8080
#define CLIENTMAX 10 // 最大连接客户数

int main() {
  // 创建套接字，第二个参数 SOCK_DGRAM 指定为 UDP
  int serv_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (serv_sock == -1) {
    perror("Error: Socket creation failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充

  int opt = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Setsockopt:端口/地址 已经被占用");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;        // 使用IPv4地址
  serv_addr.sin_addr.s_addr = htonl(IP); // IP地址
  serv_addr.sin_port = htons(PORT);      // 端口

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("Bind failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  // 阻塞等待接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);

  char bufSend[BUF_SIZE];
  while (true) {
    // 读取客户端发来的信息，然后返回数据
    int recv_num =
        recvfrom(serv_sock, bufSend, sizeof(bufSend), 0,
                 (struct sockaddr *)&clnt_addr, (socklen_t *)&clnt_addr_size);

    if (recv_num < 0) {
      perror("Recvfrom error:");
      close(serv_sock);
      exit(1);
    }

    std::cout << "Recv " << recv_num << " bytes: " << bufSend << " from IP "
              << inet_ntoa(clnt_addr.sin_addr) << " , Port "
              << ntohs(clnt_addr.sin_port) << std::endl;

    int send_num = sendto(serv_sock, bufSend, recv_num, 0,
                          (struct sockaddr *)&clnt_addr, clnt_addr_size);
    if (send_num < 0) {
      perror("Sendto error:");
      close(serv_sock);
      exit(1);
    }
    memset(bufSend, 0, BUF_SIZE); //重置缓冲区
  }

  close(serv_sock);
  std::cout << "Server close" << std::endl;
  return 0;
}
