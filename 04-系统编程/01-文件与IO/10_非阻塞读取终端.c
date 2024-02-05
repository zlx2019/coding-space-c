#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define MSG_TRY "try again read\n"

/**
 * 通过标准输入，以非阻塞模式读取输入
 */
int main(void) {
    char buf[10]; // 定义缓冲区
    int n;        // 读取到的字节数量
    int stdfd;       // 重新打开的终端设备描述符

    // 打开终端设备，以非阻塞模式
    stdfd = open("/dev/tty",O_RDONLY | O_NONBLOCK);
    if(stdfd == -1){
        perror("open terminal failed: ");
        exit(1);
    }

    // 以非阻塞模式循环读取标准输入
    loop_read:
    n = read(stdfd,buf, sizeof(buf));
    if (n == -1){
        if(errno == EAGAIN){// 表示目前没有数据可读
            sleep(1); // 睡眠1秒
            write(STDOUT_FILENO,MSG_TRY, sizeof(MSG_TRY));
            goto loop_read; // 跳转回去继续尝试读取
        }
        perror("read stdin failed:");
        exit(1);
    }

    // 写回到标准输出
    write(STDOUT_FILENO,buf,n);
    close(stdfd);
    return 0;
}
