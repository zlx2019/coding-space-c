#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

/**
 * 通过 共享内存 实现父子进程之间的通讯案例。
 * 需求: 父进程接收标准输入内容，子进程将内容输出到标准输出;
 */

int main() {
    // 1.创建共享内存
    key_t  shm_key = ftok("README.md",'X');
    int shm_id = shmget(shm_key, 32, 0666 | IPC_CREAT);
    char* shm_ptr = shmat(shm_id,NULL,0);
    // 共享内存清除内容
    bzero(shm_ptr,32);

    // 2.创建进程
    int pid = fork();
    if(pid == -1){
        perror("fork process failed");
        exit(1);
    }
    if(pid){
        // parent process
        // 3. 读取内容，写入到共享内存
        char buf[32];
        while (1){
            scanf("%s",buf);
            strncpy(shm_ptr,buf, sizeof(buf));
            if (strcmp(buf,"quit") == 0)
                // quit，表示停止接收输入
                break;
        }
        // 阻塞等待子进程结束
        wait(NULL);
    } else{
        // child process
        // 4. 读取共享内存，输出到标准输出
        while (1){
            if (strcmp(shm_ptr,"quit") == 0)
                // 子进程退出
                break;
            if (*shm_ptr){
                // 当共享内存中有值，则输出内容
                printf("child read: %s\n",shm_ptr);
                bzero(shm_ptr,32);
            }
            // TODO 防止子进程高速空转，适当的睡眠一段时间
            sleep(1);
        }
    }

    // 5. 取消挂载共享内存
    shmdt(shm_ptr);

    // 6. 删除共享内存
    shmctl(shm_id,IPC_RMID,NULL);
    return 0;
}
