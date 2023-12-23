#include <stdio.h>

/**
 * printf函数的格式化字符串中可以用%s表示字符串的占位符;
 */

int main(int argc, char *argv[]) {
    // 通过 printf 格式化输出字符串
    char msg[] = "Hello";
    printf("%s",msg); // Hello

    /// printf会从数组str的开头一直打印到Null字符为止，Null字符本身是Non-printable字符，不打印。
    /// 这其实是一个危险的信号：如果数组str中没有Null字符，那么printf函数就会访问数组越界，
    /// 后果可能会很诡异：有时候打印出乱码，有时候看起来没错误，有时候引起程序崩溃。
    return 0;
}
