#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * 通过 管道 实现进程之间的通信
 */

#define MAX_LINE 80
#define MSG "hello world\n"

int main() {
    int pid;
    // 管道的读写端
    int chan[2];

    // 1. 创建管道
    if(pipe(chan) == -1){
        perror("create pipe failed:");
        exit(1);
    }
    // 2. 创建子进程
    if ((pid = fork()) < 0){
        perror("fork child process failed:");
        exit(1);
    }
    if (pid == 0){
        // 子进程操作
        // 关闭通道写端
        close(chan[1]);
        // 睡眠3s
        sleep(1);
        // 从通道中读取数据
        char line[MAX_LINE];
        int n = read(chan[0], line, MAX_LINE);
        // 打印到标准输出
        write(STDOUT_FILENO,line,n);
    } else{
        // 父进程操作
        // 关闭通道读端
        close(chan[0]);
        // 向通道内写入数据
        write(chan[1],MSG, sizeof(MSG));
        // 等待子进程终止
        wait(NULL);
    }
    return 0;
}
