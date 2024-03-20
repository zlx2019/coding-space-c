//
// Created by Zero on 2024/3/18.
//

#ifndef LEARNS_LOG_H
#define LEARNS_LOG_H


#define INFO_COLOR "\033[0;32m"
#define DEBUG_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define COLOR_END "\033[0m"

#define INFO(format, ...) fprintf(stdout, INFO_COLOR"[INFO] " format "\n" COLOR_END, ##__VA_ARGS__)
#define DEBUG(format, ...) fprintf(stdout, DEBUG_COLOR"[Debug] " format "\n" COLOR_END, ##__VA_ARGS__)
#define ERROR(format, ...) fprintf(stderr, ERROR_COLOR"[Error] " format "\n" COLOR_END, ##__VA_ARGS__)

#endif //LEARNS_LOG_H
