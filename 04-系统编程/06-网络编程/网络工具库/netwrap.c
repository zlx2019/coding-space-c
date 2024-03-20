// 网络相关函数库.
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

// 引入头文件
#include "netwrap.h"

// 结束程序，并且输出错误信息
void Exit(const char* message){
    perror(message);
    exit(1);
}

// 错误处理
void ErrHandler(int no, const char* message){
    if (no == -1){
        Exit(message);
    }
}

// 创建socket
int Socket(int domain, int type, int protocol){
    int fd = socket(domain, type, protocol);
    ErrHandler(fd, "create socket fail");
    return fd;
}

// 开始监听
int Listen(int fd, int backlog){
    int res = listen(fd, backlog);
    ErrHandler(res,"Listen fail");
    return res;
}


// 阻塞等待获取新的连接，返回新的连接文件描述符
int Accept(int fd, struct sockaddr* sockaddr, socklen_t* socklen){
    int client_fd;
    while ((client_fd = accept(fd, sockaddr, socklen)) == -1){
        if (errno == ECONNABORTED || errno == EINTR){
            // 被信号中断，继续服务
            continue;
        }
        Exit("Accept fail");
    }
    return client_fd;
}

// 绑定主机端口
int Bind(int fd, const struct sockaddr* sockaddr, socklen_t socklen){
    int res = bind(fd, sockaddr, socklen);
    ErrHandler(res, "Bind fail");
    return res;
}

// 连接服务端
int Connect(int fd, const struct sockaddr* sockaddr, socklen_t socklen){
    int res = connect(fd, sockaddr, socklen);
    ErrHandler(res, "Connect fail");
    return res;
}

// 关闭连接
int Close(int fd){
    int res = close(fd);
    ErrHandler(res, "Close fail");
    return res;
}


// 读取数据
ssize_t Read(int fd, void* buf, size_t len){
    ssize_t n;
reset:
    if ((n = read(fd, buf, len)) == -1){
        if (errno == EINTR){
            // 信号打断，继续读取
            goto reset;
        } else if (errno == EAGAIN){
            // 非阻塞模式，当前没有数据可读
            return n;
        }
        return -1;
    }
    return n;
}
// 写入数据
ssize_t Write(int fd, const void* buf, size_t len){
    ssize_t n;
reset:
    if ((n = write(fd, buf, len)) == -1){
        if (errno == EINTR){
            // 信号打断
            goto reset;
        }else if (errno == EAGAIN){
            // 非阻塞模式，当前无法写入数据
            return n;
        }
        return -1;
    }
    return n;
}

// 读取数据，直到读取够 bytes 个字节
ssize_t ReadN(int fd, void* buf, size_t bytes){
    size_t unread = bytes; //  还剩余可读数量
    ssize_t count;   // 每次读取到的数量
    char* cursor = buf; // 缓冲区已写入的位置

    while (unread > 0){
        if ((count = read(fd, buf, bytes)) == -1){
            if (errno == EINTR)
                continue;
            else
                return -1;
        } else if (count == 0){
            return 0;
        }
        unread -= count;
        cursor += count;
    }
    return bytes;
}

// 阻塞写入指定数量的数据
ssize_t WriteN(int fd, void* buf, size_t bytes){
    size_t unWrite = bytes; // 未写的数量
    size_t count; // 每次写入的数量
    const char* position = buf;
    while (unWrite > 0){
        if ((count = write(fd, position, unWrite)) <= 0){
            if (errno == EINTR)
                count = 0;
            else
                return -1;
        }
        unWrite -= count;
        position += count;
    }
}