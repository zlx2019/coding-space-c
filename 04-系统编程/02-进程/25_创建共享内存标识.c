#include <stdio.h>
#include <sys/shm.h>

/**
 * 共享内存 - ftok函数 创建键值标识
 */

int main() {
    key_t key = ftok("HELP.md",1);
    printf("%d\n",key); //
    return 0;
}
