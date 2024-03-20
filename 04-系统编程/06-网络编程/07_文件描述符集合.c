#include <stdio.h>
#include <sys/select.h>

/**
 * `fd_set`，表示为一组文件描述符的集合，可通过如下宏来对该类型操作:
 */

int main() {
    // 声明一个文件描述符集合
    fd_set set;
    FD_ZERO(&set); // 清空集合
    FD_SET(10,&set); // 添加描述符
    FD_SET(20,&set); // 添加描述符
    FD_CLR(10,&set); // 删除描述符

    // 查看描述符是否在集合中
    printf("%d\n", FD_ISSET(10,&set)); // 0
    printf("%d\n", FD_ISSET(20,&set)); // 1


    return 0;
}
