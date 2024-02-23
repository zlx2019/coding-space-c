#include <pthread.h>
#include <stdio.h>

/**
 * 多线程程序可能会存在的问题
 */

// 总数
int counter = 0;

// 对 counter 自增,循环5000次
void* add(void *arg) {
  for (int i = 0; i < 5000; i++) {
    counter++;
  }
  return NULL;
}

int main() {
    // 启用两个线程，分别执行 add 函数
    pthread_t t1, t2;
    pthread_create(&t1, NULL, add, NULL);
    pthread_create(&t2, NULL, add, NULL);

    // 等待两个线程结束
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    // 输出结果值
    printf("counter: %d \n", counter);
    return 0;
}
