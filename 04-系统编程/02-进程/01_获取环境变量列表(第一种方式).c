#include <stdio.h>

/**
 * 在终端运行一个可执行程序时，会将当前终端进程的环境变量，传递给可执行程序的main函数.
 * 所以我们可以通过main函数来接收父进程的环境变量信息
 */

int main(int argc, char *argv[], char** envs) {
    // 变量输出环境变量
    for (int i = 0; envs[i] != NULL; i++) {
        printf("%s\n",envs[i]);
    }
    return 0;
}
