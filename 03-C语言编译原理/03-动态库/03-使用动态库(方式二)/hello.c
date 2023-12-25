#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

#define LIB "./libkmath.so"

int main(int argc, char *argv[]) {

    // 加载动态链接库
    // 参数为: 要加载的动态库路径，打开模式
    void* math_dl = dlopen(LIB,RTLD_LAZY);
    if(math_dl == NULL){
        perror("load kmath lib failed:");
        exit(1);
    }

    // 获取动态库的函数地址
    int (*add)(int,int) = dlsym(math_dl,"add");
    // 调用函数
    int sum = add(100,200);
    printf("%d \n",sum);

    // 关闭动态链接库
    dlclose(math_dl);
    return 0;
}


//