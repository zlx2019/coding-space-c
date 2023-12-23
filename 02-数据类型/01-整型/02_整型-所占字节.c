#include <stdio.h>

/**
 * 除了char型之外，整型还包括如下几种:
 *  - short int，简称为 short;
 *  - int;
 *  - long int，简称为 long;
 *  - long long int，简称为 long long;
 *  这些类型都可以加上signed或unsigned关键字表示有符号或无符号数。
 *  用这两个关键字修饰int型时，signed int和unsigned int可以简写为signed和unsigned。
 *
 *  除了char型在C标准中明确规定占一个字节之外，其他整型占几个字节都是Implementation Defined。
 *  通常的编译器实现遵守ILP32或LP64规范，如下:
 *
 *  类型        32位系统       64位系统
 *  char        1byte           1byte
 *  short       2byte           2byte
 *  int         4byte           4byte
 *  long        4byte           8byte
 *  longlong    8byte           8byte
 *  指针         4byte           8byte
 *
 *
 *
 */

int main(int argc, char *argv[]) {
    return 0;
}
