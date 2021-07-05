#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>

int main() {
  /*   在 linux 上使用 gethostbyname 通过域名获取 IP 信息
        博客域名（IP信息很奇怪）：https://backmountaindevil.github.io/
  */
  struct hostent *host = gethostbyname("www.baidu.com");
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }
  //输出主机的规范名
  printf("Official: %s\n", host->h_name);

  //别名
  for (int i = 0; host->h_aliases[i]; i++) {
    printf("Aliases %d: %s\n", i + 1, host->h_aliases[i]);
  }
  //地址类型,IPv4 对应 AF_INET，IPv6 对应 AF_INET6。
  printf("Address type: %s\n",
         (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
  // IP地址
  for (int i = 0; host->h_addr_list[i]; i++) {
    printf("IP addr %d: %s\n", i + 1,
           inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
  }

  return 0;
}