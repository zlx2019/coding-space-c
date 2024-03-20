#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/**
 * UDP 客户端
 */

#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}

#define PORT 8921
#define BUF_SIZE 1024

int main(void) {
    // 创建 UDP 套接字，并且设置为非阻塞模式
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    Err_Handler(sock, "create socket fail");
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);


    // ready server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
    socklen_t server_addr_len = sizeof(server_addr);

    char line[BUF_SIZE];
    ssize_t n;
    while (1){
        memset(line,0x00, sizeof(line));
        // 读取标准输入
        n = read(0, line, BUF_SIZE);
        if (n == EOF || strncmp(line, "quit", 4) == 0){
            printf("Client exit.\n");
            break;
        }

        // 将数据发送到UDP服务端
        n = sendto(sock, line, n, 0, (struct sockaddr*)&server_addr, server_addr_len);
        if (n == -1) Err_Handler(n, "sendto server fail");

        // 读取服务端响应
        while (1){
            n = recvfrom(sock, line, BUF_SIZE, 0, (struct sockaddr*)&server_addr, &server_addr_len);
            if (n == -1){
                if (errno == EAGAIN){
                    continue;
                }
                Err_Handler(n,"recvfrom server fail");
            }
            write(1, line, n);
            break;
        }
    }
    // close client
    close(sock);
    return 0;
}
