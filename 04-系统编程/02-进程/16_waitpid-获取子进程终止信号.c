#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 创建子进程
    pid_t  pid = fork();
    if(pid == 0){
        // 让子进程死循环，然后通过 kill -9 来终止子进程
        while (1);
    }


    int status;
    waitpid(pid,&status,0);
    printf("子进程执行结束.\n");
    // 如果子进程是因为信号而结束则此宏值为真
    if(WIFSIGNALED(status)){
        // 获取终止的信号
        int sig = WTERMSIG(status);
        printf("子进程因信号退出，信号为: %d \n",sig);
    }
    return 0;

    // 子进程执行结束.
    // 子进程因信号退出，信号为: 9
}
