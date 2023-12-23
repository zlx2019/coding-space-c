#include <stdio.h>
#include <string.h>

/**
 * size_t strlen(const char* s);
 * 返回字符串 s 的长度 (不包含'/0')，从s的第一个字符开始找'\0'字符，一旦找到就返回;
 */

int main(int argc, char *argv[]) {
    char buf[] = "hello";
    size_t length = strlen(buf);
    printf("%zu\n",length); // 5

    //
    char* line = "hello";
    printf("%zu \n",strlen(line)); // 5
    return 0;
}
