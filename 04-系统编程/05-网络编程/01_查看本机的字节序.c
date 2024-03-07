#include <stdio.h>


/**
 * 判断处理器是否为大端模式 方式一
 * @return 1: 是大端模式, 0: 小端模式
 */
char is_big_endian(){
    int i = 1;
    if(*((char*) &i) == 1){
        return 0;
    }
    return 1;
}



/**
 * 判断当前架构 是大端模式还是小端模式 方式二
 * @return
 */
int main() {
    union {
        short s1;
        char s2[2];
    } block;
    block.s1 = 0x0201;

    printf("%d,%d \n",block.s2[0], block.s2[1]);
    //如果 低位数据放在低地址 那么就是小端
    if(block.s2[0] == 1 && block.s2[1] == 2){
        printf("是小端\n");
    } else{
        printf("是大端\n");
        // 否则就是大端
    }
    return 0;
}


