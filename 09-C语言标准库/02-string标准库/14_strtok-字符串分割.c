#include <stdio.h>
#include <string.h>

/**
 * C标准库提供的strtok函数可以很方便地完成分割字符串的操作。tok是Token的缩写，分割出来的每一段字符串称为一个Token。
 *
 * 函数声明:
 *      char* strtok(char* str, const char* deIim);
 *          - str: 要分割的字符串，首次调用时传入即可，继续分割直接传入NULL;
 *          - deIim: 分隔符
 * 返回值: 每次调用，依次返回字符串str中的一个Token，直到遇到'\0'符号，返回NULL，表示分割结束;
 * 首次调用，会返回 str首字符 ~ 下一个deIim中间的子串，也就是分隔号的第一个Token,
 * 后续继续分割，切记不要再传入str字符串,直接传入NULL和分隔符，会获取到后续的Token，直到分割结束，返回NULL;
 *
 * 执行原理:
 *      第一次调用时要传字符串的首地址给strtok的第一个参数，以后每次调用只要传NULL给第一个参数就可以了，
 * strtok函数自己会记住上次处理到字符串的什么位置（显然这是通过strtok函数里的一个静态指针变量记住的）。
 * str字符串被strtok不断修改，每次调用strtok把str中的一个分隔符改成'\0'，分割出一个小字符串，并返回这个小字符串的首地址。
 * 在做了分割之后，字符串中的分隔符就被'\0'覆盖了。
 *
 * 注意：
 *  - 该函数不能用于分割字符串字面值，因为试图改写.rodata段会产生段错误。
 *  - strtok函数使用了静态变量，它不是线程安全的，必要时应该用可重入的strtok_r函数。
 *
 * 总得来说 C语言中的字符串分割，相对来说较为麻烦，后续我们可以封装一个简单的实现函数，下面介绍分割一个字符串的详情:
 */

int main(int argc, char *argv[]) {
    // 要分割的字符串
    char text[] = "hello,hi,why,like";
    // 分隔符
    char* sep = ",";
    // 用于保存分割的每个Token
    char* token;

    // 首次分割，返回 'hello' 部分
    token = strtok(text,sep);
    printf("%s \n", token); // hello

    // 此时在strtok函数内部，text指针已经指向了'h'字符

    // 再次分割, 从'h'字符开始截取，截取到下一个 sep 分隔符前一个字符，也就是 'hi'
    token = strtok(NULL,",");
    printf("%s \n", token); // hi

    // 继续分割
    token = strtok(NULL,",");
    printf("%s \n", token); // why
    token = strtok(NULL,",");
    printf("%s \n", token); // like

    // 继续分割，由于到了字符串末尾，返回NULL
    token = strtok(NULL,",");
    printf("%s \n", token); // (null)
    return 0;
}
