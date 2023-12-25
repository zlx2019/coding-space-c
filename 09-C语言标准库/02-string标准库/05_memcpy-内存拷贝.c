#include <stdio.h>
#include <string.h>

/**
 * 在上一小节中，介绍了 strcpy和strncpy函数，它们是用来拷贝以'\0'结尾的字符串的;
 * 下面介绍比较类似的 memcpy (memory copy) 内存拷贝函数;
 *
 *
 * 函数声明:
 *      void* memcpy(void* dest, const void* src, size_t n);
 *  - dest: 目标内存首地址
 *  - src:  被拷贝的内存首地址
 *  - n:    拷贝的字节数
 *
 * memcpy函数从src所指的内存地址拷贝n个字节到dest所指的内存地址，并且返回dest首地址
 * 和strncpy不同，memcpy并不是遇到'\0'就结束，而是一定会拷贝完n个字节。
 *
 * 注意: 如果src和dest所指的内存区间如果重叠则无法保证正确拷贝，因为memcpy是直接拷贝内存，
 * 推荐使用memmove函数，该函数借助于一个临时缓冲区来进行拷贝;
 *
 */

int main(int argc, char *argv[]) {
    char buf1[] = {1,2,3,4,5,6};
    char buf2[3];
    // 将 buf1的前3个字节，拷贝到bu2中
    memcpy(buf2,buf1,3);
    printf("%d,%d,%d",buf2[0],buf2[1],buf2[2]); // 1,2,3

    printf("\n");

    // 将 buf1的后3个字节，拷贝到bu3中
    char buf3[3];
    memcpy(buf3,&buf1[3],3);
    printf("%d,%d,%d",buf3[0],buf3[1],buf3[2]); // 4,5,6
    return 0;
}
