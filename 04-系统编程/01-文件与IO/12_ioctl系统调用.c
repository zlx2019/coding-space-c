#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/**
 * 
 */

int main(int argc, char *argv[]) {
    // 终端窗口 大小信息结构体
    struct winsize windowSize;

    // 是否连接到终端
    if (isatty(STDOUT_FILENO) == 0){
        exit(1);
    }

    // 通过ioctl函数 对终端设备发送命令
    // 发送 TIOCGWINSZ 命令，获取终端窗口的大小信息
    if (ioctl(STDOUT_FILENO,TIOCGWINSZ,&windowSize) < 0){
        perror("ioctl TIOCGWINSZ error:");
        exit(1);
    }
    printf("高度: %d, 宽度: %d \n",windowSize.ws_row, windowSize.ws_col);
    return 0;
}
