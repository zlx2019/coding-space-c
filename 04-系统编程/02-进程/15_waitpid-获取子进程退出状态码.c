#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 创建子进程
    pid_t  pid = fork();
    if(pid == 0){
        // 子进程执行其他程序
//        execlp("ls", "ls", "-a", NULL);
        exit(3);
    }
    int status;
    // 父进程等待子进程结束
    waitpid(pid,&status,0);
    printf("子进程执行结束.\n");

    if (WIFEXITED(status)){// 判断程序是否正常结束，通过exit终止(无论值是多少)，如果是则返回非0
        // 获取exit的状态值
        int code = WEXITSTATUS(status);
        printf("子进程正常退出，状态码为: %d \n",code);
    }else if(WIFSIGNALED(status)){// 如果子进程是因为信号而结束则此宏值为真
        // 获取终止的信号
        int sig = WTERMSIG(status);
        printf("子进程因信号退出，信号为: %d \n",sig);
    }
    return 0;
}
