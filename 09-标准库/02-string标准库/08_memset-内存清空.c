#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * 在C语言中，一个局部变量或者从堆中申请的内存，在没有赋值之前，它的值是不确定的，这个时候就可以用memset来清零;
 * memset 函数可以将一段连续内存空间的每一个字节，都设置为一个指定的数值;
 *
 * void* memset(void* s,int c,size_t n);
 *  - s: 起始字节指针;
 *  - c: 要赋予的值;
 *  - n: 操作的字节数;
 * 将地址 s 后面的 n 个字节都赋值为c;(注意是每一个字节，而不是整体赋值为c),最终再将 s 指针返回;
 * 通常调用memset时传给c的值是0，把一块内存区清零。
 */

int main(int argc, char *argv[]) {
    // 申请堆内存
    int* list = malloc(sizeof(int) * 4);
    // 此时元素的值是不确定的
    printf("%d \n",list[0]); // 1064321152
    // 将 age 的所有字节，都设置为0;
    memset(list,0, sizeof(int) * 4);
    printf("%d \n",list[0]); // 0
    return 0;
}
