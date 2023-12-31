#include <stdio.h>

/**
 * int remove(const char* name);
 *  - name： 要删除的文件路径
 */

int main(int argc, char *argv[]) {
    if(remove("USER_FILE") == 0){
        printf("删除成功\n");
    } else{
        perror("删除失败:");
    }
    return 0;
}
