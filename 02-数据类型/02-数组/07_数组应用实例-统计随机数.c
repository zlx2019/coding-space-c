#include <stdio.h>
#include <stdlib.h>

/**
 * 首先生成一列0～9的随机数保存在数组中，然后统计其中每个数字出现的次数并打印，检查这些数字的随机性如何。
 *
 * C标准库中生成伪随机数的是rand函数，使用这个函数需要包含头文件stdlib.h，
 * 它没有参数，返回值是一个介于0和RAND_MAX之间的接近均匀分布的整数。
 */

// 生成的随机数数量
#define N 10000

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
    // 生成数字 0 ~ 10
    gen_random(10);
    printf("0~9每个数字生成的次数: \n");

    // 获取0~9每个数字生成的次数
    for (int i = 0; i < 10; i++) {
        printf("%d 生成了 %d 个;\n",i, how_many(i));
    }
    return 0;
}

//0~9每个数字生成的次数:
//0 生成了 1001 个;
//1 生成了 1000 个;
//2 生成了 960 个;
//3 生成了 1022 个;
//4 生成了 1008 个;
//5 生成了 999 个;
//6 生成了 995 个;
//7 生成了 1019 个;
//8 生成了 956 个;
//9 生成了 1040 个;