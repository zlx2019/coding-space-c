#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * 数组的定义语法：
 *  第二种:
 *    数组类型* 数组名 = malloc(数组的内存长度);
 *  这种方式，会将数组内存分配到堆中
 */

int main(int argc, char *argv[]) {
    /// 定义一个长度为3的int类型数组
    int* arr = malloc(sizeof(int) * 3);

    // 清空申请到的内存
    memset(arr,0, sizeof(arr));

    // 计算数组元素容量
    int size = sizeof(arr) / sizeof(arr[0]);

    return 0;
}
