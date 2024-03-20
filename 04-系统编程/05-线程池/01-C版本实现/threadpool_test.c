#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "threadpool.h"


void say(void* arg){
    int no = *(int*)arg;
    printf("say hello, no: %d threadId: %lu \n",no, pthread_self());
    sleep(1);
}

/**
 * 测试线程池的使用
 */
int main(void) {
    // 初始化线程池
    ThreadPool* pool = createThreadPool(8,16,10);

    // 添加批量任务
    for (int i = 0; i < 100; i++) {
        int* num = malloc(sizeof(int));
        *num = i;
        addTask(pool, say, num);
    }
    sleep(10);
    shutdown(pool);
    return 0;
}
