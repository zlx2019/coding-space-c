#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


/**
 * 通过kill函数，由父进程向子进程发送信号，使子进程终止;
 */

int main() {
    // 创建子进程
    pid_t pid = fork();
    int second = 0;
    switch (pid) {
        case -1:
            perror("fork process");
            exit(1);
        case 0:
            // child
            // 子进程恶意循环阻塞
            while (1){
                printf("child sleep %d.\n",++second);
                sleep(1);
            }
        default:
            // parent
            // 父进程，10秒后向子进程发送信号，终止子进程
            sleep(10);
             if(kill(pid,SIGSEGV) == -1){
                 perror("kill child");
                 exit(1);
             };
            printf("parent kill child complete.\n");
            // 等待子进程结束，获取子进程终止状态信息
            int child_status;
            wait(&child_status);
            // 判断子进程是正常终止，还是由于信号而终止
            if (WIFEXITED(child_status)){
                // 正常终止
                printf("child normal terminate.");
            } else if(WIFSIGNALED(child_status)){
                // 由于信号终止
                printf("child terminate by signal; signal is: %d", WTERMSIG(child_status));
            }
    }
    return 0;
}
