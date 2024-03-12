#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>

/**
 * Epoll (ET + Reactor)
 *  基于 ET 边缘触发工作模式，以及反应堆思想，实现高性能TCP服务端
 */

// 相关宏定义
#define INFO_COLOR "\033[0;32m"
#define DEBUG_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define COLOR_END "\033[0m"
#define Log(format, ...) fprintf(stdout, INFO_COLOR"[INFO] " format "\n" COLOR_END, ##__VA_ARGS__)
#define Debug(format, ...) fprintf(stdout, DEBUG_COLOR"[Debug] " format "\n" COLOR_END, ##__VA_ARGS__)
#define Error(format, ...) fprintf(stderr, ERROR_COLOR"[Error] " format "\n" COLOR_END, ##__VA_ARGS__)
#define Err_Handler(res_no,message) if(res_no == -1){Error("%s:%s",message, strerror(errno));exit(1);}

/**
 * 自定义事件数据信息结构体，将对应的事件处理信息封装为该结构，一并注册到epoll中，然后统一回调即可.
 */
typedef struct event_payload{
    // 事件对应的描述符
    int fd;
    // 事件类型
    int events;
    // 事件处理回调函数，事件触发后调用该函数
    void (* callback)(int fd, struct event_payload* payload);
    // 事件缓冲区
    char buf[1024];
    // 缓冲区的数据量(读取到的字节数 or 要写出的字节数)
    int len;
} event_payload;


/// =============== 临时客户端信息 ================
int client; // 客户端描述符
struct sockaddr_in client_addr; // 客户端地址信息
socklen_t client_addr_len = sizeof(client_addr); // 地址长度
char client_ip[INET_ADDRSTRLEN];// 客户端IP（字符串）
int client_port; // 客户端端口
/// =============================================


/// ================= 事件回调处理函数 =============
void acceptEventCallback(int listen, event_payload* payload);
void readEventCallback(int fd, event_payload* payload);
void writeEventCallback(int fd, event_payload* payload);
/// =============================================


/// ================= 事件操作相关函数 =============
void addEvent(int fd, int events, void(* callback)(int, event_payload*));
void setEvent(event_payload* payload, int events, void(*callback)(int, event_payload*));
/// =============================================


// 全局 epoll 实例描述符
int epoll;

/* main  */
int main(void){

    /// init network...
    int server = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(server, "create listen socket fail");
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in listenAddr;
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(8921);
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int no = bind(server,(struct sockaddr*)&listenAddr, sizeof(listenAddr));
    Err_Handler(no, "bind the listen the address");
    no = listen(server, 128);
    Err_Handler(no, "listen server fail");


    Log("Server running success...");

    /// 1. 创建 epoll 实例
    epoll = epoll_create(1024);
    Err_Handler(no, "create epoll instance fail");


    /// TODO 将listen socket 注册到 epoll 实例中，监听可读事件
    addEvent(server,EPOLLIN | EPOLLET, acceptEventCallback);

    // 用于存放由于事件就绪而被归还的事件数组
    struct epoll_event readyEvents[1024];
    // 每次内核告知的就绪的事件数量
    int ready;

    // loop thread wait io event
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
            /// 获取事件信息的数据载体
            event_payload* payload = readyEvents[i].data.ptr;
            /// 执行该事件的回调函数
            payload->callback(payload->fd, payload);
        }
    }
    // close server listen socket
    close(server);
    return 0;
}



/**
 * 将描述符注册到 epoll 实例中
 * @param fd        要注册的fd
 * @param events    要监听的事件
 * @param callback  事件触发后回调的处理函数
 * @param arg       处理函数参数
 */
void addEvent(int fd, int events, void(* callback)(int, event_payload*)){
    // 定义事件信息，申请内存
    event_payload* payload = malloc(sizeof(event_payload));
    // 设置事件信息
    payload->fd = fd;
    payload->events = events;
    payload->callback = callback;
    // 定义 epoll 事件对象
    struct epoll_event event;
    event.events = payload->events;
    event.data.ptr = payload; // 设置事件载体信息
    // 将事件注册到epoll中
    epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &event);
}


/**
 * 修改事件信息的类型、回调函数.
 * @param payload   要修改的事件信息
 * @param events    新的事件类型
 * @param callback  新的事件回调处理函数
 */
void setEvent(event_payload* payload, int events, void(*callback)(int, event_payload*)){
    payload->events = events;
    payload->callback = callback;
    struct epoll_event event;
    event.events = events;
    event.data.ptr = payload;
    // 修改事件
    epoll_ctl(epoll, EPOLL_CTL_MOD, payload->fd, &event);
}

/**
 * 连接事件回调处理函数，负责接收新的连接，并且注册到epoll实例中;
 *
 * @param listen  发生事件的描述符(listen fd)
 * @param payload 事件信息
 */
void acceptEventCallback(int listen, event_payload* payload){
    // 接收新的客户端连接
    client = accept(listen, (struct sockaddr*)&client_addr, &client_addr_len);
    // 将连接设置为非阻塞式
    int flags = fcntl(client, F_GETFL);
    fcntl(client, F_SETFL, flags | O_NONBLOCK);
    // 注册到 epoll 实例中，设置监听类型、工作模式以及事件回调处理函数
    addEvent(client, EPOLLIN | EPOLLET, readEventCallback);
    inet_ntop(AF_INET,&client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    Log("[%s:%d] Connection~ ",client_ip, ntohs(client_addr.sin_port));
}

/**
 * 可读数据事件回调处理函数，负责把连接中的数据读取到事件缓冲区.
 *
 * @param fd        可读事件就绪的描述符
 * @param payload   事件信息
 */
void readEventCallback(int fd, event_payload* payload){
    client = fd;
    // 获取客户端的相关信息
    getpeername(client, (struct sockaddr*)&client_addr, &client_addr_len);
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    client_port = ntohs(client_addr.sin_port);
    /// 由于是 ET 边缘触发工作模式，所以需要一次性读完数据
    // 每次循环读取到的数据量
    int count;
    memset(payload->buf, 0x00, sizeof(payload->buf));
    while (1){
        // 读取数据(非阻塞式)
        count = read(client, payload->buf, sizeof(payload->buf));
        switch (count) {
            case 0:
                // 客户端关闭连接，从epoll实例中移除
                close(client);
                epoll_ctl(epoll, EPOLL_CTL_DEL, client, NULL);
                // 释放事件信息内存
                free(payload);
                Log("[%s:%d] Closed.", client_ip, client_port);
                break;
            case -1:
                // 数据读取完毕
                if (errno == EAGAIN){
                    /// 数据全部读取到缓冲区完毕，将该描述符的监听事件修改为 可写事件
                    /// 然后由 epoll 回调写事件处理函数，实现数据的写回
                    setEvent( payload,EPOLLOUT, writeEventCallback);
                    goto end;
                }
                break;
            default:
                payload->len += count;
                printf("[%s:%d]: %s", client_ip, client_port, payload->buf);
                // 数据未读取完，继续
                continue;
        }
        end:
        break;
    }
}

/**
 * 可写数据事件回调处理函数，负责将事件缓冲区中的数据写入连接。
 *
 * @param fd        触发事件的描述符
 * @param payload   事件信息
 */
void writeEventCallback(int fd, event_payload* payload){
    // 内容转为大写
    for (int i = 0; i < payload->len; i++)
        payload->buf[i] = toupper(payload->buf[i]);
    // 写回客户端
    write(fd, payload->buf, payload->len);
    /// 将事件类型 重置为可读事件
    payload->len = 0;
    setEvent(payload, EPOLLIN | EPOLLET, readEventCallback);
}