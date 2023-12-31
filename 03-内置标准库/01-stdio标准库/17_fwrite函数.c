#include <stdio.h>
#include <stdlib.h>

/**
 * fwrite函数可以一次性向一个文件流中，写入大量数据;
 * 该函数会按照数据块的形式来写入数据;
 *
 *      size_t fwrite(const void* buf, size_t size, size_t count, FILE* stream);
 *          - buf: 要写入的缓冲区;
 *          - size: 一个数据库的大小;
 *          - count: 要写入的数据块的数量;
 *          - stream: 要写入到的文件流;
 *
 *     该函数表示，从buf缓冲区中，读取 size * count 个字节，写入到stream中;
 *     写入成功后，返回写入的块数，也即count，如果小于count，则产生了错误;
 */


/**
 * 需求案例: 定义一个结构体，声明一个其类型数组，将其写入到一个文件中，再读取出来;
 */

typedef struct {
    char name[10];
    int age;
} User;

int main(int argc, char *argv[]) {
    // 定义结构体数组，数量为2，将该数组写入到一个文件中
    User array[2] = {{"张三",18},{"李四",22}};
    // 创建文件
    FILE* file = fopen("USER_FILE","w");
    if(file == NULL){
        perror("Open file USER_FILE:");
        exit(1);
    }

    // 将数组写入文件
    size_t count = fwrite(array, sizeof(User), sizeof(array) / sizeof(User),file);
    if (ferror(file)){
        // 写入错误处理
        perror("Write file USER_FILE:");
        exit(1);
    }
    printf("写入成功，共写入 %lu 个字节 \n",count * sizeof(User));
    fclose(file);
    return 0;
}
