#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

/**
 * 使用多线程，实现并发TCP服务器
 */

// 宏定义，用于统一错误处理
#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}

// 连接的客户端信息
typedef struct {
    int fd; // 连接的文件描述符
    int port; // 端口
    const char* ip; // IP
    pthread_t tid; // 服务的线程id
} client_info;

// 处理客户端连接，线程函数
void* client_handler(void* arg){
    // 将参数转换为客户端信息结构
    client_info* client = arg;
    char buf[80]; // 读取缓冲区
    ssize_t read_bytes;

    while (1){
        // 读取数据
        read_bytes = read(client->fd, buf, sizeof(buf));
        if (read_bytes == 0){
            printf("客户端关闭连接 [%s: %d]，服务线程: %lu \n",client->ip,client->port, (unsigned long int)client->tid);
            close(client->fd);
            free(client); // 回收内存
            pthread_exit(NULL); // 结束线程，注意不要使用 exit,该函数会导致整个进程结束;
        }
        printf("[%s:%d]: %s \n",client->ip, client->port,buf);

        // 将数据全部转为大写
        for(int i = 0; i < read_bytes; i++)
            buf[i] = toupper(buf[i]);
        // 将数据写回客户端
        write(client->fd, buf, read_bytes);
    }
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

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

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
        // 客户端信息
        client_info* info = malloc(sizeof(client_info));
        info->fd = client_fd;
        info->port = client_port;
        info->ip = client_ip;

        // 创建新线程，处理连接，与客户端通信
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handler,info);
        info->tid = thread_id;
        printf("客户端连接 [%s :%d]，服务线程: %lu \n",client_ip, client_port,(unsigned long int)thread_id);

        // 将新线程标记为 detach 状态，一旦线程终止，则自动释放资源.
        pthread_detach(thread_id);
    }
    close(listen_fd);
}
