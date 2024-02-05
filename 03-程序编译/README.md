# C/C++的编译

## C/C++的常见编译器

常见的编译器有如下几种:

- gcc/g++
- visual c++
- clang

不同的计算机平台默认集成的编译器可能有所不同，例如Linux通常会集成gcc/g++，而mac集成clang。

## C的编译流程

C/C++的编译分为两大阶段，分别为**编译**和**链接**，而编译又分为三个小阶段: `预处理`、`编译`、`汇编`。

1. **编译**

   1.1 `预处理`

   1.2 `编译`

   1.3 `汇编`

2. **链接**

C和C++的编译非常相似，我们使用C语言作为案例，下面提供了一个非常简单的C程序:

定义`hello.h`头文件:

```c
// hello.h

#define CODE 0

int add(int,int);
```

定义`hello.c`源文件:

```c
// hello.c

#include <hello.h>

int main(){
    int sum = add(3,7);
    return CODE;
}

int add(int n1, int n2){
    return n1 + n2;
}
```

### 阶段一: 编译

该阶段是指将C的源文件(.c)，编译成二进制的目标文件(.o)，以上面的代码为例，其实就是将`hello.c`编译为`hello.o`目标文件;

编译命令:

```sh
$ gcc -c  -I./	-o hello.o hello.c
```

`-c`: 表示将源文件编译成二进制目标文件;

`-I`: 表示指定头文件目录，因为示例中我们使用的`<>`符号引入的自定义头文件，所以需要指定目录，`-I./` 表示当前目录为头文件检索目录;

`-o`: 表示输出文件;

执行完成后，该目录会多出一个`hello.o`目标文件，每个源文件编译后都会生成一个对应目标文件。

下面我们将这个步骤再次拆分为三个小阶段.

#### 1. 预处理

预处理阶段的主要作用有如下:

- 将所有通过`#include`宏引入的头文件的内容，拷贝到当前源文件中;
- 将源文件中所有使用了`#define`宏定义的地方，全部替换为具体的值;

预处理命令:

```sh
$ gcc -E -I./  -o hello.i hello.c
```

`-E`: 表示编译器在预处理之后就退出，不进行后续编译过程;

预处理完成后，当前目录会多出一个`hello.i`文件，该文件就是经过预处理后的源文件，内容如下:

```c
// hello.i

...
  
int add(int,int);
# 2 "hello.c" 2

int main(){
    int sum = add(3,7);
    return 0;
}

int add(int n1, int n2){
    return n1 + n2;
}
```

#### 2. 编译

这个**编译**阶段不是指程序从源文件到二进制程序的全部过程，而是将**预处理**后的源文件，编译成特定的汇编代码的过程。

编译命令:

```sh
$ gcc -S -o hello.s hello.i
```

`-S`: 指示编译器生成汇编代码而不进行汇编和链接步骤。

编译完成后，会得到一个`hello.s`汇编文件.

```assembly
 .file   "hello.c"
        .text
        .globl  main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        subq    $16, %rsp
        movl    $7, %esi
        movl    $3, %edi
        call    add
        movl    %eax, -4(%rbp)
        movl    $0, %eax
        leave
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   main, .-main
        .globl  add
        .type   add, @function
add:
.LFB1:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    %edi, -4(%rbp)
        movl    %esi, -8(%rbp)
        movl    -8(%rbp), %eax
        movl    -4(%rbp), %edx
        addl    %edx, %eax
        popq    %rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
```

也可以直接将未经过**预处理**的源文件，直接编译成汇编代码:

```sh
$ gcc -S -I./ -o hello.s hello.c
```

#### 3. 汇编

汇编阶段是指将上一步生成的汇编代码文件翻译成二进制机器码(机器语言)，这一步产生的文件叫做**目标文件**，是二进制格式的。

汇编命令:

```sh
$ gcc -c -o hello.o hello.s
```

`gcc`汇编过程通过`as`命令完成，等价于如下命令:

```sh
$ as -c hello.s -o hello.o
```

这一步会为每一个源文件产生一个目标文件。

### 阶段二: 链接

**链接**阶段主要将多个目标文件以及所需要使用到的静态链接库(.a .lib等)链接成最终的可执行文件(executable file)。

链接命令:

```sh
$ gcc hello.o -o hello
```

链接完成后，会得到一个名为`hello`的可执行文件;

### 总结

经过以上分析，我们发现编译过程并不像想象的那么简单，而是要经过预处理、编译、汇编、链接。尽管我们平时使用`gcc`命令的时候没有关心中间结果，但每次程序的编译都少不了这几个步骤。

但是通常我们不会选择这么麻烦的方式，而是直接通过一个命令即可完成这所有步骤:

```sh
$ gcc -I./ -o hello hello.c
```

## 静态库和动态库

在编程的过程中，库是一种已经封装好，可以进行复用的代码。本质上来说**库是一种可执行代码的二进制形式**，可以被操作系统加载到内存中执行。

你可以这样理解: 就类似于我们通过源文件编写的函数一样，我们将无数个这样的函数归纳到一起，形成了一个函数库，不同的是需要将这个函数库压缩成二进制形式。

库共分为两种: **静态库**和**动态库**;

- Windows系统: 静态库文件后缀为`.lib`,动态库文件后缀为`.dll`。
- Linux系统: 静态库文件后缀为`.a`，动态库文件后缀为`.so`。

### 静态库

静态库又称为**静态链接库**，其含义就是在程序**链接阶段**，会将**汇编阶段**生成的**目标文件(.o)**与引用的库一起链接打包到可执行文件内部。因此对应的链接方式称为**静态链接**。

静态库与汇编生成的目标文件一起链接为可执行文件，那么静态库必定跟.o文件格式相似。其实一个静态库可以简单看成是**一组目标文件(.o)的集合**，即很多目标文件经过压缩打包后形成的一个文件。

