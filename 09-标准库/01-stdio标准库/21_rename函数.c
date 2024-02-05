#include <stdio.h>

/**
 * int rename(const char* oldName, const char* newName);
 *  - oldName: 旧文件名称
 *  - newName: 新文件名称
 * 修改成功返回1，失败返回0;
 *
 */

int main(int argc, char *argv[]) {
    if(rename("USER_FILE","NEW_USER_FILE") == 0){
        printf("修改成功\n");
    } else{
        perror("修改失败: ");
    }
    return 0;
}
