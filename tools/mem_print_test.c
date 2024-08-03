#include "mem_print.h"
/**
 * 工具函数测试
 * 编译命令: gcc -Wall -o test -I./  mem_print_test.c mem_print.c
 *      -Wall: 输出所有警告信息
 *      -o: 设置可执行程序名称
 *      -I: 指定头文件检索路径，由于使用的<>引入的自定义头文件，所以需要指定;
 */

int main(){
    printf("%d \n",is_big_endian());
    return 0;
}