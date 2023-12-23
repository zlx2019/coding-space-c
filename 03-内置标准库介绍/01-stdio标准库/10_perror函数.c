#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
 * perror 函数将指定字符串打印到标准错误输出，然后根据errno的值，打印出对应的错误原因;
 *
 * void perror(const char* msg);
 *  - msg: 自定义提示消息
 */

int main(int argc, char *argv[]) {
    /// 这里以打开一个不存在的文件为例，通过perror()输出具体错误原因
    FILE* file = fopen("abc","r");
    if(file == NULL){
        // 输出 错误码
        fprintf(stderr,"errno: %d \n",errno); // errno: 2
        // 输出具体的 错误原因
        perror("open file abc failed"); // open file abc failed: No such file or directory
        // perror函数读取errno的值，将ENOENT解释成字符串No such fiIe or directory并打印
        exit(1);
    }
    return 0;

    //
}
