#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * UDP 服务端
 */

#define PORT 8921
#define BUF_SIZE 1024

int main() {
    // 创建socket
    // IPv4，UDP
    int server = socket(AF_INET, SOCK_DGRAM, 0);

    // 定义服务端地址
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // socket 绑定 IP + Port
    if (bind(server,(struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
        perror("bind socket fail");
        return 1;
    }
    printf("UDP server ready... \n");

    // 循环阻塞接收来自于客户端的数据
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];
    char buf[BUF_SIZE];
    int n;
    while (1){
        bzero(buf, sizeof(buf));
        n = recvfrom(server, buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (n < 0){
            perror("recvfrom client fail");
            exit(1);
        }
        // 将IP转换为字符串
        inet_ntop(AF_INET,&client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%s:%d]: %s",client_ip, ntohs(client_addr.sin_port), buf);

        for (int i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);

        // 将数据重新写回客户端
        n = sendto(server, buf, n, 0, (struct sockaddr*)&client_addr,client_addr_len);
        if (n < 0){
            perror("sendto client fail");
            exit(1);
        }
    }
    close(server);
    return 0;
}
