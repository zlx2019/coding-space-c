#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "threadpool.h"
#include "log.h"

/**
 * C语言实现的线程池.
 */

int initSync(ThreadPool* pool);


/**
 * 创建并且初始化线程池
 * @param minThreads 默认的线程数量
 * @param maxThreads 最大的线程数量
 * @param capacity   阻塞队列的容量
 */
ThreadPool* createThreadPool(int minThreads, int maxThreads, int capacity){
    /// 初始化线程池实例
    ThreadPool* pool =  malloc(sizeof(ThreadPool));
    do {
        if (pool == NULL){
            ERROR("malloc thread pool fail");
            break;
        }
        /// 初始化工作线程组，分配内存
        pool->workThreads = malloc(sizeof(pthread_t) * maxThreads);
        if (pool->workThreads == NULL){
            ERROR("malloc works thread fail");
            break;
        }
        memset(pool->workThreads, 0x00, sizeof(pthread_t) * maxThreads);
        /// 初始化基本属性
        pool->minThreadNum = minThreads;
        pool->maxThreadNum = maxThreads;
        pool->runThreadNum = 0;
        pool->waitThreadNum = 0;
        pool->waitProducerNum = 0;
        pool->cancelThreadNum = 0;
        pool->shutdown = 0;
        /// 初始化任务队列
        pool->tasks = createTaskQueue(capacity);
        if (pool->tasks == NULL){
            ERROR("malloc task list fail");
            break;
        }
        /// 初始化互斥锁相关
        if (initSync(pool)){
            ERROR("init mutex cond fail");
            break;
        }
        /// 创建管理线程
        pthread_create(&pool->leaderThread, NULL, mgrThreadHandler, pool);
        /// 创建指定数量的工作线程
        for (int i = 0; i < minThreads; i++) {
            pthread_create(&pool->workThreads[i], NULL, workThreadHandler, pool);
        }
        // 设置目前线程总数
        pool->totalThreadNum = minThreads;
        return pool;
    } while (1);
    // 初始化错误，释放已分配的堆内存
    if (pool && pool->workThreads) free(pool->workThreads);
    if (pool && pool->tasks) free(pool->tasks);
    if (pool) free(pool);
    return NULL;
}

/**
 * 工作线程的执行函数.
 * 所有的工作线程都执行该函数，在该函数内不断的从任务队列取出任务并且执行.
 * @param arg   函数参数，这里的参数为所属线程池
 *
 */
void* workThreadHandler(void* arg){
    pthread_t curId = pthread_self();
    INFO("WorkThread[%lu] is running~", curId);

    // 将参数转换为线程池实例
    ThreadPool* pool = (ThreadPool*) arg;
    // 工作线程循环处理任务，不进行销毁
    while (1){
        // 对线程池进行加锁
        pthread_mutex_lock(&pool->globalMutex);
        /// 线程池未关闭，并且任务队列为空
        while (pool->tasks->size == 0 && !pool->shutdown){
            // 当前任务队列为空，没有可执行任务，阻塞当前工作线程，等待被唤醒
            pool->waitThreadNum++;

            pthread_cond_wait(&pool->consumer, &pool->globalMutex);
            // Block...
            // 线程被唤醒,判断是什么原因被唤醒..
            pool->waitThreadNum--;
            // 由于工作线程过多，导致需要销毁而唤醒
            if (pool->cancelThreadNum > 0){
                pool->cancelThreadNum--;
                pool->totalThreadNum--;
                // 释放全局锁
                pthread_mutex_unlock(&pool->globalMutex);
                // 从线程组中移除，并且停止当前线程
                removeWorkThread(pool);
            }
        }
        // 由于线程池被关闭所以被唤醒
        if (pool->shutdown){
            pool->totalThreadNum--;
            // 解锁
            pthread_mutex_unlock(&pool->globalMutex);
            // 关闭线程
            closeWorkThread(curId);
        }
        /// 被生成者唤醒，有任务需要处理
        // 从任务队列中取出一个任务
        TaskNode* taskNode = popNode(pool->tasks);
        INFO("WorkThread[%lu] get task[%d]~", curId, taskNode->task.id);
        // 如果有处于阻塞的任务投放者(生产者)，就唤醒它
        if (pool->waitProducerNum > 0){
            pthread_cond_signal(&pool->producer);
        }
        pthread_mutex_unlock(&pool->globalMutex);// 解锁
        // 拿到了任务，线程要执行，将运行的线程数量+1
        pthread_mutex_lock(&pool->mutex);
        pool->runThreadNum++;
        pthread_mutex_unlock(&pool->mutex);

        /// 执行任务
        taskNode->task.func(taskNode->task.arg);
        INFO("WorkThread[%lu] do task[%d] complete~", curId, taskNode->task.id);

        /// 任务执行完毕，将运行的线程数量-1
        pthread_mutex_lock(&pool->mutex);
        pool->runThreadNum--;
        pthread_mutex_unlock(&pool->mutex);
        // 回收该任务节点内存
        freeNode(taskNode);
        // 执行任务过程中线程池被关闭,结束当前工作线程
        if (pool->shutdown){
            closeWorkThread(curId);
        }
        // 继续处理其他任务...
    }
    return NULL;
}



