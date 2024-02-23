#include <stdio.h>
#include <pthread.h>
#include <string.h>

/**
 * 使用 Mutex 实现 多线程的同步
 */

// 共享资源
int counter = 0;
/// 定义并且初始化互斥锁
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

// 对 counter 自增,循环5000次
void* add(void *arg) {
    pthread_mutex_init(&counter_lock, NULL);
    for (int i = 0; i < 5000; i++) {
        /// 加锁
        int errno = pthread_mutex_lock(&counter_lock);
        if(errno != 0){
            // 加锁失败
            fprintf(stderr,"lock failed: %s \n",strerror(errno));
        }

        /// 操作共享数据
        counter++;

        /// 解锁
        errno = pthread_mutex_unlock(&counter_lock);
        if(errno != 0){
            // 解锁失败
            fprintf(stderr,"unlock failed: %s \n",strerror(errno));
        }
    }
    return NULL;
}

int main() {
    // 启用 10 个线程测试
    pthread_t tids[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&tids[i], NULL, add, NULL);
    }

    // 等待10个线程全部结束
    for (int i = 0; i < 10; i++) {
        pthread_join(tids[i], NULL);
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&counter_lock);

    // 输出总结果
    printf("counter: %d \n", counter);
    return 0;
}

// 如果程序正确，那么结果应该总是 50000