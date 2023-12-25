#include <stdio.h>
#include <string.h>

/**
 * char* strcat(char* s1, const char* s2);
 *
 * 将s2所指的字符串，拼接到s1所指的字符串后面，并且返回s2所指的首地址;
 * 注意: s1的缓冲区，必须要能够放下s1和s2相加的字节大小，否则会溢出;
 *
 *
 * char* strncat(char* s1, const char* s2, size_t n);
 *
 * strncat函数通过参数n指定一个长度，就可以避免缓冲区溢出错误。
 * 注意这个参数n的含义和strncpy的参数n不同，它并不是缓冲区dest的长度，而是表示最多从src缓冲区中取n个字符（不包括结尾的'\0'）连接到dest后面。
 * 如果src中前n个字符没有出现'\0'，则取前n个字符再加一个'\0'连接到dest后面，所以strncat总是保证dest缓冲区以'\0'结尾
 *
 */

int main(int argc, char *argv[]) {
    char hello[30] = "Hello，";
    strcat(hello,"今天天气真好!");
    printf("%s \n", hello); // Hello，今天天气真好!

    char buf[6] = "he";
    strncat(buf,"llo,你好",3);
    printf("%s",buf); // hello
    return 0;
}
