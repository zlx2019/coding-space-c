#include <stdio.h>

/**
 * 整型 - char类型
 *   在C语言中char型占一个字节的存储空间，一个字节通常是8个bit。
 *   如果这8个bit按无符号(unsigned)整数来解释，取值范围是0～255;
 *   如果按有符号(signed)整数来解释，采用2's Complement表示法，取值范围是-128～127。
 *
 */


int main(int argc,char* argv[]){

    // 定义无符号 char 类型(0 ~ 255)
    unsigned char c1 = 255;

    // 定义有符号 char 类型(-128 ~ 127)
    signed char c2 =  127;
    // 有符号可以省略写为 char
    char c3 = 127;

    return 0;
}