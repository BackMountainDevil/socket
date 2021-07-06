#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 8080
// 一对一自由交流服务端
int main() {
  //创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serv_sock == 0) {
    perror("Socket failed");
    close(serv_sock);
    exit(EXIT_FAILURE); // 相当于 exit(1)
  }

  //将套接字和IP、端口绑定
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  int opt = 1;
  // Forcefully attaching socket to the port 8080
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Setsockopt:端口/地址 已经被占用");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;            //使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr(IP); //具体的IP地址
  serv_addr.sin_port = htons(PORT);          //端口
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Bind failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  //进入监听状态，等待用户发起请求
  if (listen(serv_sock, 20) < 0) {
    perror("Listen");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  //接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  if (clnt_sock < 0) {
    perror("Accept");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  printf("Server: New connection from %s\n",
         inet_ntoa(clnt_addr.sin_addr)); // 获取客户端的 IP
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  while (true) {
    if (read(clnt_sock, bufRecv, sizeof(bufRecv) - 1) <
        0) { // 接收客户端发来的数据
      perror("Error: Receive fail");
      break;
    }
    printf("Message form client: %s\n", bufRecv);
    if (!strcmp(bufRecv, "exit")) { // 收到 ‘exit’ ,逐步终止程序
      break;
    }

    // 获取用户输入的字符串并发送给客户端
    printf("Input a string: ");
    std::cin.getline(bufSend, BUF_SIZE);
    if (write(clnt_sock, bufSend, sizeof(bufSend)) < 0) {
      perror("Error: Send fail\n");
      break;
    }
    if (!strcmp(bufSend, "exit")) { // 输入 ‘exit’ ,逐步终止程序
      shutdown(clnt_sock, SHUT_WR); // 关闭输出流
      printf("Log: Output close\n");
      read(clnt_sock, bufRecv, sizeof(bufRecv) - 1); // 阻塞，等待客户端接收完毕
      break;
    }
  }

  close(clnt_sock);
  printf("Log: Client Socket close\n");
  close(serv_sock);
  printf("Log: Server Socket close\n");
  return 0;
}
