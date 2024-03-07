#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/**
 * 实现一个简单的TCP客户端 Echo
 */

// 宏定义，用于统一错误处理
#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}


int main() {
    //1. 创建 客户端 socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(socket_fd,"create socket fail");

    //2. TCP服务端的地址信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // 指定协议版本
    server_addr.sin_port = htons(8921);  // 指定端口号(转为大端字节序)
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr); // 指定IP

    //3. 连接服务端
    int res_no = connect(socket_fd,(struct sockaddr*)&server_addr, sizeof(server_addr));
    Err_Handler(res_no, "connect server fail")
    // 连接成功
    printf("Connect server success~ \n");

    char line[1024];
    ssize_t n;
    while (1){
        // 读取标准输入
        n = read(0, line, sizeof(line));
        if (n == EOF || strncmp(line, "quit", 4) == 0){
            printf("Client exit. \n");
            break;
        }
        //4. 将数据发送给服务端
        write(socket_fd, line, n);

        //5. 读取服务端响应
        n = read(socket_fd, line, sizeof(line));
        if (n == 0){
            printf("Server exit.\n");
            break;
        }

        // 输出到标准输出
        write(1, line, n);
    }
    // 6. 关闭服务端连接
    close(socket_fd);
    return 0;
}
