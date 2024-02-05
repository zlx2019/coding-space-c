#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

/**
 * 打开 FIFO 管道
 */

int main(int argc, char *argv[]) {
    int fifo_fd = open("temp",O_WRONLY | O_NONBLOCK);
    if(fifo_fd == -1){
        perror("open fifo");
        exit(1);
    }
}
