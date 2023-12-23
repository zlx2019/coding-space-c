#include <stdio.h>
#include <string.h>

/**
 * 关于字符串的拷贝，标准库内提供了两个函数: strcpy和strncpy函数;
 *
 * strcpy
 *      char* strcpy(char* dest, const char* src);
 *       - dest: 目标字符串地址
 *       - src: 源字符地址
 *  将src所指向的字符串拷贝到dest的内存空间，并且返回dest;
 *  从src首字符，到下一个'\0'字符(包括'\0')之前的字符，全部拷贝到dest内存;
 *  所以必须保证: dest的长度 >= src的长度 + 1，否则会产生溢出;
 *  strcpy函数能够保证dest的末尾是以'\0'结尾，因为它会不断的拷贝src的字符，直到遇到'\0'后结束;
 *
 * strncpy
 *      char* strcpy(char* dest, const char* src, size_t n);
 *       - dest: 目标字符串地址
 *       - src: 源字符地址
 *       - n: 要拷贝的字节数
 *  从src所指向的字符串，拷贝n个字节到dest的内存空间，并且返回dest;
 *  strncpy的参数n指定最多从src中拷贝n个字节到dest中,该函数有如下几种情况:
 *  1. 如果没有拷贝够n个字节就遇到'\0'直接结束;
 *  2. 如果拷贝到够了n个字节，依然没有遇到'\0'，那么也结束;
 *  3. 如果src字符串全部拷贝完了不足n个字节，那么还差多少个字节就补多少个'\0';
 *
 * 注意:
 * 无论是strcpy函数还是strncpy函数，src和dest所指向的内存空间不能有重叠，例如这样调用是不允许的：
 * char buf[10] = "hello";
 * strcpy(buf,buf + 1);
 *
 */

int main(int argc, char *argv[]) {
    char* src = "Hello World!";

    // 通过strcpy 拷贝整个字符串
    char dest1[strlen(src) + 1];
    strcpy(dest1,src);
    printf("%s \n",dest1); // Hello World!

    // 通过 strncpy 只拷贝 Hello 子串
    char dest2[5 + 1];
    strncpy(dest2,src,5);
    printf("%s \n",dest2); // Hello
    return 0;
}
