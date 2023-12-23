#include <stdio.h>

/**
 * 本节介绍一些 在C语言中，不同进制的表示形式;
 *  - 二进制: 在数字前加上`0b`或者`0B`前缀，表示为二进制数字;
 *  - 八进制: 在数字前加上`0`前缀，表示为八进制数字;
 *  - 十六进制: 在数字前加上`0x`或者`0X`前缀，表示为十六进制数字;
 */

int main(int argc, char *argv[]) {

    // 定义一个整型，以二进制形式
    int binaryNumber1 = 0b1010;
    int binaryNumber2 = 0B1011;
    // 以十进制形式输出
    printf("%d \n",binaryNumber1); // 10
    printf("%d \n",binaryNumber2); // 11


    // 定义八进制整数
    int octNumber1 = 010;
    int octNumber2 = 011;
    // 以十进制形式输出
    printf("%d \n",octNumber1); // 8
    printf("%d \n",octNumber2); // 9


    // 定义十六进制
    int hexNumber1 = 0xf;
    int hexNumber2 = 0xff;
    // 以十进制形式输出
    printf("%d \n",hexNumber1); // 15
    printf("%d \n",hexNumber2); // 255

    return 0;
}
