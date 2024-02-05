#include <stdio.h>
#include <string.h>

/**
 * 函数声明:
 *      size_t strcspn(const char* s, const char* charset);
 * 从字符串s的起始处开始，寻找charset子串第一次的位置，返回其位置索引值。
 *
 * 函数声明:
 *      size_t strspn(const char* s, const char* charset);
 * 从字符串s的起始处开始，寻找第一个不出现在charset中的字符，返回其位置索引值。
 *
 * 如果 s 中没有匹配 charset的内容，则返回 s 字符串长度;
 */

int main(int argc, char *argv[]) {
    // 寻找 'll' 字符串出现的位置，并返回索引;
    size_t index = strcspn("hello,hello,world","ll");
    printf("%zu \n",index); // 2

    // 如果找不到子串，则返回主串的长度
    index = strcspn("hello","ok");
    printf("%zu \n",index); // 4

    // 寻找不是 'he' 字符串出现的位置;
    index = strspn("hello","he");
    printf("%zu \n",index); // 2
    return 0;
}
