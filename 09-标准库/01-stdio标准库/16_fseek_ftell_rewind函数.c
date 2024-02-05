#include <stdio.h>

/**
 * 当我们对一个文件进行操作时，内部会存在一个文件指针来指向当前操作的文件位置。
 * 比如打开一个文件后，默认从0开始，表示文件起始位置，调用一次fgetc后，指针会向后移动一个字节。
 *
 * 相关函数如下:
 *   - fseek 移动文件的读写位置;
 *   - ftell 获取当前读写位置;
 *   - rewind 将文件读写位置，重置回初始位置;
 *
 * 函数声明:
 *      int fseek(FILE* stream, long offset, int whence);
 *          - stream: 操作的文件句柄。
 *          - offset: 指针移动的偏移量，正值为向后移动，负值为向前移动。
 *          - whence: 从什么位置开始移动，可选值如下:
 *                  - SEEK_SET 从文件开头移动offset个字节。
 *                  - SEEK_CUR 从当前读写位置移动offset个字节。
 *                  - SEEK_END 从文件末尾移动offset个字节。
 *     通过该函数，可以任意移动文件指针的指向。
 *     成功返回0，出错返回-1，并且设置errno。
 *     如果向前移动的字节数超过了文件开头则出错返回。如果向后移动的字节数超过了文件末尾，再次写入时将增大文件尺寸，
 *     从原来的文件末尾到fseek移动之后的读写位置之间的字节都是0。
 *
 *      long ftell(FILE* stream);
 *          - stream: 要获取的文件句柄。
 *      成功返回当前读写位置，错误返回-1，并且设置errno;
 *
 *      void rewind(FILE* stream);
 *      回到文件头部;
 *
 * 最后还有一点要注意，常规文件都可以做Seek操作，而设备文件有很多是不支持Seek操作的，只允许顺序读写，比如对终端设备调用fseek会出错返回。
 */

int main(int argc, char *argv[]) {
    // 打开一个文件
    FILE* file = fopen("HELP.md","r");

    // 默认为 0，起始位置
    printf("%ld\n",ftell(file)); // 0

    // 读取一个字节
    fgetc(file);
    printf("%ld\n",ftell(file)); // 1

    // 再读取5个字节
    char bytes[5 + 1];
    fgets(bytes, sizeof(bytes),file);
    printf("%ld\n",ftell(file)); // 6

    // 现在我们不读取了，直接向后移动4个字节
    fseek(file,4,SEEK_CUR);
    printf("%ld\n",ftell(file)); // 10

    // 让我们回到文件起始位置
    fseek(file,0,SEEK_SET);
    printf("%ld\n",ftell(file)); // 0

    // 直接跳转到文件末尾位置，这样能够更快的直到该文件的大小
    fseek(file,0,SEEK_END);
    printf("%ld\n",ftell(file)); // 148


    fclose(file);
    return 0;
}
