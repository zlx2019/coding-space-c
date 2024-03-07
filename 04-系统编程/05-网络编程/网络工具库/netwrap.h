//
// Created by Zero on 2024/2/27.
// 网络相关函数库封装 头文件.

#ifndef NET_WRAP_H
#define NET_WRAP_H
#include <sys/socket.h>


#ifdef __cplusplus
    extern "C"{
#endif
    // 创建socket
    int Socket(int domain, int type, int protocol);
    // 开始监听
    int Listen(int fd, int backlog);
    // 从监听描述符中以阻塞式获取连接
    int Accept(int fd, struct sockaddr* sockaddr, socklen_t* socklen);
    // 绑定主机端口
    int Bind(int fd, const struct sockaddr* sockaddr, socklen_t socklen);
    // 连接服务端
    int Connect(int fd, const struct sockaddr* sockaddr, socklen_t socklen);
    // 关闭连接
    int Close(int fd);

    // 读取数据
    ssize_t Read(int fd, void* buf, size_t len);
    // 写入数据
    ssize_t Write(int fd, const void* buf, size_t len);

    // 阻塞读取指定数量的数据
    ssize_t ReadN(int fd, void* buf, size_t bytes);
    // 阻塞写入指定数量的数据
    ssize_t WriteN(int fd, void* buf, size_t bytes);





#ifdef __cplusplus
}
#endif
#endif NET_WRAP_H
