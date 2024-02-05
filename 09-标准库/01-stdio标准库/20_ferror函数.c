#include <stdio.h>

/**
 * int ferror(FILE* stream);
 * 用于检测文件操作是否发生了错误。如果发生了错误则返回1，如果没有发生错误则返回0;
 */

int main(int argc, char *argv[]) {
    FILE* file = fopen("HELP.md","r");
    fputc(0,file);
    if(ferror(file)){
        // 发生了错误
        perror("错误原因: "); // 错误原因: : Bad file descriptor
    }
    fclose(file);
    return 0;
}
