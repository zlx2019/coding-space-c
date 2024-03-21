# 一个最基本的 cmake 项目，配置如下:

# 1. 设置CMake使用的最低版本为 3.10
cmake_minimum_required(VERSION 3.10)

# 2. 创建项目，项目名为 hello_world
# C表示为C语言，CXX表示为C++语言
project(hello_world C)

# 3. 添加可执行文件，以及编译该文件需要用到的 源文件
add_executable(hello_world helloworld.c)


# 编译命令:
# cmake -S . -B ./bin
# -S: cmake 要编译的项目目录
# -B: 指定生成的目录
