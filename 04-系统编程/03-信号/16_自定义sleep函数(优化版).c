#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/**
 * 自定义 sleep 函数(优化版)
 *  函数执行流程:
 *   1. 设置 SIGALRM 信号的处理函数;
 *   2. 让当前进程屏蔽 SIGALRM 信号;
 *   3. 通知内核在n秒后向当前进程发送 SIGALRM 信号;
 *   4. 使用原子操作，同时执行下面两条操作
 *     4.1 让当前进程解除 SIGALRM 信号的屏蔽;
 *     4.2 阻塞挂起当前进程，等待信号的递达;
 *   5. 接收到 SIGALRM 信号，进程被唤醒, 查看有没有剩余的可阻塞时长
 *   6. 设置 SIGALRM 信号的处理函数置为默认处理函数;
 *   7. 恢复进程的信号屏蔽字信息;
 */


// 自定义 SIGALRM 信号处理函数
// 该函数什么都不做，只用来将进程从内核态切换回用户态
void wake_handler(int sig_no){
}

// 使当前进程阻塞挂起 second 秒
// @param second 要阻塞的时长
unsigned int my_sleep(unsigned int second){
    // 1. 对 SIGALRM 信号绑定处理函数
    struct sigaction new_act, old_act;
    new_act.sa_handler = wake_handler;
    new_act.sa_flags = 0;
    sigemptyset(&new_act.sa_mask);
    if (sigaction(SIGALRM,&new_act,&old_act) == -1){
        perror("bind SIGALRM");
        exit(1);
    }

    // 2. 屏蔽 SIGALRM 信号
    sigset_t mask, mask_back, sus_make;
    sigemptyset(&mask);
    sigaddset(&mask,SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &mask_back);

    // 3. 通知内核，在 second 秒后向该进程发送 SIGALRM 信号
    alarm(second);

    // 4. 解除 SIGALRM 信号的屏蔽 && 阻塞进程等待有信号递达(注意这是一个原子操作)
    sus_make = mask_back;
    sigdelset(&sus_make, SIGALRM);
    sigsuspend(&sus_make);

    // 5. 判断进程阻塞时长是否达到了 second 秒，
    // 如果没有则返回剩余的时长
    // alarm(0) 表示取消之前的定时时长，返回剩余的时间.
    unsigned int remain = alarm(0);

    // 6. 将SIGALRM 信号的处理行为还原为默认的
    sigaction(SIGALRM,&old_act,NULL);

    // 7. 恢复进程屏蔽字
    sigprocmask(SIG_SETMASK, &mask_back, NULL);
    return remain;
}

int main(void) {
    int n = 10;
    while (n){
        printf("%d\n",n);
        n--;
        my_sleep(1);
    }
    return 0;
}
