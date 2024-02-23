#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/**
 * 使用 Semaphore 实现生产者消费者模型
 *
 * 注意: 请在Linux系统下编译运行
 *
 */

// ============== 共享资源 ================
int cakeCount = 0;    // 当前蛋糕数量
int maxCakeCount = 10;// 最大蛋糕数量
int producerSum = 0; // 共生产的蛋糕数量
int consumerSum = 0; // 共消费的蛋糕数量
// ======================================

// ============= 同步工具 =================
sem_t producerSem; // 生产者信号量
sem_t consumerSem; // 消费者信号量
pthread_mutex_t lock;  // 互斥量
// =======================================


/**
 * 生产者线程函数
 */
void* producerTask(void* arg){
    // 生产蛋糕
    while (1){
        sem_wait(&producerSem); // producerSem - 1
        pthread_mutex_lock(&lock); // lock
        cakeCount++;
        producerSum++;
        printf("生产者生成了1个蛋糕，当前蛋糕数量: %d，共生产蛋糕数量: %d\n",cakeCount,producerSum);
        sleep(1);
        pthread_mutex_unlock(&lock); // unlock
        sem_post(&consumerSem); // consumerSem + 1
        if (producerSum == 50){
            printf("生产者结束!\n");
            break;
        }
    }
}

/**
 * 消费者线程函数
 */
void* consumerTask(void* arg){
    // 消费蛋糕
    while (1){
        sem_wait(&consumerSem); // consumerSem - 1
        pthread_mutex_lock(&lock); // lock
        cakeCount--;
        consumerSum++;
        printf("消费者消费了1个蛋糕，当前蛋糕数量: %d，共消费蛋糕数量: %d \n",cakeCount,consumerSum);
        sleep(1);
        pthread_mutex_unlock(&lock); // unlock
        sem_post(&producerSem); // producerSem + 1
        if (consumerSum == 50){
            printf("消费者结束!\n");
            break;
        }
    }
}

int main() {
    // 初始化互斥量
    pthread_mutex_init(&lock,NULL);
    // 初始化信号量
    // 生产者
    sem_init(&producerSem, 0, maxCakeCount);
    sem_init(&consumerSem, 0, 0);

    // 开启线程
    pthread_t producer, consumer;
    // 开启生产者
    pthread_create(&producer, NULL, producerTask, NULL);
    // 开启消费者
    pthread_create(&consumer, NULL, consumerTask, NULL);

    // 等待线程结束
    pthread_join(producer,NULL);
    pthread_join(consumer,NULL);

    // 销毁信号量
    sem_destroy(&producerSum);
    sem_destroy(&consumerSem);

    // 销毁互斥量
    pthread_mutex_destroy(&lock);
    return 0;
}
