#include <stdio.h>
#include <unistd.h>

/**
 * ssize_t	 read(int fd, void* buf, size_t count);
 * 返回值：成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0;
 */

int main(int argc, char *argv[]) {
    // 通过 read 系统调用函数，直接从标准输入中获取数据
    char buf[20];
    read(STDIN_FILENO,buf, sizeof(buf));
    printf("%s\n",buf);
    return 0;
}
