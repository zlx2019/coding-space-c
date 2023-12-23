#include <stdio.h>

/**
 * 手动实现一个strcpy和strncpy函数
 */

/**
 * 字符串拷贝
 * @param dest 目标地址
 * @param src  被拷贝的源字符串地址
 * @return     返回拷贝后的目标地址
 */
char* k_strcpy(char* dest, const char* src){
    // 记录首地址，作为返回值
    char* destTemp = dest;
    // 将src的每一个字符，赋值给dest地址，遇到\0后就结束;
    int i = 0;
    do {
        *(dest+i) = *(src+i);
    } while (*(src + i++) != '\0');
    return destTemp;
}


/**
 * 字符串拷贝
 * @param dest 目标地址
 * @param src  被拷贝的源字符串地址
 * @param len  要拷贝的字节数
 * @return     返回拷贝后的目标地址
 */
char* k_strncpy(char* dest, const char* src, size_t len){
    int isEnd = 0;
    for (int i = 0; i < len; i++) {
        if(isEnd){
            *(dest + 1) = '\0';
        }else {
            *(dest + i) = *(src + i);
            if(*(src + i) == '\0'){
                isEnd = 1;
            }
        }
    }
    return dest;
}



int main(int argc, char *argv[]) {
    const char* src = "hello world";
    char line[12];
    k_strcpy(line,src);
    printf("%s \n",line); // hello world


    char dest[6];
    k_strncpy(dest,src, sizeof(dest) -1);
    dest[sizeof(dest) - 1] = '\0';
    printf("%s \n",dest); // hello world
    return 0;
}
