#include <stdio.h>
#include <signal.h>

/**
 * sigset_t 类型表示一组信号的集合，称之为信号集，其中使用一个bit位来表示有效或者无效的状态.
 * 提供了如下函数来操作该类型.
 */

int main() {
    // 声明一个信号集
    sigset_t  set;

    // 清空信号集，将所有的bit位设置为0，表示所有的信号都设置为无效
    sigemptyset(&set);

    // 设置信号集，将所有的bit位设置为1，也就是表示所有的信号都设置为有效
    sigemptyset(&set);

    // 将指定的信号设置到信号集中，表示有效;
    sigaddset(&set,SIGINT);

    // 将指定的信号从信号集中移除,表示无效
    sigdelset(&set,SIGINT);
    // 以上函数正常返回0，错误返回-1;


    // 判断某个信号 在信号集中是否为有效
    // 是则返回1，不是返回0，出错返回-1;
    sigismember(&set,SIGSEGV);
    return 0;
}
