#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025
#define IP "127.0.0.1"
#define PORT 8080

int main() {
  //创建套接字
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
  serv_addr.sin_family = AF_INET;            //使用IPv4地址
  serv_addr.sin_addr.s_addr = inet_addr(IP); //具体的IP地址
  serv_addr.sin_port = htons(PORT);          //端口
  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  printf("Connect to server\n");

  //读取服务器传回的数据
  char bufRecv[BUF_SIZE];
  read(sock, bufRecv, sizeof(bufRecv));
  printf("Message form server: %s\n", bufRecv);

  //关闭套接字
  printf("Client close\n");
  close(sock);
  return 0;
}