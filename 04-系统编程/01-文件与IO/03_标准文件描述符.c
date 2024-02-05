#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * 每个程序启动时会自动打开三个文件：标准输入、标准输出和标准错误输出。
 * 在C标准库中分别使用`FILE*`类型的`stdin`、`stdout`、`stderr`表示。
 * 这三个文件是最先打开的，所以描述符分别是0、1、2，保存在对应的`FILE`结构体中。
 * 头文件`unistd.h`中有如下的宏定义来标识这三个文件描述符:
 */

int main(int argc, char *argv[]) {
    printf("标准输入: %p，文件描述符: %d \n",stdin,STDIN_FILENO);
    printf("标准输出: %p，文件描述符: %d \n",stdout,STDOUT_FILENO);
    printf("标准输入: %p，文件描述符: %d \n",stderr,STDERR_FILENO);
    // 标准输入: 0x1f54c93d8，文件描述符: 0
    // 标准输出: 0x1f54c9470，文件描述符: 1
    // 标准输入: 0x1f54c9508，文件描述符: 2
    return 0;
}
