#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/**
 * 案例需求: 阻塞 SIGINT、SIGSEGV和SIGQUIT信号，当程序运行后，通过终端发送这三个信号
 *
 */

void mem_println(const void* ptr, size_t size);

int main() {
    // 添加信号屏蔽
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGSEGV);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 循环10秒，每秒输出一次当前未决信号信息
    int sec = 30;
    sigset_t pending_set;
    for (; sec > 0; sec--) {
        // 获取当前进程未决信号集
        sigpending(&pending_set);

        // 输出未决信号集信息，sigset_t 占4个字节，共32bit，每个bit对应一个信号是否处于未决，1表示是，0表示否。
        mem_println(&pending_set, sizeof(pending_set));

        sleep(1);
    }
    // 解除信号屏蔽
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    return 0;
}

// 程序运行过程:
//00000000,00000000,00000000,00000000
//00000000,00000000,00000000,00000000
//^C 触发 SIGINT 信号
//00000000,00000000,00000000,00000010
//00000000,00000000,00000000,00000010
//00000000,00000000,00000000,00000010
//00000000,00000000,00000000,00000010
//^\ 触发 SIGQUIT信号
//00000000,00000000,00000000,00000110
//通过kill -11 pid 发送 SIGSEGV 信号
//00000000,00000000,00000100,00000110
//00000000,00000000,00000100,00000110
//00000000,00000000,00000100,00000110


void mem_println(const void* ptr, size_t size){
    char* start_byte = ptr + size - 1;
    for(size_t i = 0; i < size; i++){
        char* current_byte = (start_byte - i);
        char current_byte_val = *current_byte;
        for(int j = 8 - 1; j >= 0; j--){
            int bit_val = (current_byte_val & (1 << j)) == 0 ? 0 : 1;
            printf("%d",bit_val);
        }
        if(i != size -1){
            printf(",");
        }
    }
    printf("\n");
}
