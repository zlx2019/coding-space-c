#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * fputs 函数用于向一个文件流中写入一个字符串;
 *      int fputs(const char* s, FILE* stream);
 *      - s: 要写入的字符串
 *      - stream: 要写入的文件流
 * 返回一个非负值，表示写入成功，返回EOF(-1)，表示出现错误;
 *
 * puts(const char* s);
 * 用于向标准输出写入一个字符串;
 */

/**
 * 根据标准输入，创建一个文件，并且写入内容;
 */
int main(int argc, char *argv[]) {
    // 定义缓冲区，用于从标准输入中读取输入
    char fileName[32];
    char line[50];

    // 接收文件名
    printf("请输入要创建的文件名称(已存在则追加内容): ");
    fscanf(stdin,"%s",fileName);

    // 创建或打开该文件，以追加模式打开，不存在则创建
    FILE* file = fopen(fileName,"a+");
    if (file == NULL){
        perror("open file failed");
        exit(1);
    }
    printf("请输入文件内容: ");

    // 循环从标准输入中读取内容，并且写入到文件中
    while ((fgets(line, sizeof(line),stdin)) != NULL){
        if (strncmp(line,"quit",4) == 0){
            // 主动结束输入
            break;
        }
        // 写入到文件中
        int count = fputs(line,file);
        if (count == EOF){
            fprintf(stderr,"写入发生错误.");
            break;
        }
    }

    // 关闭文件
    fclose(file);
    return 0;
}
