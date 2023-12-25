#include <stdio.h>
#include <errno.h>

/**
 * errno 是一个整型的全局变量，表示错误码，定义在errno.h头文件中;
 * 0表示执行正常结束;
 * 很多系统内置函数的错误，都会记录在这个变量中，不同的值表示不同的错误原因;
 * 如果直接对外输出errno的值，并不能很直观的看出是什么错误，比较好的办法是解析成字符串;
 *
 * 具体的值和错误原因的映射，可参考 sys/errno.h头文件中的宏定义
 *
 */

int main(int argc,char* argv[]){
    printf("%d \n",errno); // 0
    return 0;
}