/**
 * 线程池的管理线程执行函数
 * 每隔一段时间，检测任务的数量和工作线程的数量，适当的调整比例.
 *
 * @param arg 线程池实例
 */
void* mgrThreadHandler(void* arg){
    ThreadPool* pool = (ThreadPool*) arg;
    int maxThreadNum = pool->maxThreadNum;  // 可以扩容到的最大工作线程数量
    int minThreadNum = pool->minThreadNum;  // 线程池默认的工作线程数量

    while (!pool->shutdown){
        sleep(1);
        // 获取当前线程池的一些状态
        pthread_mutex_lock(&pool->globalMutex);
        int taskSize = pool->tasks->size;       // 当前任务队列中待执行的任务数量
        int totalThreads = pool->totalThreadNum; // 当前所有的线程数量
        int runThreads = pool->runThreadNum;    // 当前处于运行状态的线程数量
        pthread_mutex_unlock(&pool->globalMutex);


        /// 判断是否需要扩容工作线程
        /// 线程扩容规则:
        ///     当前所有线程都在执行任务 && 当前线程数 < 待执行的任务数量 && 当前线程总数 < 最大线程数
        if (runThreads == totalThreads && totalThreads < taskSize && totalThreads < pool->maxThreadNum){
            // 计算当前可以扩容的线程数量:  (最大线程数 - 当前线程总数) < LIMIT ? (最大线程数 - 当前线程总数) : LIMIT
            int incrLimit = maxThreadNum - totalThreads < LIMIT ? maxThreadNum - totalThreads : LIMIT;
            int counter = 0;
            for (int i = 0; i < pool->maxThreadNum; i++) {
                // 在线程数组中，找到一个可以放的空位
                if (pool->workThreads[i] == 0){
                    // 扩容线程
                    pthread_create(&pool->workThreads[i], NULL, workThreadHandler, pool);
                    incrLimit++;
                    pool->totalThreadNum++;
                    if (counter >= incrLimit){
                        // 已到达扩容上限.
                        break;
                    }
                }
            }
        }

        /// 判断是否需要销毁工作线程
        /// 线程缩减规则:
        ////    当前线程总数 > 最小工作线程数量 && 处于执行任务的线程数量不足总数的一半
        if (totalThreads > pool->minThreadNum && runThreads * 2 < totalThreads){
            // 开始销毁线程，每次最多销毁两个线程.
            // 计算当前可以销毁的线程数量: (当前线程总数 - 最小线程数) < LIMIT ? (当前线程总数 - 最小线程数) : LIMIT
            int decrLimit = totalThreads - minThreadNum < LIMIT ? totalThreads - minThreadNum : LIMIT;

            // 那么如何回收一些线程呢？当任务不足时，大量线程都会在获取任务时阻塞
            // 我们只需要唤醒需要销毁的线程数量即可
            pthread_mutex_lock(&pool->globalMutex);
            pool->cancelThreadNum = decrLimit;
            pthread_mutex_unlock(&pool->globalMutex);

            // 循环唤醒n个需要销毁的线程
            for (int i = 0; i < decrLimit; i++) {
                pthread_cond_signal(&pool->consumer);
            }
        }
    }
    return NULL;
}

/**
 * 将任务添加到线程池中
 * @param pool 线程池
 * @param func 任务函数
 * @param arg  任务函数参数
 */
void addTask(ThreadPool* pool, void (*func) (void*), void* arg){
    pthread_mutex_lock(&pool->globalMutex);
    // 线程未关闭 && 队列中的任务数量 == 队列的容量
    while (!pool->shutdown && pool->tasks->size == pool->tasks->capacity){
        // 任务队列已满，阻塞当前线程，等待工作线程唤醒后再投放任务.
        pool->waitProducerNum++;
        pthread_cond_wait(&pool->producer, &pool->globalMutex);
        pool->waitProducerNum--;
    }
    // 唤醒后，如果线程池已关闭
    if (pool->shutdown){
        pthread_mutex_unlock(&pool->globalMutex);
        return;
    }
    // 构建任务节点
    TaskNode* node = (TaskNode*) malloc(sizeof(TaskNode));
    node->task.func = func;
    node->task.arg = arg;
    // 添加任务
    pushNode(pool->tasks, node);

    // 如果有处于阻塞的工作线程
    if (pool->waitThreadNum > 0){
        // 唤醒处于阻塞状态的工作线程(消费者)
        pthread_cond_signal(&pool->consumer);
    }
    // 解锁
    pthread_mutex_unlock(&pool->globalMutex);
}

