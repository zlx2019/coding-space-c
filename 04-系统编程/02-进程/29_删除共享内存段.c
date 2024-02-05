#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>

/**
 * 
 */

int main() {
    // 1. 获取共享内存的键值
    key_t key = ftok("HELP.md",9501);

    // 2. 获取共享内存的标识符
    int shm_id = shmget(key,0,IPC_CREAT | 0666);

    // 3. 根据共享内存标识符 删除共享内存段
    if(shmctl(shm_id,IPC_RMID,NULL) == -1){
        perror("remove shm failed");
        exit(1);
    }
    return 0;
}
