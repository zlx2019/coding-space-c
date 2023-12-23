#include <stdio.h>
#include <stdlib.h>

/**
 * fputc函数向指定的文件句柄中写入一个字节;
 * putchar向标准输出写一个字节，调用putchar(c)相当于调用fputc(c, stdout);
 *
 * int fputc(int c,FILE* stream);
 * int putchar(int c);
 * - c: 要输出的字符
 * - stream: 输出到哪里
 *
 * 说明:
 *  - 要用fputc函数写一个文件，该文件的打开方式必须是可写的(包括追加);
 *  - 每调用一次fputc，读写位置向后移动一个字节，因此可以连续多次调用fputc函数依次写入多个字节。
 *  - 如果文件是以追加方式打开的，则每次调用fputc总是先把读写位置移到文件末尾然后把要写入的字节追加到后面。
 *
 * 下面完成一个练习: 创建一个文件，从键盘中读取一串字符，写入文件，并且再次读取出来输出到终端;
 *
 */

int main() {
    FILE* file;
    int ch;

    // 1. 创建/打开文件
    file = fopen("temp.txt","w+");
    if(file == NULL){
        perror("open file temp.txt failed");
        exit(1);
    }

    // 2. 循环读取标准输入的每个字节，写入到文件中，直到读取到文件结束符(Ctrl+D)
    while ((ch = getchar()) != EOF){
        fputc(ch,file);// 写入文件
    }

    // 3. 将文件读写位置，重置到文件开头
    rewind(file);

    // 4. 循环读取文件内容的每个字，打印到标准输出;
    while ((ch = fgetc(file)) != EOF){
        putchar(ch); // 打印到终端
    }

    // 5. 关闭文件
    fclose(file);
    return 0;
}