静态库的特点:

- 静态库对函数库的链接是放在编译时期完成的。
- 程序在运行时与函数库再无瓜葛，移植方便。
- 浪费空间和资源，因为所有相关的目标文件与牵涉到的函数库被链接合成一个可执行文件。

#### 生成静态库

使用Linux系统为例，创建静态库并且使用。

Linux静态库命名规范，必须是`lib[xxx].a`：lib为前缀，中间是静态库名，扩展名为.a。

静态库的生成流程:

1. 首先完成编译的第一大阶段**编译阶段**，生成对应的目标文件(.o);
2. 通过`ar`工具，将生成的目标文件打包成静态库文件;

定义源文件`kmath.c`

```c
// kmath.c

int add(int n1, int n2){
  return n1 + n2;
}
```

生成目标文件

```sh
$ gcc -c -o kmath.o kmath.c
```

打包为静态库

```sh
$ ar -crv libkmath.a kmath.o
```

执行完该命令后，将会得到一个`libkmath.a`静态库文件;

下面是`ar`的参数描述

| 参数 |                        意义                         |
| :--: | :-------------------------------------------------: |
|  -r  | 将objectfile 文件插入静态库尾或替换静态库中同名文件 |
|  -x  |            从静态库文件中抽取文件objfile            |
|  -t  |              打印静态库的成员文件列表               |
|  -d  |              从静态库中删除文件objfile              |
|  -s  |                 重置静态库文件索引                  |
|  -v  |                  创建文件冗余信息                   |
|  -c  |                   创建静态库文件                    |

#### 使用静态库

编写一个源文件来使用刚刚生成的静态库:

```c
// hello.c

#include <stdio.h>

// 声明要调用的静态库函数
extern int add(int n1, int n2);

int main(int argc, char *argv[]) {
    int sum = add(10,20);
    printf("%d\n",sum);
    return 0;
}
```

然后将生成的`libkmath.a`静态库拷贝到当前目录(当然你可以拷贝到任意目录，拷贝过来主要是方便指定路径)。

接下来对该源文件进行编译打包为可执行文件:

```sh
$ gcc -L ./ -l kmath -o hello hello.c
```

`-L`: 表示指定要链接的静态库所在的目录，在这里也就是指定`libkmath.a`文件的所在目录;

`-l`: 表示指定要链接的静态库，编译器在指定目录查找动态库时，会自动加上对应的前缀与后缀，所以只需要指定库名称的核心部分即可;

最终运行打包好的可执行程序:

```sh
$ ./hello
30
```

### 动态库

动态库在程序链接时并不会打包到可执行文件内，而是在**程序运行过程中**动态的加载调用，因此称为**动态链接库**。

不同的进程如果调用同一个动态库，那么在内存中也只会存在一份该动态库的示例，所以又称为共享库，这样规避了空间浪费的问题。

特点:

- 动态库把对一些库函数的链接载入推迟到程序运行的时期。

- 可以实现进程之间的资源共享(因此动态库也称为共享库)。
- 将一些程序升级变得简单。
- 可以做到链接载入完全由程序员在程序代码中控制(显示调用)。

比如我们通常使用的C语言标准库，大部分都是以动态链接库的形式存在，例如`stdio.h`、`stdlib.h`等头文件，都是需要动态链接`libc`共享库，而这些共享库通常都是放在系统默认的库目录中，编译器会自动去这些目录中寻找动态库。

#### 生成动态库

依然使用Linux系统为例，创建动态库并且使用。

Linux 动态链接库的命名规范: `lib[xxx].so`，lib是前缀，中间是动态库名称,`.so`是后缀;

动态库生成流程:

1. 编译源文件，生成目标文件，注意: 需要加上 `-fPIC` 参数;
2. 将目标文件打包为动态库;

定义源文件:

```c
// kmath.c

int add(int n2, int n2){
  return n1 + n2;
}
```

生成目标文件:

```sh
$ gcc -c -fPIC -o kmath.o kmath.c
```

`-fPIC`: 表示创建与地址无关的编译程序（pic，position independent code），是为了能够在多个应用程序间共享。

接下来打包为动态库:

```sh
$ gcc -shared -o libkmath.so kmath.o
```

或者直接将源文件打包为动态库:

```sh
$ gcc -fPIC -shared -o libkmath.so kmath.c
```

`-shared`: 表示生成动态库;

#### 使用动态库(方式一)

此方式和使用静态链接库的方式非常相似

定义源文件:

```c
// hello.c

#include <stdio.h>

// 声明要调用的静态库函数
extern int add(int n1, int n2);

int main(int argc, char *argv[]) {
    int sum = add(10,20);
    printf("%d\n",sum);
    return 0;
}
```

编译运行:

```sh
$ gcc -L ./ -l kmath -o hello hello.c
$ ./hello
30
```

#### 使用动态库(方式二)

动态库也可以使用纯代码形式进行调用，如下所示:

```c
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

#define LIB "./libkmath.so"

int main(int argc, char *argv[]) {

    // 加载动态链接库
  	// 参数为: 要加载的动态库路径，打开模式
    void* math_dl = dlopen(LIB,RTLD_LAZY);
    if(math_dl == NULL){
        perror("load kmath lib failed:");
        exit(1);
    }
  
    // 获取动态库中的 `add` 函数地址
    int (*add)(int,int) = dlsym(math_dl,"add");
  
    // 调用函数
    int sum = add(100,200);
    printf("%d \n",sum);

    // 释放动态链接库
    dlclose(math_dl);
    return 0;
}
```

注意`LIB`宏定义的 动态链接库的路径。

编译运行: 

```sh
$ gcc -o hello hello.c
$ ./hello
300
```

