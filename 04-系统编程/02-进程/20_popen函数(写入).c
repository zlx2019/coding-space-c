#include <stdio.h>
#include <stdlib.h>

/**
 * 通过 popen 函数执行 cat > 终端程序，并且获取到对应的标注输入，向文件中写入数据;
 */

int main() {
    // 执行 cat  > hello.txt 命令
    FILE* in = popen("cat > hello.txt","w");
    if(in == NULL){
        perror("popen failed:");
        exit(1);
    }
    fputc('A',in);
    // 关闭
    pclose(in);
    return 0;
}
