#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/**
 * 在上一节案例中，通过信号的屏蔽实现了进程不能通过 Ctrl-C 来终止进程
 * 接下来我们来讲述 如何恢复信号的屏蔽.
 *
 * 案例: 屏蔽 SIGINT 信号10秒，在这10秒内无论产生多少个 SIGINT 信号都不会处理，
 * 10秒后，恢复 SIGINT 信号的屏蔽，这时信号才会被递达处理.
 */

int main(void) {
    // 定义两个信号集，一个用于添加要屏蔽的信号，一个用于接收进程屏蔽之前的原信号集信息
    sigset_t set, old_set;
    // 将要屏蔽的信号 添加至信号集
    sigaddset(&set,SIGINT);

    // 将要阻塞的信号集，增加到进程的信号屏蔽字，将原来的信号屏蔽字临时保存到 old_set 变量中保存
    sigprocmask(SIG_BLOCK, &set, &old_set);

    // 进程睡眠10s
    int sec = 10;
    while (sec > 0){
        sleep(1);
        printf("process sleep count down: %d.\n",sec);
        sec--;
    }

    // 恢复进程的信号屏蔽字
    sigprocmask(SIG_SETMASK, &old_set, NULL);
    printf("reset signal\n");

    // 再睡眠10s，然后结束
    // TODO 如果此时进程中没有 处于Pending 的 SIGINT 信号才会睡眠
    //  如果有SIGINT信号存在，则会直接处理该信号，从而终止进程.
    sec = 10;
    while (sec > 0){
        sleep(1);
        printf("process sleep count down: %d.\n",sec);
        sec--;
    }
    return 0;
}
