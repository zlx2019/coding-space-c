#include <stdio.h>
/**
 * 一些获取文件信息的宏定义:
 *    __FILE__ 获取当前源文件的绝对路径和名称
 *    __FILE_NAME__ 获取源文件名
 *    __LINE__ 获取该行代码在文件中的行号
 *    __func__ 获取函数名
 *    __VA_ARGS__ 宏的可变参数列表
 *    ##__VA_ARGS__ 表示允许在调用宏时省略额外参数。如果调用时没有传入额外参数，这个部分会被预处理器移除。
 *
 * 通过这些宏定义，实现一些标准输出宏函数
 */

// 终端字体颜色
#define INFO_COLOR "\033[0;32m"
#define DEBUG_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define COLOR_END "\033[0m" // 恢复默认

// 输出格式: [日志级别] [源文件路径:行号 - 函数名()] 输出内容
#define AllLog(format, ...) fprintf(stdout, "[INFO][%s:%d - %s()] " format "\n" , __FILE__, __LINE__, __func__ ,##__VA_ARGS__)


// Info
#define Log(format, ...) fprintf(stdout, INFO_COLOR"[INFO] " format "\n" COLOR_END, ##__VA_ARGS__)
// Debug
#define Debug(format, ...) fprintf(stdout, DEBUG_COLOR"[Debug] " format "\n" COLOR_END, ##__VA_ARGS__)
// Error
#define Error(format, ...) fprintf(stderr, ERROR_COLOR"[Error] " format "\n" COLOR_END, ##__VA_ARGS__)