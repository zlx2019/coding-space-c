#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * 方式三: 被其他线程通过 pthread_cancel 函数终止;
 */

// 线程函数
void* task(void* arg){
    printf("new thread running task \n");
    // 手动进入死循环
    int sec = 0;
    while (1){
        printf("new thread sleep %d sec. \n", ++sec);
        sleep(1);
    }
}

int main() {
    // 线程id
    pthread_t threadId;
    // 创建线程
    int errno;
    if((errno = pthread_create(&threadId, NULL, task, NULL))){
        fprintf(stderr,"create thread failed: %s \n", strerror(errno));
        exit(errno);
    }

    sleep(3);
    // TODO 中断线程
    // 三秒后，主线程通过 pthread_cancel 主动打断新线程的运行
    if ((errno = pthread_cancel(threadId))){
        fprintf(stderr,"cancel thread failed: %s \n", strerror(errno));
        exit(errno);
    };

    // TODO  等待线程结束，并且获取线程执行的函数返回值
    void* res;
    if((errno = pthread_join(threadId, &res))){
        fprintf(stderr,"join thread failed: %s \n", strerror(errno));
        exit(errno);
    }
    if (res == PTHREAD_CANCELED){
        // 线程是被其他线程取消的
        printf("new thread is cancel \n");
    }
    return 0;
}

// new thread running task
// new thread sleep 1 sec.
// new thread sleep 2 sec.
// new thread sleep 3 sec.
// new thread sleep 4 sec.
// new thread is cancel