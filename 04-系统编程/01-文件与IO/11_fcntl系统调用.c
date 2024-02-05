#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define MSG_TRY "try again read\n"

/**
 * 使用fcntl 系统调用，动态将标准输入更改为 非阻塞模式
 */

int main(void) {
    char buf[10]; // 定义缓冲区
    int n;        // 读取到的字节数量

    int flags;
    // 1. 获取标准输入的状态标记
    flags = fcntl(STDIN_FILENO,F_GETFL);
    // 2. 将状态标记 追加上 O_NONBLOCK
    flags |= O_NONBLOCK;
    // 3. 将新的状态值，更新回标准输入
    int res = fcntl(STDIN_FILENO,F_SETFL,flags);
    if(res == -1){
        perror("Modify stdin flags:");
        exit(1);
    }

    loop:
    n = read(STDIN_FILENO,buf, sizeof(buf));
    if (n == -1){
        if (errno == EAGAIN){
            write(STDOUT_FILENO,MSG_TRY, sizeof(MSG_TRY));
            sleep(1);
            goto loop;
        }
        perror("read stdin failed:");
        exit(1);
    }
    return 0;
}
