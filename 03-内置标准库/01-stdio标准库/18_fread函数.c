#include <stdio.h>
#include <stdlib.h>

/**
 * fgets函数有一定的局限性，它最多能从文件流中读取一行内容，遇到'\n'换行符就结束读取。
 * 如果想要一次性读取大量内容，可以使用fread函数，声明如下:
 *
 *   size_t fread(void* buf, size_t size, size_t count, *FILE stream);
 *      - buf: 要读取到的缓冲区首地址;
 *      - size: 每个数据块的大小;
 *      - count: 数据块的数量;
 *      - stream: 要读取的文件句柄;
 *
 *   fread函数表示，从stream中读取 size * count 个字节，写入到buf所指的内存空间中，并且返回成功读取到的数据块数量;
 *
 *   返回值: 是实际完成读写的数据块数，有可能小于count。
 *   例如调用fread时指定nmemb为2，而当前读写位置距文件末尾只有一条记录的长度，在这种情况下只读一条记录，返回1。
 *   如果当前读写位置已经到达文件末尾，则调用fread返回0。
 */


typedef struct {
    char name[10];
    int age;
} User;

int main(int argc, char *argv[]) {
    // 初始化数组
    User array[2];
    // 打开文件
    FILE* file = fopen("USER_FILE","r");
    if(file == NULL){
        perror("Open file USER_FILE:");
        exit(1);
    }
    // 读取数据
    size_t count = fread(array, sizeof(User), sizeof(array) / sizeof(User),file);
    if (ferror(file)){
        perror("Read file USER_FILE:");
        exit(1);
    }
    printf("读取成功，共读取 %ld 字节\n", sizeof(User) * count);

    printf("name: %s, age: %d \n",array[0].name,array[0].age);
    printf("name: %s, age: %d \n",array[1].name,array[1].age);

    fclose(file);
    return 0;
}
