#include <stdio.h>
#include <stdlib.h>

/**
 * void* calloc(size_t count, size_t size);
 *  - size: 每个元素的大小;
 *  - count: 要分配的元素数量;
 *
 * 分配count个元素的内存空间，每个元素占size字节，并且calloc负责把这块内存空间用字节0填充，而malloc并不负责把分配的内存空间清零。
 * 共分配 size * count 个字节
 */

int main(int argc, char *argv[]) {

    // 释放内存
    int* array = calloc(4, sizeof(int));

    *array = 10;
    *(array + 1) = 20;
    *(array + 2) = 30;
    *(array + 3) = 40;

    for (int i = 0; i < 4; i++){
        printf("%d,",*(array + i));
    }

    // 输出:
    // 10,20,30,40,

    // 释放内存
    free(array);
    return 0;
}
