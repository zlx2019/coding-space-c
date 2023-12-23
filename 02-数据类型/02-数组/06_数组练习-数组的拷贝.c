#include <stdio.h>

/**
 * 1.编写一个程序，定义两个类型和长度都相同的数组，将其中一个数组的所有元素复制给另一个数组。
 * 既然数组不能直接赋值，想想应该如何实现。
 */

/**
 * 将source数组的元素，拷贝到target中
 */
void array_copy(int target[], int source[],size_t length){
    for (int i = 0; i < length; i++){
        target[i] = source[i];
    }
}

void print_array(int arr[], size_t length){
    for (int i = 0; i < length; i++){
        printf("%d",arr[i]);
        if(i != length - 1){
            printf(",");
        } else{
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]) {
    int a1[5] = {1,2,3,4,5};
    int a2[5] = {};
    print_array(a1,5); // 1,2,3,4,5
    print_array(a2,5); // 0,0,0,0,0

    array_copy(a2,a1,5);
    print_array(a1,5); // 1,2,3,4,5
    print_array(a2,5); // 1,2,3,4,5
    return 0;
}
