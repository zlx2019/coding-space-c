#include <stdio.h>

/**
 * 我们经常用printf打印到屏幕，也用过scanf读键盘输入，这些也属于I/O操作，但不是对文件做I/O操作而是对终端设备做I/O操作。
 * 所谓终端（Terminal）是指人机交互的设备，也就是可以接受用户输入并输出信息给用户的设备。
 *
 * 每个进程启动时都会自动打开终端设备，并且使用三个 FILE* 指针，指向这个文件句柄:
 *  - stdin
 *  - stdout
 *  - stderr
 * 这三个文件指针是Iibc中定义的全局变量,在stdio.h中声明;
 *
 * 在stdio.h中声明，printf向stdout写，而scanf从stdin读，用户程序也可以直接使用这三个文件指针。
 * stdin、stdout和stderr的打开方式都是可读可写的;
 * 但通常stdin只用于读操作，称为标准输入（Standard Input）
 * stdout只用于写操作，称为标准输出（Standard Output），stderr也只用于写操作，称为标准错误输出（Standard Error）
 *
 */

int main(int argc, char *argv[]) {

    // 通过 printf()函数向标准输出(stdout) 输出内容
    // 也可以通过 fprintf()函数，向指定的文件句柄中输出内容，例如向 stdout 中输出
    printf("这是一句标准输出...\n"); // 这是一句标准输出...
    fprintf(stdout,"这也是一句标准输出... \n"); // 这也是一句标准输出...

    // 通过 fprintf() 向 stderr 输出内容
    fprintf(stderr,"这是一句标准错误输出... \n"); // 这是一句标准错误输出...

    // 通过 scanf() 获取标输入内容
    int num;
    printf("请输入一个整数: "); // 请输入一个整数: 18
    scanf("%d",&num);
    printf("您输入的是: %d",num); // 您输入的是: 18
    return 0;
}
