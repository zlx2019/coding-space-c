#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    // 打开管道
    int fifo_fd = open("temp", O_RDONLY);
    if (fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
    printf("read open fifo success.\n");
    // 读取数据
    char buf[20];
    size_t n = read(fifo_fd,buf, sizeof(buf));
    printf("read fifo success.\n");

    // 输出数据
    write(STDOUT_FILENO,buf,n-1);

    // 关闭管道
    close(fifo_fd);
    return 0;
}
