#include <stdio.h>

/**
 * 通过循环把数组中的每个元素依次访问一遍，在计算机术语中称为遍历（Traversal）。
 */

int main(int argc, char *argv[]) {
    int counts[4] = {1,2,3,4};
    for(int i = 0; i < 4; i++){
        printf("%d ,",counts[i]);
    }
    return 0;
}

// 1 ,2 ,3 ,4 ,
