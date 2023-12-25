#include <stdio.h>
#include <string.h>

/**
 * 字符串的比较，共提供三种函数memcmp、strcmp、strncmp;
 *
 * int memcmp(const void ＊s1, const void ＊s2, size_t n);
 *  比较s1和s2的前n个字节，即使这些字节里包含'\0'字符也不会停止比较，如果这些字节全部一样则返回0，
 *  如果出现了不相等的字节,则比较这两个字节的大小，返回-1表示s2大，返回1表示s1大;
 *
 * int strcmp(const char ＊s1, const char ＊s2);
 *  将s1和s2作为字符串进行比较，一旦其中一个字符串遇到了'\0'字符，则比较完当前两个字符就结束;
 *
 * int strncmp(const char ＊s1, const char ＊s2, size_t n);
 *  该函数结合了上面两个函数的特点: 比较时遇到'\0'会结束，或者比较完n个字节后结束;
 *
 * 返回0表示相等
 * 返回-1表示s1 < s2
 * 正值表示s1 > s2
 *
 */

int main(int argc, char *argv[]) {
    // 使用memcpm 进行字符串比较
    printf("%d \n",memcmp("abc","abc",3)); // 0
    printf("%d \n",memcmp("abh","abj",2)); // 0
    printf("%d \n",memcmp("abh","abj",3)); // -1
    printf("%d \n",memcmp("abd","abb",3)); // 1


    // 使用 strcmp 进行字符串比较
    printf("%d \n",strcmp("54321","12345")); // 1
    printf("%d \n",strcmp("12345","12345")); // 0
    printf("%d \n",strcmp("12345","54321")); // -1
    return 0;
}





