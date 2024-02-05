#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/**
 * 当我们对空指针 NULL 进行操作时，会产生 SIGSEGV 信号，那么如果把该信号屏蔽掉呢?
 *
 */

int main() {
    // 让程序屏蔽 SIGSEGV 信号
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGSEGV);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 通过 raise 函数给自己发送 SIGSEGV 信号
    raise(SIGSEGV);
    // 程序运行后，就会卡死在这里，直到解除掉 SIGSEGV 信号的阻塞.

    sleep(3);
    // 解除信号阻塞
    sigprocmask(SIG_UNBLOCK,&set, NULL);
    return 0;
}
