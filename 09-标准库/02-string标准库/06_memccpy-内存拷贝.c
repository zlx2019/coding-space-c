#include <string.h>
#include <stdio.h>

/**
 * 函数声明:
 *      void* memccpy(void* dest, void* src, int c, size_t n);
 *  - dest: 目标内存首地址
 *  - src:  被拷贝的内存首地址
 *  - n:    拷贝的字节数
 *  - c:    拷贝可以提前结束的标识符
 *
 * memccpy会从 src 地址拷贝 n 个字节到 dest 地址，如果遇到 c 则立刻停止拷贝，返回dest首地址;
 */

int main(int argc, char *argv[]) {
    char buf1[]  = {'a','b','c','d','e','f'};
    char buf2[10] = {};
    memccpy(buf2,buf1,'d',10);
    printf("%s",buf2); // abcd
    return 0;
}
