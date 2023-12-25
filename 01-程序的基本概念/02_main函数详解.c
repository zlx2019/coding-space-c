#include <stdio.h>

/**
 * main 函数是每个C程序的入口函数，操作系统通过调用 main 函数来运行该程序;
 * main 函数的返回值必须是 int 类型，返回0表示程序正常退出，而非0则表示异常退出;
 * 在 main 函数的参数中，可以接收操作系统运行程序时的参数:
 *  -  argc: 传递的参数数量;
 *  -  argv: 传递的参数数组;
 *  -  envs: 指向系统环境变量数组的指针;
 *  参数至少有一个，第一个参数也就是运行程序的命令
 */
int main(int argc, char* argv[], char** envs){
    // 遍历输出所有命令行参数
    for (int i = 0; i < argc; i++) {
        printf("%s \n",argv[i]);
    }

    printf("\n");

    // 遍历输出所有环境变量
    while (*envs != NULL){
        printf("%s \n",*envs);
        envs++;
    }
    return 0;
}