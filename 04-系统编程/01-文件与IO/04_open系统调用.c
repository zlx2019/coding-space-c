#include <fcntl.h>
#include <assert.h>

/**
 * int open(const char *pathname, int flags, mode_t mode);
 * 返回值：成功返回新分配的文件描述符，出错返回-1并设置errno
 * - `pathname` 参数是要打开或创建的文件名。
 * - `flags`参数有一系列常数值可供选择，可以同时选择多个常数使用`|`运算符连接起来，所以这些常数的宏定义都以o_开头，表示or。
 * - `mode`参数指定文件权限，可以用八进制数表示，比如0644。
 */

int main(int argc, char *argv[]) {

    // 打开一个文件， 返回对应的文件描述符
    int fd = open("./temp.txt",O_RDWR);
    assert(fd != -1);

    return 0;
}
