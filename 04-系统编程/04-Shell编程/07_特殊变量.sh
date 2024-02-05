#!/bin/zsh


echo "共 $# 个命令行参数"

# 根据索引访问命令行参数
echo "第一个参数: $0"
echo "第一个参数: $1"
echo "第一个参数: $2"

# 遍历命令行参数 第一种方式
INDEX=1
while [ "$#" -gt 0 ]; do
    echo "第 $INDEX 个参数: $1"
    shift
done


# 遍历命令行参数 第二种方式
for ITEM in "$@" ; do
    echo "$ITEM"
done

