# cmake
## cmake是什么?

## 一个最简单cmake项目
1. 编写源文件`helloworld.c`
```c
#include <stdio.h>

int main(){
    printf("hello world! \n");
    return 0;
}
```

2. 编写`CMakeLists.txt`配置文件

```cmake
# 1. 设置CMake使用的最低版本为 3.10
cmake_minimum_required(VERSION 3.10)

# 2. 创建项目，项目名为 hello_world
# C表示为C语言，CXX表示为C++语言
project(hello_world C)

# 3. 添加可执行文件，以及编译该文件需要用到的 源文件
add_executable(hello_world helloworld.c)
```

3. cmake编译

在当前目录下创建一个目录，存放cmake的编译输出。

```shell
mkdir bin
tree .
├── CMakeLists.txt
├── bin
└── helloworld.c
```

接下来通过cmake进行编译:

```shell
cmake -S . -B ./bin
```

- `-S`: 指定要编译的cmake项目目录。
- `-B`: 指定输出的目录。

编译成功后，在输出目录中会有一个`Makefile`文件，该文件就是用来真正编译可执行程序的。

4. 编译程序

```shell
$ cd bin && make
[ 50%] Linking C executable hello_world
[100%] Built target hello_world

$ ./hello
hello world! 
```

