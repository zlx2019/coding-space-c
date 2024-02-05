#include <stdio.h>
#include <ctype.h>

/**
 * 将标准输入中的所有内容，转换为大写后，输出到标准输出;
 *
 * 编译命令: gcc -o upper 10-upper-大小写转换程序.c
 */

int main(void) {
    int ch;
    while (~(ch = getchar())){
        // 将字符转换为大写，并且输出到控制台
        putchar(toupper(ch));
    }
    return 0;
}

// ./a.out < xxx.txt