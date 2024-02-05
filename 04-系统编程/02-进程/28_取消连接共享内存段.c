#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

/**
 * 通过 shmat 函数连接共享内存段
 */

int main() {
    // 1.创建标识
    key_t key = ftok("HELP.md",9501);
    printf("create key: 0x%x \n",key);
    // 2.创建共享内存段
    int sm_id = shmget(key,20,IPC_CREAT | 0666);
    printf("create shared memory id: %d\n",sm_id);

    // 3. 挂载共享内存
    void* shm_ptr = shmat(sm_id,NULL,0);
    if(shm_ptr == (void*)-1){
        perror("shmat failed");
        exit(1);
    }
    printf("shm address: %p\n",shm_ptr); // shm address: 0x10253c000

    // 写入共享内存
    char msg[] = "Hello world";
    strncpy(shm_ptr,msg, sizeof(msg));

    // 读取共享内存
    char line[sizeof(msg)];
    strncpy(line,shm_ptr, sizeof(line));
    printf("shm content: %s \n",line); // Hello world

    // 4. 取消挂载共享内存
    shmdt(shm_ptr);
    return 0;
}