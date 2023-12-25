#include <stdio.h>
#include <stdlib.h>

/**
 * 想一想，如果要打开一个文件做写操作，覆盖文件开头1KB的内容，而后面的内容保持不变，应该以哪种模式打开？
 *
 * 这里改一下需求，覆盖一个文件的开头5个字节即可，后面的内容保持不变;
 */

int main(int argc, char *argv[]) {
    // 1. 打开文件，以读写模式(覆盖)
    FILE* file = fopen("temp.txt","r+");
    if(file == NULL){
        // 错误处理
        perror("open file temp.txt failed");
        exit(1);
    }
    // 移动文件指针到文件开头的第 5 字节处
    fseek(file,5,SEEK_SET);
    // 写入新的内容，这将覆盖目前文件指针所在位置之前的所有内容
    fprintf(file,"abcd\n");
    // 关闭文件
    fclose(file);

    // 文件内容: 0123456789
    // 运行后: 01234abcd
    return 0;
}
