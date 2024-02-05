#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define msg "Hello World"

int main() {
    // 打开管道
    int fifo_fd = open("temp", O_WRONLY);
    if (fifo_fd == -1){
        perror("write open fifo");
        exit(1);
    }
    printf("write open fifo success.\n");

    // 写入数据
    write(fifo_fd,msg, sizeof(msg));
    printf("write fifo success.\n");

    // 关闭管道
    close(fifo_fd);
    return 0;
}
