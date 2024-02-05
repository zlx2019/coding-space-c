#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * 通过 exec 系列函数，执行各种其他程序
 */
int main() {
    if(fork() == 0){
        /// 1. 通过 execl 执行 ls 程序
        ///     path: 要执行的程序路径(相对或绝对路径均可)
        ///     arg:  程序的命令行参数
        /// 参数中的首个参数 ls 其实对于程序执行并不影响，它只是将该值传给ps程序，
        /// 然后使ps程序可以通过main函数的argv[0]获取到
        execl("/bin/ls","ls","-a",NULL);
    }


    if (fork() == 0){
        /// 2. 通过execlp 执行 ls 程序
        ///    file: 要执行的程序名，会从PATH环境变量中自动寻找目录
        ///    arg: 程序的参数
        execlp("ls","ls", "-a", NULL);
    }

    if (fork() == 0){
        extern char** environ;
        /// 3. 通过execle 执行 ls 程序
        ///     path: 要执行的程序路径
        ///     arg: 程序的参数
        ///     evns: 手动传递的环境变量表
        execle("/bin/ls","ls",NULL,environ);
    }

    if (fork() == 0){
        /// 4. v系列函数以 execvp 为例,执行 ls 程序
        char* const args[] = {"ps", "-o", "pid",NULL};
        execvp("ps",args);
    }
    return 0;
}
