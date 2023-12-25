#include <stdio.h>

/**
 * int fflush(FILE* stream);
 * 返回值: 成功返回0，出错返回EOF，并且设置errno;
 *
 * 表示立刻将stream的用户态IO缓冲区写入到内核;
 */

int main(int argc, char *argv[]) {
    FILE* file = fopen("temp.txt","w+");
    fputc('A',file);// 此时只是将'A' 写入到了IO缓冲区
    fflush(file); // 强制将缓冲区中的'A' 写入到内核
    fclose(file);
    return 0;
}
