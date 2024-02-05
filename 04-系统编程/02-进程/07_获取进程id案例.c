#include <stdio.h>
#include <unistd.h>

/**
 * getpid() 获取当前进程id
 * getppid() 获取父进程id
 */

int main(void) {
    // 创建子进程
    pid_t pid = fork();
    if (pid == 0){
        // 子进程
        printf("我是子进程，我的id为: %d，我的父进程id为: %d \n",getpid(),getppid());
    }else if(pid > 0){
        // 父进程
        printf("我是父进程，我的id为 %d, 我的子进程id为: %d \n",getpid(),pid);
    }

    // 我是父进程，我的id为 5866, 我的子进程id为: 5867
    // 我是子进程，我的id为: 5867，我的父进程id为: 5866
    return 0;
}
