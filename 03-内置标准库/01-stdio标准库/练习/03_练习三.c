#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


/**
 * 编程读写一个文件test.txt，每隔1秒钟向文件中写入一行记录，类似于这样：
 *      1 2009-7-30 15:16:42
 *      2 2009-7-30 15:16:43
 *   该程序应该无限循环，直到按Ctrl+C组合键终止。下次再启动程序应该在test.txt文件末尾追加记录，并且序号能够接续上次的序号
 */

// 文件名
#define FILE_NAME "test.txt"

/**
 * 获取上一次程序结束前生成的最后一行记录的行号
 */
int getLastLineNumber(){
    FILE* file = fopen(FILE_NAME,"r");
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF){
        if(ch == '\n'){
            count++;
        }
    }
    return count;
}


int main() {
    // 创建或打开文件
    FILE* file = fopen(FILE_NAME,"a+");
    if(file == NULL){
        perror("Open file failed:");
        return EXIT_FAILURE;
    }

    // 获取最后一条记录的序号，并且 +1 作为起始序号
    int no = getLastLineNumber() + 1;

    // 读写位置移动到最末尾
    fseek(file,0,SEEK_END);

    // 当前系统时间
    time_t now;
    struct tm* timeInfo;

    // 字符串缓冲区，用于记录生成的每一行内容
    char timeLine[80];

    // 循环运行
    while (1){
        // 获取当前系统时间
        time(&now);
        // 将 time_t 转换 为 struct tm 类型
        timeInfo = localtime(&now);
        // 时间类型格式化
        strftime(timeLine, sizeof(timeLine),"%Y-%m-%d %H:%M:%S",timeInfo);

        // 写入到文件中
        fprintf(file,"%d %s\n",no++,timeLine);

        // 刷新到内核
        fflush(file);

        // 睡眠1s
        sleep(1);
    }
    return 0;
}
