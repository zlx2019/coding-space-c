#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    // 打开一个文件
    int file_fd = open("hello.txt",O_RDWR);
    if(file_fd < 0){
        perror("open file");
        exit(1);
    }

    // 要映射的内存长度，只映射5个字节
    int len = 5;
    // 起始偏移量，0表示从文件内容的头部开始
    int offset = 0;

    // 通过mmap 对打开的文件建立内存映射
    char* content = mmap(NULL,len,PROT_READ | PROT_WRITE,MAP_SHARED, file_fd, offset);
    if (content == MAP_FAILED){
        // 内存映射失败
        perror("mmap failed");
        exit(1);
    }
    // 获取到了映射内容的起始内存地址，接下来就可以像操作内存一样来修改内容

    // 1. 读取映射到的内容
    char buf[len + 1];
    strncpy(buf,content,len);
    buf[len] = '\0';
    printf("%s\n",buf); // Hello

    // 2. 修改映射的内容
    strncpy(content,"HELLO",len);

    // 3. 再尝试读取映射内容
    printf("%s\n", strncpy(buf,content,len)); // HELLO

    // 3. 取消映射
    munmap(content,len);
    return 0;
}
