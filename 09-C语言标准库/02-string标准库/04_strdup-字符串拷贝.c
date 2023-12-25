#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * 字符串的拷贝也可以用strdup(3)函数实现，这个函数属于POSIX标准但并不属于C标准。
 *
 * char* strdup(const char* s);
 *  - 要拷贝的字符串字面量;
 *
 * 这个函数调用malloc动态分配内存，把字符串s拷贝到新分配的内存中然后返回。
 * 用这个函数省去了事先为新字符串分配内存的麻烦，但是用完之后要记得调用free函数释放新字符串的内存。
 */

int main(int argc, char *argv[]) {
    const char* name = "张三";
    char* new_name = strdup(name);
    printf("%s",new_name); // 张三
    free(new_name);
    return 0;
}
