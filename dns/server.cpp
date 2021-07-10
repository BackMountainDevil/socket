#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025      // 缓冲区大小 = BUF_SIZE - 1
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口
#define CLIENTMAX 10       // 最大连接客户数

int main() {
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

  // 阻塞等待接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock = 0;
  while (true) {
    clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
      perror("Accept");
      break;
    }
    std::cout << "New client：" << clnt_sock << " , IP "
              << inet_ntoa(clnt_addr.sin_addr) << " , Port "
              << ntohs(clnt_addr.sin_port) << std::endl;

    // 读取客户端发来的信息，然后返回数据
    char bufSend[BUF_SIZE];
    if (read(clnt_sock, bufSend, sizeof(bufSend)) < 0) {
      perror("Error: Receive fail");
      break;
    }
    std::cout << clnt_sock << "：" << bufSend << std::endl;
    if (write(clnt_sock, bufSend, sizeof(bufSend)) < 0) {
      perror("Error: Send fail");
      break;
    }

    close(clnt_sock);
  }

  close(clnt_sock);
  close(serv_sock);
  std::cout << "Server close" << std::endl;
  return 0;
}
