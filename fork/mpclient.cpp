#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
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
    std::cout << "Input: ";
    std::cin.getline(buf, BUF_SIZE);
    // 输入 ‘\q’ ,逐步终止程序
    if (!strcmp(buf, "\\q")) {
      shutdown(sock, SHUT_WR);
      printf("Log: Output close\n");
      // read(sock, buf, BUF_SIZE);
      // close(sock);
      // printf(" proc %d stop\n", pid);
      // return 0;
      exit(0);
    }
    if (write(sock, buf, BUF_SIZE) < 0) {
      perror("Error: Send fail\n");
      close(sock);
      exit(EXIT_FAILURE);
    } else {
      memset(buf, 0, BUF_SIZE); // 重置缓冲区
    }
  }
}

void send_routine(int sock, char *buf) {
  while (true) {
    // 读取服务器传回的数据
    int recv_num = read(sock, buf, BUF_SIZE);
    if (recv_num < 0) {
      perror("Error: Receive fail");
      close(sock);
      exit(EXIT_FAILURE);
    } else if (recv_num == 0) {
      // return 0;
      exit(0);
    } else {
      std::cout << "Recv " << recv_num << " bytes: " << buf << std::endl;
      memset(buf, 0, BUF_SIZE); // 重置缓冲区
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

  // 输出、输入缓冲区
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

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
  printf("This PID: %d, parent's pid: %d\n", getpid(), getppid());
  int pid = fork();
  if (pid == 0) { // 子进程 负责 接收 数据
                  // write_routine(sock, bufSend);
    printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(),
           pid);
    while (true) {
      // 读取服务器传回的数据
      int recv_num = read(sock, bufRecv, sizeof(bufRecv));
      if (recv_num < 0) {
        perror("Error: Receive fail");
        close(sock);
        exit(EXIT_FAILURE);
      } else {
        std::cout << "Recv " << recv_num << " bytes: " << bufRecv
                  << " . From IP " << inet_ntoa(serv_addr.sin_addr)
                  << " , Port " << ntohs(serv_addr.sin_port) << std::endl;
      }
    }
  } else { // 父进程 负责 发送 数据
           // send_routine(sock, bufRecv);
    printf("This PID: %d, parent's pid: %d, pid %d\n", getpid(), getppid(),
           pid);
    while (true) {
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
        shutdown(sock, SHUT_WR);
        printf("Log: Output close\n");
        read(sock, bufRecv, sizeof(bufRecv));
        break;
      }
    }
  }
  close(sock);
  printf("Client close\n");
  return 0;
}