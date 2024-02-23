#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/**
 * 自定义 sleep 函数
 */

/**
 * 自定义 SIGALRM 信号处理函数
 * 该函数什么都不做，只用来将进程从内核态切换回用户态
 */
void wake_handler(int sig_no){
    return;
}

/**
 * 使当前进程阻塞挂起 second 秒
 * @param second 要阻塞的时长
 */
unsigned int my_sleep(unsigned int second){
    // 1. 对 SIGALRM 信号进行捕捉处理
    struct sigaction new_act, old_act;
    new_act.sa_handler = wake_handler;
    new_act.sa_flags = 0;
    sigemptyset(&new_act.sa_mask);
    if (sigaction(SIGALRM,&new_act,&old_act) == -1){
        perror("bind SIGALRM");
        exit(1);
    }

    // 2. 通知内核，在 second 秒后向该进程发送 SIGALRM 信号
    alarm(second);

    // 3. 调用 pause 函数阻塞进程，等待信号
    pause();

    // 4. 判断进程阻塞时长是否达到了 second 秒，
    // 如果没有则返回剩余的时长
    // alarm(0) 表示取消之前的定时时长，返回剩余的时间.
    unsigned int remain = alarm(0);

    // 5. 将SIGALRM 信号的处理还原
    sigaction(SIGALRM,&old_act,NULL);
    return remain;
}

int main(void) {
//    int n = 10;
//    while (n){
//        printf("%d\n",n);
//        n--;
//        my_sleep(1);
//    }
    unsigned int res = my_sleep(10);
    printf("%d",res);
    return 0;
}
