//#include <stdio.h>

// 引入非标准库，编译时请手动链接此库  -lncurses
#include <ncurses.h>

/**
 * 游戏的整个布局由一个int类型的二维数组构成，元素分别表示几种物体:
 *  - 空地 0
 *  - 墙体 1
 *  - 箱子目的地 2
 *  - 箱子 3
 *  - 玩家 4
 */


/// 全局宏定义
// 游戏元素的各项值
#define EMPTY    0   // 空地
#define COLLIDER 1   // 墙体
#define TARGET   2   // 箱子目的地
#define BOX      3   // 箱子
#define PLAYER   4   // 玩家


#define ROW     10 // 地图的行数
#define COL     10 // 地图的列数

/// 将地图信息打印到终端
void print_map(int[ROW][COL]);

/// 角色移动
void player_move();


int main(int argc, char *argv[]) {
    // 初始化ncurses
    initscr();
    printf("\n");

    int i = getch();

    // 初始化地图
    int map[ROW][COL] =
            {
                0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
                0, 0, 0, 1, 2, 0, 1, 0, 0, 0,
                0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
                1, 1, 1, 1, 3, 0, 1, 1, 1, 1,
                1, 2, 0, 3, 0, 4, 0, 3, 2, 1,
                1, 1, 1, 1, 1, 3, 0, 1, 1, 1,
                0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
                0, 0, 0, 0, 1, 2, 1, 0, 0, 0,
                0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
    // 打印地图
    print_map(map);

    return 0;
}


void player_move(){
    // 获取键盘事件
    int keyCode = getch();
    printf("%d",keyCode);
}

/**
 * 将地图信息打印到终端
 * @param map 地图
 */
void print_map(int map[ROW][COL]) {
    for (int x = 0; x < ROW; x++) {
        for (int y = 0; y < COL; y++) {
            // 渲染地图，根据不同的元素值，输出不同的字符
            switch (map[x][y]) {
                case EMPTY:
                    // 空地
                    printf("  ");
                    break;
                case PLAYER:
                    // 玩家
                    printf("♀ ");
                    break;
                case COLLIDER:
                    // 墙体
                    printf("▩ ");
                    break;
                case TARGET:
                    // 箱子目的地
                    printf("♡ ");
                    break;
                case BOX:
                    printf("☒  ");
                    break;
            }
        }
        printf("\n");
    }
}

