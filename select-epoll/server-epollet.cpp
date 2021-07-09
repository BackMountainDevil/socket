#include <arpa/inet.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define EPOLL_SIZE 10
#define BUF_SIZE 4 // 缓冲区大小,演示边缘触发才设置那么小的，实际要设置大的多
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口
#define CLIENTMAX 10       // 最大连接客户数

int main() {
  struct hostent *host = gethostbyname(DOMAIN); // 由域名获取 IP
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  int serv_sock = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
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

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);

  struct epoll_event ep_events[EPOLL_SIZE];
  struct epoll_event event;
  int epfd = epoll_create(EPOLL_SIZE);
  int event_cnt;

  event.events = EPOLLIN;    // 监听时间类型
  event.data.fd = serv_sock; // 要监听的对象
  epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock,
            &event); // 将主套接字加入监听组 ‘有数据待读取’

  while (true) {
    event_cnt =
        epoll_wait(epfd, ep_events, EPOLL_SIZE, -1); // 阻塞等待监听对象被激活
    if (event_cnt == -1) {
      perror("epoll_wait error");
      break;
    }
    puts("epoll_wait awake"); // 演示触发类型用的

    for (int i = 0; i < event_cnt; i++) {
      if (ep_events[i].data.fd == serv_sock) { // 如果是主套接字
        int clnt_sock =
            accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
          perror("Accept Error");
          continue;
        } else {
          std::cout << "New client：" << clnt_sock << " , IP "
                    << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                    << ntohs(clnt_addr.sin_port) << std::endl;

          int flag = fcntl(clnt_sock, F_GETFL, 0); // 获取原属性
          fcntl(clnt_sock, F_SETFL, flag | O_NONBLOCK); // 给属性加上非阻塞模式

          // 将客户端套接字加入监听组 ‘有数据待读取’
          event.events = EPOLLIN | EPOLLET; // 边缘触发
          event.data.fd = clnt_sock;
          epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
        }
      } else { // 不然就是客户套接字。读取客户端发来的信息，然后返回数据
        while (1) {
          char bufSend[BUF_SIZE];
          int recv_num = read(ep_events[i].data.fd, bufSend, sizeof(bufSend));
          if (recv_num < 0) {
            perror("Error: Receive fail"); // 可以为了输出美观注释掉
            if (errno == EAGAIN) {         // 如果没有数据可以读了
              std::cout << "没有数据可以读了. errno = " << errno
                        << std::endl; // 可以为了输出美观注释掉
              break;
            } else { // Error: Receive fail: Bad file descriptor
                     // 尝试这种办法修复 bug 失败
              /*               std::cout << "Client " << ep_events[i].data.fd
                                      << " disconnect in wired. IP "
                                      << inet_ntoa(clnt_addr.sin_addr) << " ,
                 Port "
                                      << ntohs(clnt_addr.sin_port) << std::endl;
                            epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd,
                 &event); close(ep_events[i].data.fd); */
            }
          } else if (recv_num == 0) { // EOF
            std::cout << "Client " << ep_events[i].data.fd << " disconnect. IP "
                      << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                      << ntohs(clnt_addr.sin_port) << std::endl;

            // 从特定监听组中删除指定客户套接字
            epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, &event);

            close(ep_events[i].data.fd);
          } else {
            std::cout << "Recv " << recv_num << " bytes: " << bufSend
                      << " . From IP " << inet_ntoa(clnt_addr.sin_addr)
                      << " , Port " << ntohs(clnt_addr.sin_port) << std::endl;

            if (write(ep_events[i].data.fd, bufSend, sizeof(bufSend)) < 0) {
              perror("Error: Send fail");
              close(ep_events[i].data.fd);
              exit(EXIT_FAILURE);
            }
          }
        }
      }
    }
  }

  close(serv_sock);
  close(epfd);
  puts("Server close");
  return 0;
}
