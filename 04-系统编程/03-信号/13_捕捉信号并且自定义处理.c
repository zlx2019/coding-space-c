#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


/**
 * 自定义 SIGINT 信号处理函数
 * @param sig_no 触发的信号值
 */
void handler(int sig_no){
    printf("触发了 %d 信号，不会终止进程哦~\n",sig_no);
    // 执行完该函数后，会继续回到main函数栈继续执行...
}

int main() {
    // 自定义信号处理行为
    struct sigaction custom_action;
    custom_action.sa_handler = handler; // 执行处理函数地址
    custom_action.sa_flags = 0;         // 设置0
    sigemptyset(&custom_action.sa_mask);// 清空要屏蔽的信号集

    // 用于保存信号原来的处理行为信息
    struct sigaction old_action;

    // 绑定 SIGINT 信号的处理行为
    if (sigaction(SIGINT, &custom_action, &old_action) == -1){
        perror("bind SIGINT");
        exit(1);
    }
    // 阻塞程序，测试使用 Ctrl-C 是否可以终止进程.
    while (1);
}
