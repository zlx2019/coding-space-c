#include <unistd.h>

// 定义字符串和字符串长度
char msg[14] = "Hello, world!\n";
#define msg_len 14

int main(){
    // 直接通过系统调用函数，将msg 写入到 标准输出中
    write(1,msg,msg_len);
    // 结束程序
    _exit(0);
}