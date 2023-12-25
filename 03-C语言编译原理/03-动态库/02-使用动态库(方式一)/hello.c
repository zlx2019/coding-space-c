#include <stdio.h>

// 声明要调用的静态库函数
extern int add(int n1, int n2);

int main(int argc, char *argv[]) {
    int sum = add(10,20);
    printf("%d\n",sum);
    return 0;
}

/**
 * 编译命令: gcc -L ./ -l kmath -o hello hello.c
 */