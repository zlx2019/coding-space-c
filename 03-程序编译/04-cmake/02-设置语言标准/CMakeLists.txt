# 设置CMake最低版本
cmake_minimum_required(VERSION 3.10)

# 创建项目，以及采用的语言
project(hello, CXX)

# 设置编译时要用的C或C++的标准
set(CMAKE_CXX_STANDARD 11)
# 是否强制使用指定的标准，true表示强制，一旦不支持则会编译报错
# false 表示非强制的，如果不支持则使用上一个可用标准
set(CMAKE_CXX_STANDARD_REQUIRED true)

# 添加可执行程序
add_executable(hello hello.cpp)