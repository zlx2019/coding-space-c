#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * 通过 fork 函数创建子进程，然后分别输出不同的内容
 */

int main(void) {
    // 创建子进程
    pid_t cPid = fork();

    // cPid为-1，表示创建进程失败
    if (cPid == -1){
        perror("fork process failed");
        exit(1);
    }

    // 创建进程成功
    if (cPid > 0){
        // 父进程后续操作
        printf("进程创建完成，我是父进程.\n");
    } else if (cPid == 0){
        // 子进程后续操作
        printf("进程创建完成，我是子进程.\n");
    }
    return 0;
}
