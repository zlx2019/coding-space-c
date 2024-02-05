#include <stdio.h>
#include <sys/shm.h>

/**
 * 通过 shmget 函数创建出共享内存段
 */

int main() {
    // 1.创建标识
    key_t key = ftok("HELP.md",9501);
    printf("create key: 0x%x \n",key);
    // 2.创建共享内存段
    int sm_id = shmget(key,20,IPC_CREAT | 0666);
    printf("create shared memory id: %d\n",sm_id);
    return 0;
}
