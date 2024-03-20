#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/event.h>
#include <fcntl.h>


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

#define MAX_FD_NUM 1024

/**
 * 使用 Mac系统中的kqueue 实现多路复用TCP服务端
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


    /// 1. 创建 kqueue 事件队列
    int kq = kqueue();
    Err_Handler(kq, "create kqueue fail");
    struct kevent eventsIn[MAX_FD_NUM]; // 要监听的事件队列
    int eventSize = 0; // 监听事件队列的元素数量
    struct kevent eventsOut[MAX_FD_NUM]; // 内核返回的就绪事件队列
    int ready; // 就绪的事件数量

    /// 2. 将服务的监听描述符 注册到监听队列
    // 将 server 描述符，注册到 eventsIn队列
    // EVFILT_READ 表示监听可读事件
    // EV_ADD | EV_ENABLE 表示添加并且开启事件
    EV_SET(eventsIn, server, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    eventSize++;


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
    ssize_t n; // 读取到的字节数
    /// ===========================================

    // enable loop thread
    while (1){
        /// 阻塞等待事件就绪
        ready = kevent(kq, eventsIn, eventSize, eventsOut, MAX_FD_NUM, NULL);
        if (ready == -1){
            if (errno == EINTR) continue;
            Error("kevent wait fail: %s", strerror(errno));
            break;
        }
        /// 有事件就绪，处理事件列表
        for (int i = 0; i < ready; i++) {
            if (eventsOut[i].ident == server){
                /// 是服务监听描述符，表示有新的连接请求
                client = accept(server, (struct sockaddr*)&client_addr, &client_addr_len);
                // 设置描述符为非阻塞
                int flags = fcntl(client, F_GETFL);
                fcntl(client, F_SETFL, flags | O_NONBLOCK);
                /// 将新连接注册到，监听事件队列
                EV_SET(eventsIn, client, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

                inet_ntop(AF_INET,&client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                client_port = ntohs(client_addr.sin_port);
                Log("[%s:%d] Connection~ ",client_ip, client_port);
            } else{
                /// 是客户端描述符，数据可读事件
                client = eventsOut[i].ident;

                // 获取客户端的相关信息
                getpeername(client, (struct sockaddr*)&client_addr, &client_addr_len);
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                client_port = ntohs(client_addr.sin_port);

                /// 读取数据
                memset(buf, 0x00, sizeof(buf));
                n = read(client, buf, sizeof(buf));
                if (n == 0){
                    // 连接关闭
                    // 客户端连接关闭，将描述符从事件队列中移除
                    close(client);
                    EV_SET(eventsIn, client, EVFILT_READ, EV_DELETE | EV_ENABLE, 0, 0, NULL);
                    eventSize--;
                    Log("[%s : %d] Closed.", client_ip, client_port);
                    continue;
                } else if (n == -1){
                    if (errno == EAGAIN)continue;
                    break;
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
