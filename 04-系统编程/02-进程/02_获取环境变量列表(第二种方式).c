#include <stdio.h>

/**
 * 在之前我们通过main函数的参数获取过环境变量，接下来我们介绍另一种方式获取环境变量.
 */

int main(void) {
    // 声明一个字符串指针变量，注意名称必须为`environ`
    // 因为该变量由 libc 库中所定义和提供
    extern char** environ;

    // 变量输出环境变量
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n",environ[i]);
    }
    return 0;
}
