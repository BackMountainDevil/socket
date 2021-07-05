#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 8080

int main() {
  //创建套接字
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serv_sock == 0) {
    perror("Socket failed");
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
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;            //使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr(IP); //具体的IP地址
  serv_addr.sin_port = htons(PORT);          //端口
  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  //进入监听状态，等待用户发起请求
  if (listen(serv_sock, 20) < 0) {
    perror("Listen");
    exit(EXIT_FAILURE);
  }

  //接收客户端请求
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUF_SIZE] = {0};

  int clnt_sock =
      accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
  if (clnt_sock < 0) {
    perror("Accept");
    exit(EXIT_FAILURE);
  }

  FILE *fp = fopen("./client.cpp", "rb"); // 以二进制方式打开文件
  if (fp == NULL) {                       // 先检查文件是否存在
    perror("Cannot open file");
    exit(EXIT_FAILURE);
  }
  int nCount;
  while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
    write(clnt_sock, buffer, nCount);
  }
  fclose(fp);                   // 关闭文件
  shutdown(clnt_sock, SHUT_WR); //文件读取完毕，断开输出流，向客户端发送FIN包

  read(clnt_sock, buffer, sizeof(buffer) - 1); // 阻塞，等待客户端接收完毕
  printf("File transfer success!\n");
  //关闭套接字
  close(clnt_sock);
  memset(buffer, 0, BUF_SIZE); //重置缓冲区

  close(serv_sock);

  return 0;
}