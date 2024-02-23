#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
/**
 * 方式二: 线程通过 pthread_exit方式终止;
 */

// 线程函数
void* task(void* arg){
    printf("new thread running task \n");
    printf("task arg: %d \n",arg);

    // TODO 通过 pthread_exit 终止线程，并且传出返回值指针
    pthread_exit((void*) 200);
}

int main() {
    // 线程id
    pthread_t threadId;
    // 创建线程
    int errno = pthread_create(&threadId, NULL, task, (void*)100);
    if(errno){
        fprintf(stderr,"create thread failed: %s \n", strerror(errno));
        exit(errno);
    }

    // 等待线程结束，并且获取线程执行的函数返回值
    void* res;
    errno = pthread_join(threadId, &res);
    if(errno){
        fprintf(stderr,"join thread failed: %s \n", strerror(errno));
        exit(errno);
    }
    printf("new thread stop res: %d \n", res);
    return 0;
}

// new thread running task
// task arg: 100
// new thread stop res: 200
