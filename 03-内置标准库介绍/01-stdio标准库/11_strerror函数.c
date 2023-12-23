#include <stdio.h>
#include <string.h>

/**
 * strerror函数属于 string.h 头文件
 * strerror函数可以根据错误号返回错误原因字符串。
 *
 * char* strerror(int errnum);
 *  - errnum: 要转换的错误码;
 *
 *
 * 有时候错误码并不保存在errno中，例如pthread库函数的错误码都是通过返回值返回，而不改变errno的值，显然这种情况用strerror比perror方便。
 */

int main(int argc, char *argv[]) {
    printf("%s \n",strerror(2)); // No such file or directory
    return 0;
}
