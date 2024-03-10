#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#ifdef _WIN32
// Windwos system
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib");
#else
// Unix system
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

/**
 * 实现一个简单的TCP服务端 Echo
 */


// 宏定义，用于统一错误处理
#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}

int main() {
    // 1. 创建Tcp服务端的监听socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(listen_fd,"create socket fail")

    // 2. 要占用的主机地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // 指定协议版本
    server_addr.sin_port = htons(8921);  // 指定端口号(转为大端字节序)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定IP  INADDR_ANY 表示为0，也就是0.0.0.0
    // inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr); // 指定IP

    // 3. 将 socket 与一个地址进行绑定，表示要占用的主机端口
    int res_no = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    Err_Handler(res_no,"socket bind fail");

    // 4. 将 socket 标记为服务监听，作为服务端
    Err_Handler(listen(listen_fd, 1),"listen fail");

    printf("Accepting connections... \n");

    // 用于记录连接的客户端信息
    int client_fd;  // 客户端连接文件描述符
    struct sockaddr_in client_addr; // 客户端地址
    socklen_t client_addr_len = sizeof(client_addr);
    char buf[80]; // 读取缓冲区
    ssize_t read_bytes; // 读取的数据量
    while (1){
        // 5. 阻塞等待获取连接
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        // 获取连接的IP地址
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

        // 获取连接的端口号
        int client_port = ntohs(client_addr.sin_port);
        printf("客户端连接，IP: %s, Port: %d \n",client_ip, client_port);

        while (1){
            // 6. 读取数据
            read_bytes = read(client_fd, buf, sizeof(buf));
            if (read_bytes == 0){
                printf("客户端已经关闭连接... \n");
                break;
            }
            printf("读取到的数据: %s \n", buf);

            // 将数据全部转为大写
            for(int i = 0; i < read_bytes; i++)
                buf[i] = toupper(buf[i]);

            // 7. 将数据写回客户端
            write(client_fd, buf, read_bytes);
        }
        // 关闭客户端
        close(client_fd);
        bzero(buf, sizeof(buf));
    }
}
