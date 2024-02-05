#include <stdio.h>
#include <string.h>

/**
 * char* strstr(const char* haystack, const char* needle);
 *  - haystack: 长串
 *  - needle: 要搜索的子串
 *
 * strstr在一个长字符串中从前到后查找一个子串（Substring），找到子串第一次出现的位置就返回，返回值指向子串的开头，如果找不到就返回NULL。
 * 注意: 这里是返回子串的首地址，而不是返回完整的子串;
 */

int main(int argc, char *argv[]) {
    char* world = strstr("hello,world!","world");
    printf("%s",world); // world!
    return 0;
}
