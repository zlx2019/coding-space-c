# 1 "hello.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 418 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "hello.c" 2
# 1 "./hello.h" 1


int add(int,int);
# 2 "hello.c" 2

int main(){
    int sum = add(3,7);
    return 0;
}

int add(int n1, int n2){
    return n1 + n2;
}
