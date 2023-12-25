#include <stdio.h>

/**
 * fgetc函数从指定文件句柄中读取一个字节，并且返回;
 *
 * int fgetc(FILE* stream);
 *  - stream: 要读取的文件句柄，该文件的打开方式必须是可读的;
 * 成功返回读取到的字节，出错或者文件已经没有可读的则返回EOF;
 *
 * 系统对于每个打开的文件都记录着当前读写位置，
 * 当文件打开时，读写位置在文件开头，每调用一次fgetc，读写位置向后移动一个字节，
 * 因此可以连续多次调用fgetc函数依次读取多个字节。
 *
 *
 * 该函数明明读取的是一个字节，也就是一个char，那为什么要使用int作为返回值?
 * 如果规定返回值是unsigned char型，那么当返回值是0xff时无法区分到底是EOF还是字节0xff。
 *
 * getchar()函数用于从标准输入(stdin)中读取一个字节,相当于 fgetc(stdin);
 * int getchar();
 */

int main(int argc, char *argv[]) {
    // 从标准输入中，读取一个字节
    int byte = fgetc(stdin);
    printf("%c \n",byte);
    return 0;
}
