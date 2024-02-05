#include <stdio.h>

/**
 * fgets函数用于从一个文件流中读取一行或者n个字节的内容。
 *  char* fgets(char* buf, int size, FILE* stream);
 *      - buf: 要读取到的缓冲区
 *      - size: 缓冲区的大小
 *      - stream: 要读取的文件句柄
 *  参数 buf 是缓冲区首地址，size 是缓冲区长度，该函数从 stream 所指的文件中读取以'\n'结尾的一行（包括'\n'在内）存到缓冲区 buf 中，
 * 并且在该行末尾加上一个'\0'组成完整的字符串。如果文件中的一行太长，fgets从文件中读了size-1个字符还没读到'\n'，
 * 就把已经读到的size-1个字符再加上一个'\0'存入缓冲区并返回，文件中剩下的半行可以在下次调用fgets时继续读。
 * 如果fgets读到文件末尾的最后几个字符，不管是不是以'\n'结尾都加上一个'\0'存入缓冲区并返回。下次再调用fgets时读写位置已经到达文件末尾，则返回NULL。
 *
 * gets函数用于从标准输入中读取一行或者n个字节的内容。
 *  char* gets(char* buf);
 *      - buf: 要读取到的缓冲区首地址
 *  等同于 fgets(stdin,&buf);
 */


/**
 * 下面案例通过fgets，来读取一个文件的内容，并且输出到标准输出。
 */
int main(int argc, char *argv[]) {
    // 打开文件
    FILE* file = fopen("HELP.md","r");

    // 定义一个缓冲区，每次最多只能读取20个字节
    char line[20];

    // 循环读取文件内容
    while ((fgets(line, sizeof(line),file) != NULL)){
        fprintf(stdout,"%s",line);
    }
    // 关闭文件
    fclose(file);
    return 0;
}


/**
 * 细节补充:
 *      对于fgets来说，'\n'是一个特别的字符，而'\0'并无任何特别之处，如果读到'\0'就当作普通字符读入。
 *      如果文件中存在'\0'字符（或者说字节0），调用fgets之后就无法判断缓冲区中的'\0'究竟是从文件读上来的还是由fgets自动添加的，
 *      所以fgets只适合读文本文件而不适合读二进制文件，并且文本文件中的所有字符都应该是可见字符，不能有'\0'。
 */