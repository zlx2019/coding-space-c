#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 创建子进程
    pid_t  pid = fork();
    if(pid == 0){
        sleep(3);
        // 子进程执行其他程序
        execlp("ls", "ls", "-a", NULL);
    }

    // 父进程等待子进程结束
    waitpid(pid,NULL,0);
    printf("子进程执行结束.\n");
    return 0;
}
