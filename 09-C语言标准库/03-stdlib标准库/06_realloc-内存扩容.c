#include <stdio.h>
#include <stdlib.h>

/**
 * 当我们使用malloc或者calloc分配的内存空间不足时，想要对内存进行扩容时，有两种方法:
 * 第一种是重新通过malloc分配出更大的内存空间，然后使用memmove函数将原内存空间的数据拷贝到新的内存空间中，然后调用free，释放原内存空间。
 *
 * 第二种就是通过 realloc函数，该函数的实现原理其实就是第一种方法，但是简化并且做了部分优化;
 *  函数声明:
 *      void* realloc(void* ptr, size_t size);
 *          - ptr: 原内存首地址
 *          - size: 新内存的大小
 *      返回新内存的首地址，并且释放原内存空间;
 *      如果size比原来小，则前size个字节不变，后面的数据被截断；如果size比原来大，则原来的数据全部保留，后面长出来的一块内存空间未初始化。
 *
 */

int main(int argc, char *argv[]) {
    char* array = malloc(5);
    char* newArray = realloc(array,10);

    free(array);
    free(newArray);
    return 0;
}
