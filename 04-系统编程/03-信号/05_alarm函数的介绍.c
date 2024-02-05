#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


/**
 * alarm 函数负责让内核在延迟一定的时间后，给当前进程发送 SIGALRM 信号;
 * 进程接收到 SIGALRM 信号后会终止进程;
 */

int main(){
    // 10后向当前进程发送 SIGALRM 信号，使进程终止.
    if(alarm(10) != 0){
        perror("alarm 10 sec failed");
        exit(1);
    }

    // 死循环执行
    int counter = 0;
    while (1){
        printf("sleep %d.\n",++counter);
        sleep(1);
    }
}