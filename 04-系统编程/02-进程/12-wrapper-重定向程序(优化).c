#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/**
 * 重定向辅助命令行工具
 * 通过命令行参数 接收两个文件，分别作为当前进程的标准输入和标准输出，然后执行 upper 程序;
 *
 * 编译该程序 gcc -o wrapper 12-wrapper-重定向程序(优化).c
 *
 * 执行程序: ./wrapper [file]
 */

int main(int argc, char *argv[]) {
    // 参数校验
    if(argc != 3){
        fprintf(stderr,"usage: %s [input-file] [output-file]\n",argv[0]);
        exit(1);
    }

    // 打开要重定向的 标准输入 文件
    int in_fd = open(argv[1],O_RDONLY);
    if (in_fd < 0){
        fprintf(stderr, "open %s failed: %s",argv[1],strerror(errno));
        exit(1);
    }
    // 将当前进程的标准输入指向文件描述符fd
    dup2(in_fd,STDIN_FILENO);
    // 关闭fd
    close(in_fd);

    // 打开或创建要重定向的 标准输出 文件
    int out_fd = open(argv[2],O_WRONLY | O_CREAT,0644);
    if (out_fd < 0){
        fprintf(stderr,"open %s failed: %s",argv[2], strerror(errno));
        exit(1);
    }
    dup2(out_fd,STDOUT_FILENO);
    close(out_fd);

    // 执行 upper 程序
    execl("./upper","upper",NULL);
    perror("exec upper failed.");
    return 0;
}
