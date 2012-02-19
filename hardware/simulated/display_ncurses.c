#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "maze.h"

void init_display(void) {
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
}
void finalize_display(void) {
    sleep(4);
    endwin();
}
void display_maze(struct cell maze[WIDTH][HEIGHT], 
                  struct point pos, direction_t current_direction,
                  bool fast) {
    static bool fast_visited[WIDTH][HEIGHT] = {{0}};
    if (fast)
        fast_visited[pos.x][pos.y] = true;
    int8_t i = 0;
    int8_t j = 0;
    int8_t k = HEIGHT-1;
    mvprintw(0, 0, " ");
    for (j = 0; j < WIDTH; j++) {
        mvprintw(0, 4*j+1, "___ ");
    }   

    char arrow[] = {'^', 'v', '>', '<'};
    for (i = 0; i < HEIGHT; i++) {
        mvprintw(2*i+1, 0, "|");
        mvprintw(2*i+2, 0, "|");
        for (j = 0; j < WIDTH; j++) {
            if (maze[j][k].visited)
                attron(A_STANDOUT);
            if (fast_visited[j][k])
                attron(COLOR_PAIR(1));
            mvprintw(2*i+1, 4*j+1, " %c %c",
                j == pos.x && k == pos.y ? current_direction[arrow] : ' ' ,
                maze[j][k].path[EAST] ? ' ' : '|');

            mvprintw(2*i+2, 4*j+1, "%s%c", maze[j][k].path[SOUTH] ? "   " : "___",
                maze[j][k].path[EAST] ?  ' ': '|');
            attroff(A_STANDOUT);
            attroff(COLOR_PAIR(1));
        }
        k--;
    }
    mvprintw(2*HEIGHT+1, 0, "\n");

    refresh();
    usleep(5000);
}
