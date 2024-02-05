#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

/**
 * int close(int fd);
 * 返回值：成功返回0，出错返回-1并设置errno
 *
 * - `fd`参数是要关闭的文件描述符。
 */

int main(int argc, char *argv[]) {
    int fd = open("./temp.txt",O_RDWR);
    assert(fd != -1);

    // 关闭文件
    close(fd);
    return 0;
}
