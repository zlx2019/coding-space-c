#include <stdio.h>
#include <string.h>

/**
 * char ＊strchr(const char ＊s, int c);
 * char ＊strrchr(const char ＊s, int c);
 * - s: 搜索的目标字符串
 * - c: 要搜索的字符
 * strchr 表示从字符串头部向后搜索，找出c在s中首次出现的位置;
 * strrchr 表示从字符串末尾向前搜索，也就是找c在s中最后一次出现的位置;
 * 返回值：如果找到字符c，返回字符串s中指向字符c的指针，如果找不到就返回NULL
 */

int main(int argc, char *argv[]) {
    const char* text = "hello";
    if(strchr(text,'l')){
        printf("text中存在 l 字符;\n");
    }
    if(strchr(text,'k') == NULL){
        printf("text中不存在 k 字符;\n");
    }

    // text中存在 l 字符;
    // text中不存在 k 字符;
    return 0;
}
