#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/epoll.h>


// 终端字体颜色
#define INFO_COLOR "\033[0;32m"
#define DEBUG_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define COLOR_END "\033[0m"

#define Log(format, ...) fprintf(stdout, INFO_COLOR"[INFO] " format "\n" COLOR_END, ##__VA_ARGS__)
#define Debug(format, ...) fprintf(stdout, DEBUG_COLOR"[Debug] " format "\n" COLOR_END, ##__VA_ARGS__)
#define Error(format, ...) fprintf(stderr, ERROR_COLOR"[Error] " format "\n" COLOR_END, ##__VA_ARGS__)

// 错误处理宏定义
#define Err_Handler(res_no,message) if(res_no == -1){ \
    Error("%s:%s",message, strerror(errno));          \
    exit(1);                                          \
}

/**
 * 使用 epoll多路复用机制实现高性能TCP并发服务器(单线程)
 *
 */
int main(void){
    // create server listen socket
    int server = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(server, "create listen socket fail");

    // set socket allow reuse address
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // init listen address and port
    struct sockaddr_in listenAddr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(8921);
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind address
    int no = bind(server,(struct sockaddr*)&listenAddr, sizeof(listenAddr));
    Err_Handler(no, "bind the listen the address");

    // server listen socket start
    no = listen(server, 128);
    Err_Handler(no, "listen server fail");

    Log("Server running success...");

    /// 1. 创建 epoll 实例
    int epoll = epoll_create(1024);
    Err_Handler(no, "create epoll instance fail");
    // 创建一个通用的 epoll event 事件对象
    struct epoll_event event;

    /// 2. 将服务监听 socket 注册到 epoll 实例
    event.data.fd = server; // 设置描述符
    event.events = EPOLLIN; // 设置事件类型为 可读事件
    epoll_ctl(epoll, EPOLL_CTL_ADD, server, &event);

    /// =============== epoll 返回信息 ===============
    // 用于存放由于事件就绪而被归还的事件数组
    struct epoll_event readyEvents[1024];
    // 每次内核告知的就绪的事件数量
    int ready;
    /// =============================================

    /// =============== 可读事件就绪的客户端信息 ================
    // 客户端描述符
    int client;
    // 客户端地址信息
    struct sockaddr_in client_addr;
    // 地址长度
    socklen_t client_addr_len = sizeof(client_addr);
    // 客户端IP（字符串）
    char client_ip[INET_ADDRSTRLEN];
    // 客户端端口
    int client_port;
    /// =============================================

    /// =================读取数据相关================
    char buf[1024]; // 读取缓冲区
    int n; // 读取到的字节数
    /// ===========================================


    // enable loop thread
    while (1){
        // 阻塞等待 epoll 实例中有io event触发
        ready = epoll_wait(epoll, readyEvents, 1024, -1);
        if (ready == -1){
            if (errno == EINTR) continue;
            Err_Handler(-1, "epoll wait fail");
            break;
        }

        /// 有事件就绪，处理事件列表
        for (int i = 0; i < ready; i++) {
            if (readyEvents[i].data.fd == server){
                /// 表示为服务监听描述符，有新的连接请求
                client = accept(server, (struct sockaddr*)&client_addr, &client_addr_len);

                /// 将新的连接，注册到 epoll 实例中
                event.data.fd = client;
                event.events = EPOLLIN;
                epoll_ctl(epoll, EPOLL_CTL_ADD, client, &event);

                inet_ntop(AF_INET,&client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                client_port = ntohs(client_addr.sin_port);
                Log("[%s:%d] Connection~ ",client_ip, client_port);
            } else{
                /// 客户端连接可读事件
                client = readyEvents[i].data.fd;

                // 获取客户端的相关信息
                getpeername(client, (struct sockaddr*)&client_addr, &client_addr_len);
                // IP
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                // Port
                client_port = ntohs(client_addr.sin_port);

                /// 读取数据
                memset(buf,0x00, sizeof(buf));
                n = read(client, buf, sizeof(buf));
                if (n == 0){
                    // 客户端连接关闭
                    close(client);
                    /// 将该连接描述符，从 epoll 实例中移除
                    epoll_ctl(epoll, EPOLL_CTL_DEL, client, NULL);
                    Log("[%s : %d] Closed.", client_ip, client_port);
                    continue;
                }

                // 输出数据
                printf("[%s:%d]: %s", client_ip, client_port, buf);

                // 转换为大写，写回客户端
                for(int j = 0; j < n; j++)
                    buf[j] = toupper(buf[j]);
                write(client, buf, n);
            }
        }
    }

    // close server listen socket
    close(server);
    return 0;
}
