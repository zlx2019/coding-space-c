#include <hello.h>

int main(){
    int sum = add(3,7);
    return CODE;
}

int add(int n1, int n2){
    return n1 + n2;
}

// 预处理命令:
// gcc -E -I./  -o hello.i hello.c