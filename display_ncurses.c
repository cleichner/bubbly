#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "maze.h"

void init_display(void) {
    initscr();
}
void finalize_display(void) {
    endwin();
}
void display_maze(struct cell maze[WIDTH][HEIGHT], 
                  struct point pos, direction_t current_direction) {
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
            mvprintw(2*i+1, 4*j+1, " %c %c",
                j == pos.x && k == pos.y ? current_direction[arrow] : ' ' ,
                maze[j][k].path[EAST] ? ' ' : '|');

            mvprintw(2*i+2, 4*j+1, "%s%c", maze[j][k].path[SOUTH] ? "   " : "___",
                maze[j][k].path[EAST] ?  ' ': '|');
        }
        k--;
    }
    mvprintw(2*HEIGHT+1, 0, "\n");

    refresh();
    usleep(250000);
}
