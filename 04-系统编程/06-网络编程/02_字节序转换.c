#include <stdio.h>
#include <arpa/inet.h>

/**
 * 大小端模式之间的互转
 */

int main() {
    // 定义一个数值
    int n = 0x12345678;

    // 以16进制方式输出
    printf("0x%x\n",n); // 0x12345678

    // 当前默认是小端模式的，接下来转换成大端模式
    int bigN = htonl(n);
    printf("0x%x\n",bigN); // 0x78563412

    // 再转为小端
    int smallN = ntohl(bigN);
    printf("0x%x\n",smallN); // 0x12345678
    return 0;
}
