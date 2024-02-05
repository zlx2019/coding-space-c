#include <stdio.h>
#include <stdlib.h>

#define KEY "ROOT_NAME"

int main() {
    // 设置环境变量
    int no = setenv(KEY,"张三",1);
    if (no != 0){
        perror("set env failed");
        exit(no);
    }

    // 获取环境变量
    const char* name = getenv(KEY);
    printf("%s\n",name); // 张三

    // 删除环境变量
    no = unsetenv(KEY);
    if (no != 0){
        perror("remove env failed");
        exit(no);
    }

    printf("%s\n", getenv(KEY)); // (null)
    return 0;
}
