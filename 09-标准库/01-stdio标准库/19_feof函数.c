#include <stdio.h>

/**
 *  int feof(FILE* stream);
 *  用于检测文件流是否已经全部读取完毕，如果已经读取完成则返回1。如果还未完成，则返回0;
 */

int main(int argc, char *argv[]) {

    // 打开文件
    FILE* file = fopen("HELP.md","r");

    // 是否读取完毕
    printf("%d \n",feof(file)); // 0

    // 将文件读写位置移动到末尾，然后读取一个字节
    fseeko(file,0,SEEK_END);
    getc(file);

    // 再次查看是否读取完毕
    printf("%d \n",feof(file)); // 1

    // 将读写位置重置回文件起始位置
    rewind(file);

    // 再次查看是否读取完毕
    printf("%d",feof(file)); // 0

    fclose(file);
    return 0;
}
