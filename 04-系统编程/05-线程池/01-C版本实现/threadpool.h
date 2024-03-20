/**
 * C语言线程池 - 头文件.
 */
#ifndef LEARNS_THREAD_POOL_H
#define LEARNS_THREAD_POOL_H

#include <pthread.h>

// 每次扩容和销毁的线程数量
#define LIMIT 2


/// =================== Task =========================
/** 任务结构体，提交给线程池的任务 */
typedef struct Task {
    void (*func)(void* arg); // 任务具体要执行的函数地址
    void* arg;               // 任务函数的参数
    int id; // 任务ID
} Task;
/// ====================================================


/// ================= Task Queue =========================
/** 任务队列的节点 */
typedef struct TaskNode{
    Task task; // 节点的任务
    struct TaskNode* next; // 下一个节点
} TaskNode;

// 清除节点内存
void freeNode(TaskNode* node);

/** 任务队列 */
typedef struct {
    TaskNode* head; // 队列的头部
    TaskNode* tail; // 队列的尾部
    int size;       // 当前队列中的任务数量
    int capacity;   // 队列可容纳的任务数量
} TaskQueue;

// 创建并且初始化一个任务队列
TaskQueue* createTaskQueue(int capacity);
// 释放一个任务队列的内存
void freeTaskQueue(TaskQueue* queue);
//向任务队列中添加一个任务节点
void pushNode(TaskQueue* queue, TaskNode* node);
// 从任务队列中获取一个任务节点
TaskNode* popNode(TaskQueue* queue);


/// =======================================================



/// ================= Thread Pool =========================
/** 线程池结构体*/
typedef struct ThreadPool {
    // 线程池全局锁
    pthread_mutex_t globalMutex;

    TaskQueue* tasks;        // 存放任务的队列
    // 用于阻塞和唤醒 生产者(线程池使用者) 和 消费者(工作线程) 的条件变量
    pthread_cond_t producer; // 用于任务队列已满，阻塞任务生产者.
    int waitProducerNum;     // 处于阻塞的生产者数量.
    pthread_cond_t consumer; // 用于任务队列已空，阻塞任务消费者(工作线程).

    pthread_t leaderThread; // 管理线程
    pthread_t* workThreads; // 工作线程数组

    int minThreadNum;      // 核心线程数(线程池最小的工作线程数量)
    int maxThreadNum;       // 可以扩容到的最大工作线程数量
    int runThreadNum;       // 当前正在执行任务的线程数量
    int waitThreadNum;      // 当前处于阻塞等待任务的线程数量
    int totalThreadNum;     // 当前池中存活的线程总数(所有线程，无论处于阻塞还是运行)
    int cancelThreadNum;    // 当前需要回收的线程数量，如果为0，表示不需要回收

    pthread_mutex_t mutex;  // 局部互斥锁，主要用来修改 runThreadNum 属性时使用.
    unsigned char shutdown; // 线程池是否已关闭
} ThreadPool;

/** 创建并且初始化线程池 */
ThreadPool* createThreadPool(int minThreads, int maxThreads, int tasksMaxCap);
/** 工作线程的执行函数 */
void* workThreadHandler(void* arg);
/** 管理线程的执行函数 */
void* mgrThreadHandler(void* arg);
/** 将当前工作线程关闭，并从线程池中移除 */
void removeWorkThread(ThreadPool* pool);
/** 关闭当前工作线程 */
void closeWorkThread(pthread_t tid);
/** 关闭线程池 */
void shutdown(ThreadPool* pool);

/** 将任务添加到线程池中 */
void addTask(ThreadPool* pool, void(*func) (void*), void* arg);



/// =======================================================

#endif //LEARNS_THREAD_POOL_H
