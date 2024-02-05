#include <stdio.h>
#include <signal.h>

/**
 * 上一节我们讲了如何通过 sigprocmask 函数操作进程的信号屏蔽字，也就是信号的阻塞与释放
 * 接下来我们介绍如何获取当前进程处于未决状态的信号.
 *
 * 通过 sigpending 函数可以读取当前进程的未决信号集，通过参数指针传出:
 *
 *  int sigpending(sigset_t* set);
 *      - set: 信号集地址
 * 成功返回0，错误返回-1;
 *
 */

int main() {
    sigset_t set;
    sigpending(&set);
    return 0;
}
