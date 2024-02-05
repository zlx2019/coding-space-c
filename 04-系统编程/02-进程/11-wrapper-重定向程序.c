#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/**
 * 重定向辅助命令行工具
 * 通过当前程序，将指定的文件重定向到 upper 程序中，作为标准输入，然后执行 upper 程序;
 *
 * 编译该程序: gcc -o wrapper 11-wrapper-重定向辅助程序.c
 * 执行程序: ./wrapper [file]
 */

int main(int argc, char *argv[]) {
    // 参数校验
    if(argc != 2){
        fprintf(stderr,"usage: %s [input-file] [output-file]\n",argv[0]);
        exit(1);
    }

    // 打开要重定向的文件
    int fd = open(argv[1],O_RDONLY);
    if (fd < 0){
        fprintf(stderr, "open %s failed: %s",argv[1],strerror(errno));
        exit(1);
    }
    // 将当前进程的标准输入指向文件描述符fd
    dup2(fd,STDIN_FILENO);
    // 关闭fd
    close(fd);

    // 执行 upper 程序
    execl("./upper","upper",NULL);
    perror("exec upper failed.");
    return 0;
}
