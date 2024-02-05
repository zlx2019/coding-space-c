#include <stdio.h>
#include <stdlib.h>

/**
 * abort 函数负责向当前进程发送 SIGABRT 信号;
 *
 *   void abort(void);
 *
 *   SIGABRT 信号会引起进程终止，哪怕该信号被忽略或者被定义的处理函数处理过，进程依然会被终止，
 * 也就是说会强制进程终止，无可挽救.
 *   该函数没有返回值，因为一旦调用后进程会直接终止，就像exit函数一样.
 */
int main() {
    abort();
}