/**
 * 将当前线程，从工作线程组中移除
 */
void removeWorkThread(ThreadPool* pool){
    // 获取线程id
    pthread_t tid = pthread_self();
    // 从工作线程组中移除
    for (int i = 0; i < pool->maxThreadNum; i++) {
        if (pool->workThreads[i] == tid){
            pool->workThreads[i] = 0;
            break;
        }
    }
    // 停止线程
    closeWorkThread(tid);
}

/**
 * 停止一个线程运行.
 */
void closeWorkThread(pthread_t tid){
    INFO("WorkThread[%lu] closed.", tid);
    pthread_exit(tid);
}

/**
 * 向任务队列中添加一个任务节点
 * @param queue 任务队列
 * @param task  任务
 */
void pushNode(TaskQueue* queue, TaskNode* node){
    TaskNode* tail = queue->tail;
    if (tail == NULL){
        // 当前队列还没有任务
        queue->head->next = node;
        queue->tail = node;
    } else{
        tail->next = node;
        queue->tail = node;
    }
    queue->size++;
}

/**
 * 从队列中获取一个任务节点.
 */
TaskNode* popNode(TaskQueue* queue){
    TaskNode* node = queue->head->next;
    queue->head->next = node->next;
    node->next = NULL;
    queue->size--;
    if (node == queue->tail){
        queue->tail = NULL;
    }
    return node;
}

/**
 * 回收节点任务的内存
 * @param node 任务节点
 */
void freeNode(TaskNode* node){
    if (node->task.arg != NULL){
        // 释放任务函数参数的内存
        free(node->task.arg);
    }
    // 释放节点内存
    free(node);
}

// 初始化互斥体
int initSync(ThreadPool* pool){
    if (pthread_mutex_init(&pool->globalMutex, NULL) != 0 ||
        pthread_mutex_init(&pool->mutex, NULL) != 0 ||
        pthread_cond_init(&pool->consumer, NULL) != 0 ||
        pthread_cond_init(&pool->producer, NULL) != 0)
    {
        return 1;
    }
    return 0;
}

/**
 * 关闭线程池
 * @param pool
 */
void shutdown(ThreadPool* pool){
    if (pool == NULL) return;
    // 加全局锁
    pthread_mutex_lock(&pool->globalMutex);
    pool->shutdown = 1; // 标识为关闭状态
    pthread_mutex_unlock(&pool->globalMutex);

    // 1.关闭管理线程
    pthread_join(pool->leaderThread, NULL);

    // 2.关闭工作线程
    // 唤醒当前阻塞的所有工作线程，唤醒后会自动结束.
    for (int i = pool->waitThreadNum; i > 0; i--) {
        pthread_cond_broadcast(&pool->consumer);
    }
    // 可能存在当前还在执行任务的工作线程，等待这些线程结束，实现优雅关闭.
    for (int i = 0; i < pool->maxThreadNum; i++) {
        // 遍历工作线程数组，判断哪些
        if (pool->workThreads[i] != 0){
            pthread_join(pool->workThreads[i], NULL);
        }
    }
    // 释放所分配的堆内存
    // 释放工作线程数组
    free(pool->workThreads);
    // 释放任务队列
    freeTaskQueue(pool->tasks);
    // 释放同步资源
    pthread_mutex_destroy(&pool->globalMutex);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->producer);
    pthread_cond_destroy(&pool->consumer);
    // 释放线程池
    free(pool);
}

/**
 * 创建并且初始化一个任务队列
 * @param capacity 队列的容量
 */
TaskQueue* createTaskQueue(int capacity){
    TaskQueue* tasks = (TaskQueue*) malloc(sizeof(TaskQueue));
    if (tasks == NULL){
        return NULL;
    }
    // 创建一个节点，作为链表的头结点
    TaskNode* head = malloc(sizeof(TaskNode));
    head->next = NULL;
    tasks->head = head;
    tasks->tail = NULL;
    tasks->capacity = capacity;
    tasks->size = 0;
    return tasks;
}

/**
 * 释放一个任务队列的内存
 * @param queue 要释放的任务队列
 */
void freeTaskQueue(TaskQueue* queue){
    TaskNode* head = queue->head;
    TaskNode* temp = head->next;
    while (temp != NULL){
        head->next = temp->next;
        freeNode(temp);
        // 指针后移，不停地释放下一个节点
        temp = head->next;
    }
    // 释放头节点
    free(queue->head);
    // 释放队列
    free(queue);
}
