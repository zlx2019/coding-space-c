#include <stdio.h>

/**
 * 像之前提到的 printf、fprintf系列函数，称之为格式化输出函数，将数据格式化后进行输出(Output)。
 * 而本节介绍的 scanf、fscanf、sscanf、snprintf系列函数，称之为格式化输入函数。
 *
 *    函数声明:
 *      int scanf(const char* format, ...);
 *      int fscanf(FILE* stream, const char* format, ...);
 *      int sscanf(const char* s, const char* format, ...);
 *
 *  scanf从标准输入读字符,将其格式化为字符串，并且写入到指定的缓冲区。
 *  fscanf从指定的文件stream中读取内容，将其格式化为字符串，并且写入到指定的缓冲区。
 *  sscanf从指定的字符串str中读字符，将其格式化为字符串，并且写入到指定的缓冲区。
 */

int main(int argc, char *argv[]) {
    // 从标准输入中读取数据，并且格式化数据
    char line[100];
    scanf("%s",line);
    printf("%s\n",line); //


    // 格式化字符串
    char buf[50];
    sscanf("张三","%s", buf);
    printf("%s\n",buf); // 张三

    return 0;
}
