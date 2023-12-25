#include <stdio.h>

/**
 * 手动实现一个 memcpy 和 memmove 函数
 *
 * 在x64平台下，每次拷贝1个字节需要一条指令，每次拷贝8个字节同样也是一条指令，
 * 所以尽可能利用这点，来优化拷贝方法;
 */


/**
 * 手写 memcpy 函数实现
 * @param dest 要拷贝到的位置首地址
 * @param src  要拷贝的内存首地址
 * @param size 要拷贝的字节数量
 * @return
 */
void* k_memcpy(void* dest, void* src, size_t size){
    // 强转为 long 类型指针
    long* l_dest = dest;
    long* l_src = src;

    // 先每次拷贝8个字节
    int l_size = size / 8;
    while (l_size--){
        *l_dest++ = *l_src++;
    }
    // 剩余不到8个字节，一个一个拷贝即可
    int c_size = size % 8;
    // 强转为char类型指针
    char* c_dest = (char*)l_dest;
    char* c_src = (char*) l_src;
    while (c_size--){
        *c_dest++ = *c_src++;
    }
    return dest;
}

/**
 * TODO 后续有空了实现
 * @return
 */
void* k_memmove(void* dest, void* src, size_t size){

}



int main(int argc, char *argv[]) {
    // 测试 k_memcpy 函数
    long l1[] = {100,200,300};
    long l2[3];
    // 将 l1 拷贝给 l2，拷贝24个字节
    k_memcpy(l2,l1, sizeof(long) * 3);
    printf("%ld,%ld,%ld \n",l1[0],l1[1],l1[2]); // 100,200,300
    printf("%ld,%ld,%ld \n",l2[0],l2[1],l2[2]); // 100,200,300
    return 0;
}
