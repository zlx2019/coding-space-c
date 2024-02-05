#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * 编写一个非正常程序，验证 僵尸进程;
 *
 * gcc -o zombie 13-僵尸进程.c
 * ./zombie &
 * ps -u
 * 
 */

int main(void) {
    pid_t pid = fork();
    if(pid > 0){
        while (1);
    }
    return 0;
}
