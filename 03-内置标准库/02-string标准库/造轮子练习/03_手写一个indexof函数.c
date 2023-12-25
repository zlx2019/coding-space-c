#include <stdio.h>


/**
 * 在一个字符串中，搜索指定字符首次出现的位置索引;
 *
 * @param string    字符串，如果没有结束符，可能会程序崩溃
 * @param character 要搜索的字符
 * @return          字符首次出现位置的索引，如果没找到则返回-1
 */
int k_str_index(const char* string, int character){
    int index = 0;
    while (1){
        char cur_c = *string++;
        if(cur_c == '\0'){
            return -1;
        }
        if(cur_c == character){
            return index;
        }
        index++;
    }
}

/**
 * 在一个字符串中，搜索指定字符最后一次出现的位置索引;
 *
 * @param string    字符串，如果没有结束符，可能会程序崩溃
 * @param character 要搜索的字符
 * @return          字符最后一次出现位置的索引，如果没找到则返回-1
 */
int k_strLastIndex(const char* string, int character){
    int cur_index = -1;
    for (int i = 0; 1; i++){
        char cur_c = *string++;
        if(cur_c == '\0'){
            return cur_index;
        }
        if(cur_c == character){
            cur_index = i;
        }
    }
}

int main(int argc, char *argv[]) {
    int index = k_str_index("hello",'l');
    printf("%d \n",index); // 2
    int index2 = k_str_index("hello",'k');
    printf("%d \n",index2); // -1


    int index3 = k_strLastIndex("hello",'l');
    printf("%d \n",index3); // 3
    int index4 = k_strLastIndex("hello",'k');
    printf("%d \n",index4); // -1
    return 0;
}
