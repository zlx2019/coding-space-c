#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

/**
 *  创建 fifo 命名管道
 */

int main(int argc, char *argv[]) {
    int pfd;
    // 创建fifo
    if((pfd = mkfifo("temp",0644)) == -1){
        perror("create fifo failed");
        exit(1);
    }
    return 0;
}
