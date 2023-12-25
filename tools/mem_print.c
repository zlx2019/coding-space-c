// 引入对应的头文件
#include "mem_print.h"



/**
 * 函数声明，根据一个变量的内存地址，将值的内容解析为二进制，以文本类型输出;
 * 如 char i = 1，应当输出 0000 0001
 *
 * @param ptr 变量的指针
 * @param size  变量的字节长度
 */
void mem_println(void* ptr, size_t size){
    // TODO 以小端 or 大端模式输出
    // 从该地址的最末尾字节开始解析
    char* start_byte = ptr + size - 1;
    // 遍历内存的所有的字节
    for(size_t i = 0; i < size; i++){
        // 当前字节
        char* current_byte = (start_byte - i);
        // 当前字节内容
        char current_byte_val = *current_byte;
        // 遍历字节的每一个bit，判断是0还是1
        for(int j = 8 - 1; j >= 0; j--){
            // 使用`1`与当前bit进行与运算(&)，结果为0，则表示bit为0，反之为1;
            int bit_val = (current_byte_val & (1 << j)) == 0 ? 0 : 1;
            printf("%d",bit_val);
        }
        // 输出完一个字节，使用,隔开
        if(i != size -1){
            printf(",");
        }
    }
    printf("\n");
}


/**
 * 判断处理器是否为大端模式
 * @return 1: 是大端模式, 0: 小端模式
 */
char is_big_endian(){
    int i = 1;
    if(*((char*) &i) == 1){
        return 0;
    }
    return 1;
}
