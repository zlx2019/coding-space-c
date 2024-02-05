#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * 简单点说孤儿进程就是没有父进程的进程，然后就会认init进程作为父进程，完成孤儿进程的善后工作。
 * 通过如下案例，来验证父进程结束后，孤儿进程的变化.
 */

int main(void) {
    int n;
    pid_t pid;
    if((pid = fork()) == -1){
        perror("fork process failed.");
        exit(1);
    }
    if (pid == 0){
        // 子进程
        n = 6;
        while (n > 0){
            printf("我是子进程，我的id为: %d，我的父进程id为: %d \n", getpid(),getppid());
            sleep(1);
            n--;
        }
    }else{
        // 父进程
        n = 3;
        while (n > 0){
            printf("我是父进程，我的id为: %d，我的父进程id为: %d, 我的子进程id为: %d \n",getpid(),getppid(),pid);
            sleep(1);
            n--;
        }
    }
    return 0;

    //我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174
    //我是子进程，我的id为: 29174，我的父进程id为: 29173
    //我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174
    //我是子进程，我的id为: 29174，我的父进程id为: 29173
    //我是父进程，我的id为: 29173，我的父进程id为: 28151, 我的子进程id为: 29174
    //我是子进程，我的id为: 29174，我的父进程id为: 29173
    //我是子进程，我的id为: 29174，我的父进程id为: 29173
    //$ 我是子进程，我的id为: 29174，我的父进程id为: 1
    //我是子进程，我的id为: 29174，我的父进程id为: 1
}
