#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/**
 * 线程要执行的任务函数
 * @param msg
 */
void* task(void* arg){
    printf("%s \n", arg);
    return NULL;
}

int main() {
    pthread_t tid; // 用于保存线程的id

    // 创建线程
    int res_no = pthread_create(&tid, NULL, task, "Hello thread.");;
    if(res_no){
        // 错误处理
        perror("thread create");
        exit(1);
    }
    sleep(1);
    printf("创建的子线程ID: (%p)\n", tid);
    return 0;
}
