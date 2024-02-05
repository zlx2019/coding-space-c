#include <stdio.h>
#include <stdlib.h>

/**
 * 通过 popen 函数执行 cat 终端程序，并且获取到对应的标准输出;
 */

int main() {
    // 通过 popen 函数执行 cat 命令行程序，并且获取标准输出
    FILE* out = popen("cat HELP.md","r");
    if(out == NULL){
        perror("popen failed:");
        exit(1);
    }

    // 读取标准输出
    int ch;
    while ((ch = fgetc(out)) != EOF){
        putchar(ch);
    }

    // 关闭
    pclose(out);
    return 0;
}
