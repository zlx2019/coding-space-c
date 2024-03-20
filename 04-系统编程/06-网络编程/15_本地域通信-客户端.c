#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>


/**
 * 使用本地域套接字，以网络通信形式实现进程之间的通信，使用 TCP 协议.
 * 客户端
 */

#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}


int main() {
    // create socket
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    Err_Handler(socket_fd,"create socket fail");

    // 要连接的 sock 文件地址
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "./local.sock");

    // 连接
    int err = connect(socket_fd,(struct sockaddr*)&addr, sizeof(addr));
    Err_Handler(err, "connect sock  fail")
    printf("Connect sock success~ \n");

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
