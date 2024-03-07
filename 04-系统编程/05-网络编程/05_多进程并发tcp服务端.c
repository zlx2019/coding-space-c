#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

/**
 * 引入多进程，实现并发TCP服务器
 */

// 宏定义，用于统一错误处理
#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}

// 子进程终止信号处理
void child_handler(int sig_no){
    int child_pid = wait(NULL);
    printf("子进程 %d 终止了\n",child_pid);
}

int main() {
    // 创建TCP服务监听socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(listen_fd,"create socket fail")
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8921);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定端口
    int res_no = bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    Err_Handler(res_no,"socket bind fail");

    // 开启监听
    Err_Handler(listen(listen_fd, 1),"listen fail");
    printf("Accepting connections... \n");


    // 为父进程设置 SIGCHILD 信号处理
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = child_handler;
    sigaction(SIGCHLD, &act, NULL);

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buf[80];
    ssize_t read_bytes;
    while (1){
        // 阻塞等待客户端连接
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1 && errno == EINTR){
            // 被信号打断，继续服务即可
            continue;
        }
        Err_Handler(client_fd, "accept fail");

        // 获取连接的IP地址
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        // 获取连接的端口号
        int client_port = ntohs(client_addr.sin_port);


        /// 创建子进程，与客户端通信
        pid_t child_pid = fork();
        if (child_pid == -1){
            perror("fork child fail");
            exit(1);
        }
        if (child_pid > 0){
            // 父进程
            printf("客户端连接 [%s :%d]，服务进程: %d \n",client_ip, client_port,child_pid);
            close(client_fd); // 父进程不需要和客户端通信，直接关闭即可
            continue;
        } else{
            // 子进程
            close(listen_fd); // 子进程不需要监听套接字，直接关闭即可
            while (1){
                // 读取数据
                read_bytes = read(client_fd, buf, sizeof(buf));
                if (read_bytes == 0){
                    printf("客户端关闭连接 [%s: %d] \n",client_ip,client_port);
                    close(client_fd);
                    exit(1);
                }
                printf("[%s:%d]: %s \n",client_ip, client_port,buf);

                // 将数据全部转为大写
                for(int i = 0; i < read_bytes; i++)
                    buf[i] = toupper(buf[i]);

                // 将数据写回客户端
                write(client_fd, buf, read_bytes);
            }
        }
    }
}
