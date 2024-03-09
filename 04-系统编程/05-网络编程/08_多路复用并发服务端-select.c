#include <stdio.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define Err_Handler(res_no,message){ \
    if(res_no == -1){                \
        perror(message);             \
        exit(1);                     \
   }                                 \
}

/**
 * 基于IO多路复用的高并发TCP服务端，使用select实现
 */

int main() {
    // 创建监听socket IPv4, TCP
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    Err_Handler(listen_fd, "create listen socket fail");

    // 设置端口复用
    int opt = 1;
    int err = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    Err_Handler(err, "set port reuse fail")

    // 准备IP和Port
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(8921);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 将 listen 与IP和Port绑定
    err = bind(listen_fd,(struct sockaddr*)&listen_addr, sizeof(listen_addr));
    Err_Handler(err, "bind fail")

    // 开始监听
    listen(listen_fd, 128);

    /// 定义用于传入要监听和传出已经就绪的描述符集合(读取事件，连接也属于可读取事件)
    fd_set in_fds, out_fds;
    FD_ZERO(&in_fds);
    FD_ZERO(&out_fds);

    /// 将监听描述符设置进集合
    FD_SET(listen_fd,&in_fds);

    /// 监听的描述符中最大的
    int cur_max_fd = listen_fd;
    /// 每次唤醒后，就绪的fd的数量
    int ready_n;


    /// =============== 临时处理的客户端连接信息 ========
    int client_fd; // 客户端连接描述符
    struct sockaddr_in client_addr; // 连接地址信息
    socklen_t client_addr_len;
    char client_ip[INET_ADDRSTRLEN]; // 客户端IP
    int client_port; // 客户端Port
    /// =============================================

    printf("Server running success...\n");

    /// 循环处理
    while (1){
        out_fds = in_fds;
        // 阻塞监听描述符，等待事件就绪
        ready_n = select(cur_max_fd + 1,&out_fds, NULL, NULL, NULL);

        // ...事件就绪了，被唤醒
        if (ready_n == -1){
            if (errno == EINTR)continue; // 被信号打断，忽略即可
            break;
        }

        /// TODO 开始处理事件就绪的fd...

        /// TODO 连接事件
        if (FD_ISSET(listen_fd, &out_fds)){
            /// 监听描述符触发连接事件，接收新的客户端连接
            client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len);
            Err_Handler(client_fd, "accept fail");
            inet_ntop(AF_INET,&client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("[%s:%d] Connection~ \n", client_ip, ntohs(client_addr.sin_port));

            // 将新的连接描述符，加入集合中，在下一轮循环中开启监听
            FD_SET(client_fd, &in_fds);
            if (client_fd > cur_max_fd){
                // 更新select监听的描述符范围
                cur_max_fd = client_fd;
            }
            ready_n--;
        }

        /// TODO 数据可读事件
        char buf[1024];
        int n;
        // 从监听描述符开始向后遍历，依次判断每个描述符是否在就绪集合中，直到监听范围的最后一个描述符
        for (int i = listen_fd + 1; ready_n > 0 && i <= cur_max_fd; i++) {
            if (FD_ISSET(i, &out_fds)){
                // 可读
                client_fd = i;
                // 获取该描述符对应的客户端地址信息
                getpeername(client_fd, (struct sockaddr*)&client_addr, &client_addr_len);
                // 将IP转换成字符串
                inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                // 获取端口
                client_port = ntohs(client_addr.sin_port);
                bzero(buf, sizeof(buf));
                ready_n--;

                // 读取数据
                if (( n = read(client_fd, buf, sizeof(buf))) == 0){
                    // 连接关闭
                    close(client_fd);
                    // 移除该描述符，不再需要监听
                    FD_CLR(client_fd, &in_fds);
                    printf("[%s : %d] Closed.\n", client_ip, client_port);
                    continue;
                }

                // 输出数据内容
                printf("[%s : %d]: %s",client_ip, client_port, buf);

                // 转换为大写后，写回客户端
                for(int j = 0; j < n; j++)
                    buf[j] = toupper(buf[j]);
                write(client_fd, buf, n);
            }
        }
    }
    close(listen_fd);
    return 0;
}
