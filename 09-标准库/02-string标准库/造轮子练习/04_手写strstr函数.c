#include <stdio.h>

/**
 * 手动实现strstr函数
 */


/**
 * 在一个字符串中，找出指定的子串(不推荐使用，效率很差)
 *
 * @param haystack 主串
 * @param needle   子串
 * @return         子串首字符地址
 */
char* k_strstr(const char* haystack, const char* needle){
    for (int i = 0; 1; i++) {
        int index = 0;
        for (int j = 0; 1; j++) {
            // 找到了子串
            if(*(needle + j) == '\0'){
                if(j == 0) return NULL;
                return (char*) haystack + i;
            }
            // 不匹配，遍历下一个主串字符
            if(*(haystack + (i + index++)) != *(needle + j)){
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char* world = k_strstr("hello,world!","world");
    printf("%s",world); // world!
    return 0;
}
