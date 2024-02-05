#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/**
 * raise函数负责向当前进程自己发送一个信号.
 *  void raise(int sig_no);
 *
 * 其实就等同于 kill(getpid(), sig_no) 这样使用。
 * 成功返回0，错误返回-1。
 */

int main() {
    raise(9);
    // 等同于
    kill(getpid(), 9);
    return 0;
}
