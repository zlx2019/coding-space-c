#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * 使用 fork 创建出一个子进程，主进程和子进程分别输出不同次数的消息
 */
int main(void) {
    char* message;
    int n;

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1){
        perror("fork process failed");
        exit(1);
    }
    if (pid == 0){
        // 子进程
        message = "我是子进程~\n";
        n = 6;
    }else{
        // 主进程
        message = "我是主进程~\n";
        n = 3;
    }
    while (n > 0){
        printf("%s",message);
        sleep(1);
        n--;
    }
    return 0;
}
