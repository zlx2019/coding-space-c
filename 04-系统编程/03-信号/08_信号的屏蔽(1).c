#include <stdio.h>
#include <signal.h>

/**
 * 让进程屏蔽掉 SIGINT 信号，并且陷入死循环，运行后尝试使用 Ctrl-C 来终止进程.
 */

int main() {
    // 创建一个信号集
    sigset_t set;
    // 将要屏蔽的信号 添加至信号集
    sigaddset(&set,SIGINT);

    // 将要阻塞的信号集，增加到进程的信号屏蔽字中
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 进入死循环
    while (1);
}

// 开始运行后，通过Ctrl-C 发送 SIGINT信号
// 由于进行阻塞(屏蔽)了该信号，所以导致该信号一直不会被递达，所以进程并不会被终止.