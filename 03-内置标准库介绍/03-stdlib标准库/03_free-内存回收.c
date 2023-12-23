#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * 通过 malloc分配的内存用完之后可以用free函数释放，更准确地说是归还给了malloc，下次调用malloc时这块内存可以再次分配出来。
 * 动态分配的内存用完之后可以调用free函数释放掉，传给free的参数正是先前malloc返回的内存块首地址。
 *
 * void free(void* ptr);
 *  - ptr: 要回收的内存首地址;
 *
 * 注意事项:
 * 1. malloc函数和free函数是一一对应的，调用了几次malloc就需要调用几次free来释放内存;
 * 2. 虽然内存耗尽是很不常见的错误，但编写程序要规范，malloc之后应该判断是否成功。以后要学习的大部分系统函数都有成功的返回值和失败的返回值，每次调用系统函数之后都应该判断是否成功。
 * 3. free(p);之后，p所指的内存空间是归还了，但是p的值并没有变，因为从free的函数接口来看根本没法改变p的值，p现在指向的内存空间已经不属于用户程序，
 *    换句话说，p成了野指针。为避免出现野指针，我们应该在free(p);之后手动设置p = NULL。
 */

// 定义一个结构体
typedef struct {
    int age;
    char* name;
} student;


int main(int argc, char *argv[]) {
    // 申请student内存
    student* stu = malloc(sizeof(student));
    if (stu == NULL){
        fprintf(stderr,"out of memory. \n");
        exit(1);
    }
    stu->age = 18;
    // 为student的name属性申请20个字节
    stu->name = malloc(20);
    if (stu->name == NULL){
        fprintf(stderr,"out of memory. \n");
        exit(1);
    }
    // 为name属性赋值
    strncpy(stu->name,"Hello,World!",11);

    printf("student{age: %d, name: %s} \n",stu->age,stu->name); // student{age: 18, name: Hello,World}

    // 释放name属性内存
    free(stu->name);
    stu->name = NULL;
    // 释放student对象内存
    free(stu);
    stu = NULL;
    return 0;
}

/**
 *  上面的例子只有一个简单的顺序控制流程，分配内存，赋值，打印，释放内存，退出程序。
 *  这种情况下即使不用free函数释放内存也可以，因为进程退出时它占用的所有内存都会被释放，也就是归还给了操作系统。
 *  但如果一个程序长年累月地运行（例如网络服务器程序），并且在循环或递归中调用malloc分配内存，则必须有free与之配对，分配一次就要释放一次，
 *  否则每次循环都分配内存，分配完了又不释放，就会慢慢耗尽系统内存，这种错误称为内存泄漏（Memory Leak）。
 *  另外，malloc返回的指针一定要保存好，只有把它传给free才能释放这块内存，如果这个指针丢失了，就没有办法释放这块内存了，也会造成内存泄漏。
 */