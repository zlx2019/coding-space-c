#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * 通过 read 和 write 实现从终端读取数据，再写回终端.
 */

int main(void) {
    char buf[10]; // 定义缓冲区
    int n;  // 读取到的字节数

    // 读取标准输入
    n = read(STDIN_FILENO,buf, sizeof(buf));
    if(n == -1){
        perror("read stdin failed:");
        exit(1);
    }
    // 将数据写回标准输出
    write(STDOUT_FILENO,buf,n);
    return 0;
}
