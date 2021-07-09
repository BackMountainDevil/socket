#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1025
#define IP "127.0.0.1"
#define PORT 8080
#define FILENAME "log.txt" // 要存放接收的数据的文件名
int main() {

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充
  serv_addr.sin_family = AF_INET;           //使用IPv4地址
  serv_addr.sin_port = htons(PORT);         //端口

  if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // 输出、输入缓冲区
  char bufSend[BUF_SIZE] = {0};
  char bufRecv[BUF_SIZE] = {0};

  FILE *fp = fopen(FILENAME, "wb"); // 以二进制方式打开（创建）文件
  if (fp == NULL) {
    perror("Error：Cannot open file");
    exit(EXIT_FAILURE);
  }

  // 创建套接字
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Error: Socket creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  // 获取默认缓冲区大小
  int opts, optr;
  socklen_t tmp = sizeof(opts);
  if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&opts, &tmp) == -1 ||
      getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&optr, &tmp) == -1) {
    perror("Error getsockopt fail");
    close(sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Send buffer length ：" << opts << "\nRecv buffer length ："
            << optr << std::endl;
  // 设置缓冲区大小
  opts = BUF_SIZE;
  optr = BUF_SIZE;
  if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &opts, sizeof(opts)) == -1 ||
      setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &optr, sizeof(optr)) == -1) {
    perror("Error setsockopt fail");
    close(sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Buffer length both set to " << BUF_SIZE << std::endl;

  // 查看缓冲区大小，看看设置生效没
  if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&opts, &tmp) == -1 ||
      getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&optr, &tmp) == -1) {
    perror("Error getsockopt fail");
    close(sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Send buffer length ：" << opts << "\nRecv buffer length ："
            << optr << std::endl;

  std::cout << "Trying to connect server" << std::endl;
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("Error: Connection creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Connected to server" << std::endl;

  //循环接收数据，直到文件传输完毕
  std::cout << "Recving data" << std::endl;
  int nCount;
  while ((nCount = read(sock, bufRecv, sizeof(bufRecv))) > 0) {
    fwrite(bufRecv, nCount, 1, fp);
  }
  fclose(fp); // 文件接收完毕后直接关闭套接字，无需调用shutdown()
  std::cout << "Recv done, data in " << FILENAME << std::endl;

  sleep(3); // 模拟延迟收到，可以看到服务端是真的在等待客户端的回应
  strcpy(bufSend, "I have recv the data,Thank you");
  if (write(sock, bufSend, sizeof(bufSend)) == -1) {
    perror("Error: Send fail\n");
    close(sock);
    exit(EXIT_FAILURE);
  }
  std::cout << "Send to server: " << bufSend << std::endl;
  close(sock);

  printf("Client close\n");
  return 0;
}