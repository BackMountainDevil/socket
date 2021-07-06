#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 100
#define PORT 8080
#define MAXCLIENT 10 // 最大连接客户数

int main(int argc, char *argv[]) {
  int opt = true;
  int master_socket, addrlen, new_socket, client_socket[MAXCLIENT], activity,
      sd, max_sd;
  struct sockaddr_in address;
  char buffer[BUF_SIZE];

  // socket 描述文件的集合
  fd_set readfds;

  // 将所有客户的 socket 描述文件初始化为 0.  so not checked
  for (int i = 0; i < MAXCLIENT; i++) {
    client_socket[i] = 0;
  }

  // 创建 主 socket
  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    close(master_socket);
    exit(EXIT_FAILURE);
  }

  // 设置允许 主socket 建立多个连接； 这只是一个好习惯，不设置这个也能正常运行
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                 sizeof(opt)) < 0) {
    perror("setsockopt");
    close(master_socket);
    exit(EXIT_FAILURE);
  }

  // type of socket created
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    close(master_socket);
    exit(EXIT_FAILURE);
  }

  // 尝试为主套接字指定最多 3 个挂起的连接
  if (listen(master_socket, 3) < 0) {
    perror("listen");
    close(master_socket);
    exit(EXIT_FAILURE);
  }

  // 接收即将到来的连接
  addrlen = sizeof(address);
  puts("Waiting for connections ...");

  while (true) {
    FD_ZERO(&readfds);               // 清空集合
    FD_SET(master_socket, &readfds); // 将 主socket 添加到集合中
    max_sd = master_socket;
    // 将 子socket 添加到集合中
    for (int i = 0; i < MAXCLIENT; i++) {
      // socket descriptor
      sd = client_socket[i];

      // 如果套接字描述符是有效的，就添加到读取列表中
      if (sd > 0)
        FD_SET(sd, &readfds);

      // 选择最大的描述符， 选择功能的时候需要它
      if (sd > max_sd)
        max_sd = sd;
    }
    // 等待其中某个 socket 被激活。超时时间设置为 NULL，所以会无限等待
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
    if ((activity < 0) && (errno != EINTR)) {
      printf("select error");
    }

    // 如果 主socket发生了啥，说明有新连接来了
    if (FD_ISSET(master_socket, &readfds)) {
      if ((new_socket = accept(master_socket, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        close(new_socket);
        close(master_socket);
        exit(EXIT_FAILURE);
      }

      // 显示新客户的 socket number （用来发送和接受命令）
      printf("New connection , socket fd is %d , ip is : %s , port : %dn ",
             new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      // 给新客户发送问候消息
      std::string msg =
          "Hello. Your socket number: " + std::to_string(new_socket);
      int k; // string 转 char[]
      for (k = 0; k < msg.length(); k++)
        buffer[k] = msg[k];
      buffer[k] = '\0';
      if (write(new_socket, buffer, sizeof(buffer)) < 0) {
        perror("Error: Send fail\n");
        break;
      }

      // 将新连接添加到 sockets 数组中
      for (int i = 0; i < MAXCLIENT; i++) {
        if (client_socket[i] == 0) { // 找到第一个空位
          client_socket[i] = new_socket;
          printf("Adding to list of sockets as %d\n", i);
          break;
        }
      }
    }

    // 对子 socket 进行检查
    for (int i = 0; i < MAXCLIENT; i++) {
      sd = client_socket[i];
      if (FD_ISSET(sd, &readfds)) {
        int valread = read(sd, buffer, sizeof(buffer));
        // std::cout << sd << " activated, len = " << valread << std::endl;
        if (valread <= 0) { // 掉线用户
          // 客户掉线：输出其相关信息
          getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
          printf("Host disconnected, socket fd %d , ip %s , port %d \n", sd,
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));
          // 关闭掉线客户的 socket,并标记为 0,以便重新使用
          close(sd);
          client_socket[i] = 0;
        } else { // 针对未掉线用户，把收到的信息传回去给客户
          std::cout << sd << " :  " << buffer << std::endl;
          std::string msg = std::to_string(sd) + " : " + buffer;
          int k; // string 转 char[]
          for (k = 0; k < msg.length(); k++)
            buffer[k] = msg[k];
          buffer[k] = '\0';
          if (write(sd, buffer, sizeof(buffer)) < 0) {
            perror("Error: Send fail");
            close(sd);
          }
          memset(buffer, 0, BUF_SIZE); // 重置缓冲区
        }
      }
    }
  }
  close(master_socket);
  puts("Server close");
  return 0;
}
