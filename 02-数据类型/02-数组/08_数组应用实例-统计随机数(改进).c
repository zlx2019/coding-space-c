#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * 首先生成一列0～9的随机数保存在数组中，然后统计其中每个数字出现的次数并打印，检查这些数字的随机性如何。
 *
 * C标准库中生成伪随机数的是rand函数，使用这个函数需要包含头文件stdlib.h，
 * 它没有参数，返回值是一个介于0和RAND_MAX之间的接近均匀分布的整数。
 */

// 生成的随机数数量
#define N 20

// 定义一个数组，用于存放生成的所有随机数
int container[N];

/**
 * 生成N个随机数(0 ~ max)，放入container数组;
 */
void gen_random(int max){
    for (int i = 0; i < N; i++) {
        int num = rand() % max;
        container[i] = num;
    }
}

/**
 * 获取 value 参数的值，在container中存在多少个
 */
int how_many(int value){
    int count = 0, i = 0;
    for(;i < N; i++){
        if(container[i] == value){
            count++;
        }
    }
    return count;
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    int i, histogram[10] = {0};
    // 生成数字 0 ~ 10
    gen_random(10);

    // 用数组记录，每个数字所出现的次数
    for (int i = 0; i < N; i++) {
        histogram[container[i]]++;
    }
    for (int j = 0; j < 10; j++) {
        printf("%d 生成的次数: %d \n",j,histogram[j]);
    }
    printf("\n");

    /// 以立方图的形式展示，每个数字生成了几次，就输出多少个*符号
    // 1. 输出索引
    for (int j = 0; j < 10; j++) {
        printf("%d ",j);
    }
    printf("\n");
    // 2. 找出最大值,输出*最多的那一列
    int max_val = 0;
    for (int j = 0; j < 10; j++){
        if (histogram[j] > max_val){
            max_val = histogram[j];
        }
    }
    // 3. 根据最大值，遍历*
    for (int j = 1; j <= max_val ; j++) {
        for (int k = 0; k < 10; k++) {
            if(histogram[k] >= j){
                printf("* ");
            }else{
                printf("  ");
            }
        }
        printf("\n");
    }
    return 0;
}
