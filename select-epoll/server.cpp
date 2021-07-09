#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>

#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1025      // 缓冲区大小 = BUF_SIZE - 1
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口
#define CLIENTMAX 10       // 最大连接客户数

int main() {
  fd_set readfds;

  // 由域名获取 IP
  struct hostent *host = gethostbyname(DOMAIN);
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  // 创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
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
    perror("Setsockopt: 设置 地址/端口 可复用失败");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET; // 使用IPv4地址
  serv_addr.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)host->h_addr_list[0])); // IP地址
  serv_addr.sin_port = htons(PORT);                                  // 端口

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("Bind failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  // 进入监听状态，等待用户发起请求
  if (listen(serv_sock, CLIENTMAX) == -1) {
    perror("Listen");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  printf("Waiting for connecting\n");

  fd_set reads, reads_cp;
  FD_ZERO(&reads);
  FD_SET(serv_sock, &reads); // 监听主套接字
  int fd_max = serv_sock;
  struct timeval timeout;

  int fd_num;
  // 阻塞等待接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);

  while (true) {
    reads_cp = reads;
    timeout.tv_sec = 5;  // s
    timeout.tv_usec = 0; // us

    if ((fd_num = select(fd_max + 1, &reads_cp, 0, 0, &timeout)) == -1) {
      perror("Select Error");
      break;
    }

    for (int i = 0; i < fd_max + 1; i++) {
      if (FD_ISSET(i, &reads_cp)) { // 逐个查找是哪个家伙被激活了
        if (i == serv_sock) {       // 如果是主套接字
          int clnt_sock =
              accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
          if (clnt_sock == -1) {
            perror("Accept Error");
            continue;
          } else {
            std::cout << "New client：" << clnt_sock << " , IP "
                      << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                      << ntohs(clnt_addr.sin_port) << std::endl;
            FD_SET(clnt_sock, &reads); // 将客户套接字加入监听名单
            if (clnt_sock > fd_max) {
              fd_max = clnt_sock;
            }
          }
        } else { // 不然就是客户套接字。读取客户端发来的信息，然后返回数据
          char bufSend[BUF_SIZE];
          int recv_num = read(i, bufSend, sizeof(bufSend));
          if (recv_num < 0) {
            perror("Error: Receive fail");
            close(i);
            exit(EXIT_FAILURE);
          } else if (recv_num == 0) { // EOF
            FD_CLR(i, &reads);        // 从监控名单中剔除
            std::cout << "Client " << i << " disconnect. IP "
                      << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                      << ntohs(clnt_addr.sin_port) << std::endl;
            close(i);
          } else {
            std::cout << "Recv " << recv_num << " bytes: " << bufSend
                      << " . From IP " << inet_ntoa(clnt_addr.sin_addr)
                      << " , Port " << ntohs(clnt_addr.sin_port) << std::endl;

            if (write(i, bufSend, sizeof(bufSend)) < 0) {
              perror("Error: Send fail");
              close(i);
              exit(EXIT_FAILURE);
            }
          }
        }
      }
    }
  }
  close(serv_sock);
  std::cout << "Server close" << std::endl;
  return 0;
}
