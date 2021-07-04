#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#define BUF_SIZE 100
#define IP "127.0.0.1"
#define PORT 8080

int main(){
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        std::cout<<"Error: Socket creation failed"<<std::endl;
        return -1;
    }

    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    // serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(PORT);  //端口

    // 将 IP 地址从文本转换为二进制格式并存储到 sin_addr 中
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) 
    {
        std::cout<<"Error: Invalid address/ Address not supported"<<std::endl;
        return -1;
    }

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        std::cout<<"Error: Connection creation failed"<<std::endl;
        return -1;
    }

    // 获取用户输入的字符串并发送给服务器
    char buffer[BUF_SIZE] = {0};
    printf("Input a string: ");
    
    // scanf("%s", buffer);
    // fgets(buffer,BUF_SIZE,stdin);
    std::cin.getline(buffer, BUF_SIZE);

    write(sock, buffer, sizeof(buffer));

    //读取服务器传回的数据
    read(sock, buffer, sizeof(buffer)-1);
    printf("Message form server: %s\n", buffer);
    
    //关闭套接字
    close(sock);
    return 0;
}