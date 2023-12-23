#include <stdio.h>

/**
 * 功能: 格式化字符串，将格式化完成的字符串，赋值给一个str缓冲区，并且在末尾添加'\0'结束符;
 *
 * int sprintf(char* str, const char* format,...)
 *  - str: 接收的字符串;
 *  - format: 要格式化的字符串字面量值;
 *
 * sprintf函数可能会存在缓冲区溢出，也就是提供的缓冲区，装不下格式化后的字符串，这就导致没有在末尾添加'\0'结束符
 * 这是非常危险的行为，如果对一个没有'\0'结尾的字符串进行遍历，那将无穷无尽遍历内存中的字节，直到遇到一个'\0'符号
 *
 * snprintf函数可以有效的解决这个问题
 * snprintf函数多了一个size参数，指定了缓冲区长度，如果格式化后的字符串长度超过缓冲区长度，
 * snprintf就把字符串截断到size-1字节，再加上一个'\0'写入缓冲区;
 *
 * int snprintf(char* str, size_t size, const char* format,...)
 *  - str: 接收的字符串;
 *  - size: 指定格式化字符串多少字节;
 *  - format: 要格式化的字符串字面量值;
 *
 */

int main(int argc, char *argv[]) {

    // 1. 使用sprintf格式化字符串
    // 缓冲区的大小，必须大于字符串所占字节
    // `abc`占3个字节，而'\0'也占一个字节，所以缓冲区需要4byte
    char buf[4];
    sprintf(buf,"abc");
    printf("%s \n",buf); //


    // 2. 使用snprintf格式化字符串
    // 缓冲区长度为4，但是只格式化`abcdef`字符串的前3个字节，也就是`abc`
    // 但是由于'\0'结束符是必须的，所以只会格式化 'ab'
    char line[4];
    snprintf(line, 3,"abcdef");
    printf("%s \n",line); // ab

    return 0;
}
