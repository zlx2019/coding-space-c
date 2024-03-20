#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/**
 * 使用本地域套接字，以网络通信形式实现进程之间的通信，使用 TCP 协议.
 * 服务端
 */

#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}


int main(void) {
    // create socket
    // 注意, 协议使用的是 AF_UNIX 或者 AF_LOCAL 也可以
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    Err_Handler(server,"socket fail");

    // 创建并且绑定本地域通信文件
    // 指定协议和 sock 通信文件路径，通过该文件进行通信
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "./local.sock"); // 指定通信文件路径
    // 预删除该文件，防止已存在导致出错.
    unlink("./local.sock");

    // bind sock file
    int err = bind(server, (struct sockaddr*)&addr, sizeof(addr));
    Err_Handler(err, "bind fail");

    // listen
    err = listen(server,128);
    Err_Handler(err, "listen fail");

    printf("local socket server running...\n");

    int client_fd;
    struct sockaddr_un client;
    socklen_t client_len = sizeof(client);
    // 接收连接
    while (1){
        client_fd = accept(server,(struct sockaddr*)&client, &client_len);
        Err_Handler(client_fd, "accept client fail");

        // 读取数据
        char buf[BUFSIZ];
        int n;
        while (1){
            memset(buf, 0x00, BUFSIZ);
            n = read(client_fd, buf, BUFSIZ);
            if (n <= 0){
                printf("Client Close. \n");
                break;
            }
            printf("%s",buf);
            for (int i = 0; i < n; i++)
                buf[i] = toupper(buf[i]);
            write(client_fd, buf, n);
        }
    }
    close(server);
    return 0;
}
