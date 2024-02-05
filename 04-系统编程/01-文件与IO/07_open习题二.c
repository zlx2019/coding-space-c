#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>


// 请按照下述要求分别写出相应的open调用。

#define FILE "/home/akae.txt"

int main() {

    // 1. 打开文件/home/akae.txt用于写操作，以追加方式打开
    // int fd = open(FILE,O_WRONLY | O_APPEND);
    // assert(fd != -1);
    // close(fd);

    // 2. 打开文件/home/akae.txt用于写操作，如果该文件不存在则创建它
    // int fd = open(FILE,O_WRONLY | O_CREAT,0666);
    // assert(fd != -1);
    // close(fd);

    // 3.打开文件/home/akae.txt用于写操作，如果该文件已存在则截断为0字节，如果该文件不存在则创建它
    // int fd = open(FILE,O_WRONLY | O_CREAT | O_TRUNC);
    // assert(fd != -1);
    // close(fd);

    // 4. 打开文件/home/akae.txt用于写操作，如果该文件已存在则报错退出，如果该文件不存在则创建它
    int fd = open(FILE, O_WRONLY | O_CREAT | O_EXCL);
    assert(fd != -1);
    close(fd);

    return 0;
}
