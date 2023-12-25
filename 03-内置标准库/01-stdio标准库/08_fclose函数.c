#include <stdio.h>

/**
 * int close(FILE* fp);
 *  - fp: 要关闭的文件句柄;
 *
 * 关闭文件句柄,关闭之后该文件指针就无效了，不能再使用了。
 * 如果fclose调用出错（比如传给它一个无效的文件指针）则返回EOF并设置errno;
 *
 * fopen调用应该和fclose调用配对，打开文件操作完之后一定要记得关闭。
 * 如果不调用fclose，在进程退出时内核会自动关闭该进程打开的所有文件，但不能因此就忽略fclose调用，
 * 如果写一个长年累月运行而不退出的程序（比如网络服务器程序），打开的文件都不关闭，堆积得越来越多，就会占用越来越多的系统资源。
 *
 */

int main(int argc, char *argv[]) {
    FILE* file = fopen("~/.bash_history","r");
    // 关闭文件流
    fclose(file);
    return 0;
}
