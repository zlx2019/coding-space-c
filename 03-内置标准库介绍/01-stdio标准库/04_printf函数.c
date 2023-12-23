#include <stdio.h>

/**
 * 功能: 格式化字符串，并且将格式化后的字符串，写入到标准输出(stdout)，
 *
 *
 * int printf(const char* format,...);
 *  - format: 要格式化的字符串字面量值
 * 返回输出的字节数(不包含末尾的'\0')，如果出错则返回负值。
 */
int main() {
    int count = printf("abc\n");
    printf("%d",count); // 4
    return 0;
}
