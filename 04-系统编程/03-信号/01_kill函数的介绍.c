#include <stdio.h>
#include <signal.h>

/**
 * kill 函数可以让当前进程向其他进程内发送指定的信号;
 *
 * int kill(pid_t pid, int sig_no);
 *  - sig_no: 要发送的信号值;
 *  - pid: 要发送的进程id，取值有如下几种:
 *      1. 如果pid > 0，则表示信号只发送pid进程自己;
 *      2. 如果pid = 0，则表示信号发送给当前进程组内的所有进程;
 *      3. 如果pid = -1,则表示信号发送给当前进程具有管理权限的所有进程;
 *      4. 如果pid < -1, 则表示是一个进程组的id,信号发送给组内的所有进程;
 *
 * 执行成功返回0，错误返回-1;
 */

int main(int argc, char *argv[]) {
    kill(1,1);
    return 0;
}
