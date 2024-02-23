#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

/**
 * 让主线程或子线程都输出 进程的id 和 线程的id
 */
void printId(const char*);
void printId(const char* thread_name){
    pid_t pid = getpid();
    // 获取线程id
    pthread_t tid = pthread_self();
    printf("%s, 我的进程ID: %d, 我的线程ID: %u \n", thread_name,pid,tid);
}

/**
 * 线程任务函数
 */
void* childThreadTask(void* arg){
    printId("child thread.");
}


int main() {
    pthread_t t;
    // 创建线程，执行childThreadTask函数
    int errno = pthread_create(&t,NULL,childThreadTask,NULL);
    if(errno){
        printf("create thread: %s \n",strerror(errno));
        exit(1);
    }
    printId("main thread.");
    // 主线程睡眠1s，防止子线程未创建完成就结束进程.
    sleep(1);
    return 0;
}

/// main thread., 我的进程ID: 14427, 我的线程ID: 4100595968
/// child thread., 我的进程ID: 14427, 我的线程ID: 1831022592