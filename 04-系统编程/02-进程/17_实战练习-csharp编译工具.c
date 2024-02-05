#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/** ====================== file desc ========================
 * 使用C语言，实现C#单个文件的编译与运行辅助程序。
 * 1. 通过C#的 csc 编译器进行源文件的编译;
 * 2. 然后通过mono运行时来运行可执行程序;
 * 3. 最后删除可执行程序;
 *
 * 参考资料:
 *      https://learn.microsoft.com/zh-cn/dotnet/csharp/language-reference/compiler-options/output
 *      https://www.cnblogs.com/cdaniu/p/15468453.html
 *
 * 编译命令: gcc -o cs 17_实战练习-csharp编译工具.c
 * 拷贝到PATH目录下: cp ./cs /usr/local/bin
 *
 * ========================================================= */

// 编译源文件时输出文件名称
#define OUTPUT_NAME     "-out:__temp"
#define NAME            "__temp"

// 指定本次编译的目标平台 CPU
#define PLATFORM_TARGET "-platform:anycpu"

// 关闭csc 编译器输出logo版本相关信息
#define NO_LOGO "-nologo"

// 以 UTF-8 编码格式输出编译器消息，防止编译出错时显示的中文乱码
#define OUTPUT_ENCODE "/utf8output"

// 结束程序
void quit(const char*);
// 判断一个进程是否正常结束，并且exit以0状态值退出;
int processSuccess(int status);


int main(int argc,const char *argv[]) {
    // 参数校验
    if(argc <= 1){
        fprintf(stderr,"usage: %s [csharp script file]\n",argv[0]);
        exit(1);
    }

    // 要编译的C#源文件
    const char* csFile = argv[1];

    // 创建一个子进程进行编译
    int compileProcess = fork();
    switch (compileProcess) {
        case -1:
            // 创建编译进程失败
            quit("fork compile process failed");
        case 0:
            // 编译源文件
            execlp("csc","csc",NO_LOGO, OUTPUT_ENCODE, OUTPUT_NAME,PLATFORM_TARGET, csFile, NULL);
            quit("exec compile process failed");
    }

    // 等待编译进程完成，并且获取终止状态
    int compileProcessStatus;
    waitpid(compileProcess, &compileProcessStatus, 0);

    // 编译进程正常终止，并且终止状态值为0
    if (processSuccess(compileProcessStatus)){
        // 编译成功
        // 创建子进程，执行编译好的可执行程序
        int runProcess = fork();
        switch (runProcess) {
            case -1:
                // 创建运行进程失败
                quit("fork running process failed");
            case 0:
                // 运行可执行程序
                execlp("mono","mono",NAME,NULL);
                quit("exec running process failed");
        }

        // 等待执行子进程完成，并且获取终止状态
        int runProcessStatus;
        waitpid(runProcess,&runProcessStatus,0);

        // 运行完成，无论是否正常执行成功都需要删除生成的可执行文件
        // 创建子进程，删除可执行文件
        if (fork() == 0){
            execlp("rm", "rm", "-f", NAME, NULL);
        }
    }
    return 0;
}

/**
 * 根据一个进程的终止信息，判断该进程是否正常终止，并且终止状态为0
 *
 * @param status 进程的终止信息
 */
int processSuccess(int status){
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

/**
 * 将信息输出到标准错误输出，并且结束进程.
 * @param message   错误信息
 */
void quit(const char* message){
    fprintf(stderr,"%s:%s\n",message, strerror(errno));
    exit(1);
}
