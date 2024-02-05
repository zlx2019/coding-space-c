#include <stdio.h>

/**
 * 通过 sigprocmask 函数读取或者修改进程的信号屏蔽字。
 *
 *   int sigprocmask(int how, const sigset_t* set, sigset_t* old_set);
 *      - how: 动作选项，可选值有如下:
 *          - SIG_BLOCK: 阻塞(屏蔽) set 信号集中指定的所有信号.
 *          - SIG_UNBLOCK: 解除阻塞(屏蔽你) set 信号集中指定的所有信号.
 *          - SIG_SETMASK: 直接将 set信号集作为信号屏蔽字，替换掉当前进程的信号屏蔽字信息，set中存在的信号会被阻塞.
 *
 *      - set: 要处理的信号集
 *      - old_set: 将处理之前的信号屏蔽字信息，保存到该变量中作为备份.
 *  成功返回0，错误返回-1;
 * 注意:
 * 1. 如果调用sigprocmask解除了若干个已经处于未决状态的信号的阻塞，则在该函数返回前至少有一个信号以递达.
 * 2. SIGKILL 和 SIGSTOP 信号是无法阻塞的.
 */

int main() {
    return 0;
}
