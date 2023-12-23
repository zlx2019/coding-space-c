#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * 
 */
int main(int argc, char *argv[]) {
    // 参数校验
    if(argc < 3){
        fprintf(stderr,"Usage: %s <要复制的文件> <目标目录>\n",argv[0]);
        exit(1);
    }
    // 源文件
    const char* source_file_name = argv[1];
    // 目标目录
    const char* target_dir = argv[2];

    // 校验源文件是否存在
    if (access(source_file_name,F_OK) == -1){
        fprintf(stderr,"%s 文件不存在! \n",source_file_name);
        exit(1);
    }
    // 校验目标目录是否存在
    if (access(target_dir,F_OK) == -1){
        // 目录不存在
        fprintf(stderr,"%s 目录不存在! \n",target_dir);
        exit(1);
    }else if(access(target_dir,X_OK) == -1){
        // 存在，但不是目录
        fprintf(stderr,"%s 不是一个目录! \n",target_dir);
        exit(1);
    }

    // 打开源文件
    FILE* s_file = fopen(source_file_name,"r");
    // TODO





//    struct stat fileInfo;
//    if(stat("/usr/local/myproject/a.tat",&fileInfo) == 0){
//        if(S_ISDIR(fileInfo.st_mode)){
//            printf("是一个目录");
//        }else{
//            printf("不是一个目录");
//        }
//    }
    return 0;
}
