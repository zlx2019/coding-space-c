#include <stdio.h>

/**
 * 内存视图查看工具
 */

#ifndef __MEM_PRINT_H
#define __MEM_PRINT_H

/// 同时兼容C和C++使用该头文件
#ifdef __cplusplus
extern "C"{
#endif
    /**
     * 函数声明，根据一个变量的内存地址，将值的内容解析为二进制，以文本类型输出;
     * 如 char i = 1，应当输出 0000 0001
     *
     * @param ptr 变量的指针
     * @param size  变量的字节长度
     */
    void mem_println(void* ptr, size_t size);

    /**
     * 判断处理器是否为大端模式
     * @return 1: 是大端模式, 0: 小端模式
     */
    char is_big_endian();
#ifdef __cplusplus
};
#endif

#endif //__MEM_PRINT_H
