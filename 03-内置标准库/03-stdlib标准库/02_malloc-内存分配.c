#include <stdio.h>
#include <stdlib.h>

/**
 * 每个进程都有一个堆空间，C标准库函数malloc可以在堆空间动态分配内存，它的底层通过brk系统调用向操作系统申请内存。
 *
 * void* malloc(size_t size);
 *  - size: 要申请分配的内存大小(字节)
 * 返回申请到的内存首地址，如果分配失败(可能是没有可用的内存)，则会返回NULL;
 * malloc函数只负责分配内存，不关心这部分内存怎么使用，所以返回的是一个万能指针(void*);
 */

int main(int argc, char *argv[]) {
    // 编译器会做隐式类型转换，void ＊类型和任何指针类型之间可以相互隐式转换。
    int* i = malloc(4);
    int* l = malloc(8);

    // 申请分配一个数组
    int* nums = malloc(sizeof(int) * 10);
    if (nums == NULL){
        // 错误处理
        fprintf(stderr,"mem alloc failed.");
        exit(1);
    }
    return 0;
}
