#include <mem_print.h>
/**
 * 工具函数测试
 * 编译命令: gcc -Wall -o test -I./  mem_print_test.c mem_print.c
 *      -Wall: 输出所有警告信息
 *      -o: 设置可执行程序名称
 *      -I: 指定头文件检索路径，由于使用的<>引入的自定义头文件，所以需要指定;
 */

int main(int argc,char* argv[]){
    unsigned char i = 3;
    mem_println(&i,1); // 00000011
    return 0;
}