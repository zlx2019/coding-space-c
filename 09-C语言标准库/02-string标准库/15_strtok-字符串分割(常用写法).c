#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // 分割字符串的常用写法

    // 要分割的字符串
    char text[] = "hello,hi,why,like";
    // 分隔符
    char* sep = ",";
    // 用于保存分割的每个Token
    char* token;

    // 开始分割
    token = strtok(text,sep);
    printf("%s \n",token);
    // 循环分割,直到结束
    while ((token = strtok(NULL,sep)) != NULL){
        printf("%s \n",token);
    }
    // hello
    // hi
    // why
    // like
    return 0;
}
