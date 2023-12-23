#include <stdio.h>

/**
 * 功能: 格式化字符串，并且将格式化后的字符串输出到指定的IO流中;
 * 例如 通过该函数进行标准输出、对文件句柄FILE* 进行输出等等...
 *
 *
 * int fprintf(FILE ＊stream, const char ＊format, ...);
 *  - stream: 字符串输出的目标IO流;
 *  - format: 要格式化的字符串字面量值;
 * 返回输出的字节数(不包含末尾的'\0')，如果出错则返回负值。
 */

int main(int argc, char *argv[]) {
    // 将'Hello World!'写入到标准输出和标准错误输出
    fprintf(stdout,"Hello World!\n");
    fprintf(stderr,"Hello World!\n");
    return 0;
}
