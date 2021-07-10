#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025
#define DOMAIN "localhost" // 域名
#define PORT 8080

void write_routine(int sock, char *buf) {
  while (true) {
    // 获取用户输入的字符串并发送给服务端
    std::cin.getline(buf, BUF_SIZE);
    // 输入 ‘\q’ ,逐步终止程序
    if (!strcmp(buf, "\\q")) {
      shutdown(sock, SHUT_WR);
      return;
    }
    if (write(sock, buf, BUF_SIZE) <= 0) {
      perror("Error: Send fail\n");
      ;
      return;
    }
  }
}

void read_routine(int sock, char *buf) {
  while (true) {
    // 读取服务器传回的数据
    int recv_num = read(sock, buf, BUF_SIZE);
    if (recv_num == 0) {
      return;
    } else {
      buf[recv_num] = '0';
      std::cout << "Recv " << recv_num << " bytes: " << buf << std::endl;
    }
  }
}

int main() {
  // 由域名获取 IP
  struct hostent *host = gethostbyname(DOMAIN);
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  if (inet_pton(AF_INET, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]),
                &serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
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

  // 输出、输入缓冲区
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  int pid = fork();
  if (pid == 0) { // 子进程 负责 接收 数据
    write_routine(sock, bufSend);
  } else { // 父进程 负责 发送 数据
    read_routine(sock, bufRecv);
  }
  close(sock);
  printf("Client close\n");
  return 0;
}