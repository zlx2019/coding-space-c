#include <stdio.h>
#include <pthread.h>

/**
 * 第一种死锁的形成(锁不可重入)
 */

// 初始化锁
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main() {

    // 获取锁
    pthread_mutex_lock(&lock);

    // 再次获取锁，由于锁已经被自己拿到了，无法再次获取
    pthread_mutex_lock(&lock);
    return 0;
}
