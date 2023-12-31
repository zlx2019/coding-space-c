#include <stdio.h>

/**
 * 下面通过一个简单的例子证明标准输出对应的终端设备是行缓冲的。
 */

int main(int argc, char *argv[]) {
    // printf("hello world");
    printf("hello world\n");
    while (1);
    // 运行这个程序，会发现hello world并没有打印出来。按Ctrl+C组合键终止它，去掉程序中的while(1);语句再试一次：

    // main方法调用流程:
    // 我们知道main函数被启动代码这样调用：exit(main(argc, argv));。main函数return时启动代码会调用exit函数，
    // exit函数首先关闭所有尚未关闭的FILE ＊指针（关闭之前要自动做Flush操作），然后通过_exit系统调用进入内核退出当前进程。

    // 在上面的例子中，由于标准输出是行缓冲的，printf("hello world");打印的字符串没有换行符，
    // 所以只把字符串写到标准输出的I/O缓冲区而没有写回内核，如果按Ctrl+C组合键，进程是异常终止的，并没有调用exit，也就没有机会做Flush操作，
    // 因此字符串最终没有打印到屏幕上。

    // 如果把打印语句改成printf("hello world\n");，有换行符，就会立刻写回内核。或者如果把while(1);去掉也可以写回内核，
    // 因为从main函数return相当于调exit函数。在本书的其他例子中，printf打印的字符串末尾都有换行符，以保证字符串在printf调用结束时就写回内核，
    // 如果你用printf打印调试信息，保证这一点尤其重要。
    return 0;
}
