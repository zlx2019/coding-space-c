#include <stdio.h>
#include <string.h>

/**
 * memmove 虽然叫做 move(移动)，但其实也是拷贝操作，但是它和memcpy有所不同，
 * memmove可以对地址有重叠的部分进行安全的内存拷贝，如数组元素的移动；
 *
 * 函数声明:
 *      void* memmove(void* dest,const void* src, size_t n);
 *  - dest: 目标内存首地址
 *  - src:  被拷贝的内存首地址
 *  - n:    拷贝的字节数
 * 先从src的n个字节拷贝到一个临时缓冲区，拷贝完成后再将缓冲区中的内存拷贝到dest地址;
 */

int main(int argc, char *argv[]) {

    int arr[2] = {[1] = 10};
    printf("arr[0]: %d \n", arr[0]); // 0
    printf("arr[1]: %d \n", arr[1]); // 10

    // 通过memmove 将arr[1]的值 拷贝给arr[0]
    memmove(&arr[0],&arr[1], sizeof(int));

    printf("arr[0]: %d \n", arr[0]); // 10
    printf("arr[1]: %d \n", arr[1]); // 10

    return 0;
}
