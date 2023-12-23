#include <stdio.h>
#include <stdlib.h>

/**
 * 关于malloc和free还有一些特殊情况:
 *  - malloc(0)这种调用也是合法的，也会返回一个非NULL的指针，这个指针也可以传给free函数释放，但是不能通过这个指针访问内存。
 *  - 释放一个野指针是不合法的，例如先调用malloc返回一个指针p，然后连着调用两次free(p);，则后一次调用会产生运行时错误。
 *  - free(NULL)也是合法的，不做任何事情，所以当内存释放后，最好将指针设置为NULL。
 */

int main(int argc, char *argv[]) {
    // 申请内存
    void* empty = malloc(0);

    // 释放内存
    free(empty); // 第一次释放没问题

    // double free,释放未分配的指针，程序崩溃
    // free(empty); //  pointer being freed was not allocated

    empty = NULL;
    free(empty); // 什么事都不会做

    return 0;
}
