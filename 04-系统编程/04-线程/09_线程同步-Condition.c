#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/**
 * 通过 Mutex 和 Condition 实现一个生产者消费者模型
 */


// ============== 共享资源 ================
int cakeCount = 0;    // 当前蛋糕数量
int maxCakeCount = 10;// 最大蛋糕数量
int producerSum = 0; // 共生产的蛋糕数量
int consumerSum = 0; // 共消费的蛋糕数量
// ======================================

// ============= 同步工具 =================
// 互斥锁
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// 条件变量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// =======================================

/**
 * 生产者线程函数
 */
void* producerTask(void* arg){
    // 加锁
    pthread_mutex_lock(&lock);
    // 生产蛋糕
    while (1){
        if(cakeCount >= maxCakeCount){
            // 已到达最大数量，无法继续生产
            // 唤醒消费者
            pthread_cond_signal(&cond);
            if(producerSum == 20){
                // 停止生产者
                printf("生产者生成完成20个蛋糕，结束!\n");
                break;
            }
            printf("蛋糕已满，不能继续生产，唤醒消费者! \n");
            // 阻塞生产者
            pthread_cond_wait(&cond, &lock);
        }
        cakeCount++;
        producerSum++;
        printf("生产者生成了1个蛋糕，当前蛋糕数量: %d，共生产蛋糕数量: %d \n",cakeCount,producerSum);
        sleep(1);
    }
    // 解锁
    pthread_mutex_unlock(&lock);
}

/**
 * 消费者线程函数
 */
void* consumerTask(void* arg){
    // 加锁
    pthread_mutex_lock(&lock);
    // 消费蛋糕
    while (1){
        if(cakeCount <= 0){
            // 没有蛋糕了，无法继续消费
            // 唤醒生产者
            pthread_cond_signal(&cond);
            if(consumerSum == 20){
                printf("消费者共消费蛋糕数量: %d，结束!\n",consumerSum);
                break;
            }
            printf("没有蛋糕了，唤醒生产者!\n");
            // 阻塞消费者
            pthread_cond_wait(&cond,&lock);
        }
        cakeCount--;
        consumerSum++;
        printf("消费者消费了1个蛋糕，当前蛋糕数量: %d，共消费蛋糕数量: %d\n",cakeCount,consumerSum);
        sleep(1);
    }
    // 解锁
    pthread_mutex_unlock(&lock);
}

int main() {
    pthread_t producer, consumer;
    // 开启生产者
    pthread_create(&producer, NULL, producerTask, NULL);
    // 开启消费者
    pthread_create(&consumer, NULL, consumerTask, NULL);

    pthread_join(producer,NULL);
    pthread_join(consumer,NULL);
    return 0;
}
