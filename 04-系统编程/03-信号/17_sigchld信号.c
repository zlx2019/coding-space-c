#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * 请编写一个程序完成以下功能：父进程fork出子进程，子进程调用exit(2)终止，父进程自定
    义SIGCHLD信号的处理函数，在其中调用wait获得子进程的退出状态并打印。
 */


// 自定义子进程终止信号的 处理函数
void child_handler(int sig_no){
    int child_status;
    int child_pid = wait(&child_status);
    printf("子进程 %d 终止了，终止状态为: %d \n",child_pid, WEXITSTATUS(child_status));
}

int main() {

    // 创建子进程
    pid_t pid = fork();
    if (pid == -1){
        perror("fork process");
        return 1;
    }
    if (pid){
        // parent process
        // 设置 SIGCHLD 信号处理函数
        struct sigaction act;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask);
        act.sa_handler = child_handler;
        sigaction(SIGCHLD, &act, NULL);

        // 3秒后结束程序.
        sleep(3);
    } else{
        // child process
        // 直接终止子进程，以 SIGINT 信号方式终止
        exit(2);
    }
    return 0;
}
