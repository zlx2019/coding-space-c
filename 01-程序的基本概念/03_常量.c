#include <stdio.h>
/**
 * 常量(Constant) 是程序中最基本的元素，有字符（Character）常量、整数（Integer）常量、浮点数（Floating Point）常量和枚举常量。
 *  - 常量一旦定义后，将无法再次修改;
 */

int main(){
    // 定义常量
    const double PI = 3.14;
    const char SIGN = '(';
    const int AGE = 18;

    // 浮点数格式化输出，使用'%f'占位符, 精度格式化为2位
    printf("%.2f",PI); // 3.14
    printf("\n");

    // 字符格式化输出，使用 '%c' 占位符;
    printf("%c",SIGN); // (
    printf("\n");
    // 整型格式化输出，使用 '%d' 占位符;
    printf("%d",AGE); // 18
    printf("\n");

    // 如何格式化输出 '%'字符呢?，使用'%%'占位符
    printf("%%"); // %
    return 0;
}