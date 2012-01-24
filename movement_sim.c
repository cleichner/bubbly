#include <assert.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "maze.h"
#include "movement.h"

#define CHAR_WIDTH 2*WIDTH+1
#define CHAR_HEIGHT HEIGHT+1

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream);
static void make_graph(struct cell maze[WIDTH][HEIGHT],
                       char chars[CHAR_WIDTH][CHAR_HEIGHT]);
//static void print_edges(struct cell maze[WIDTH][HEIGHT]);
static void display_maze(void);

static void accept(bool (*accept_f)(char), char* name, char* dest, char cur);
static bool space(char cur) { return cur == ' ' || cur == '\t'; }
static bool underscore(char cur) { return cur == '_'; }
static bool newline(char cur) { return cur == '\n'; }
static bool pipe_char(char cur) { return cur == '|'; }
static bool underscore_or_space(char cur) {
    return underscore(cur) || space(cur);
}
static bool pipe_char_or_space(char cur) { return pipe_char(cur) || space(cur); }
static void ignore_trailing_whitespace(FILE* stream);

static struct cell maze[WIDTH][HEIGHT];
static int8_t x_pos = 0;
static int8_t y_pos = 0;
static abs_direction current_direction = NORTH;

void initialize_movement(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s maze-file\n", argv[0]);
        exit(1);
    }

    char chars[CHAR_WIDTH][CHAR_HEIGHT];
    FILE* stream = fopen(argv[1], "r");
    assert(stream && "Maze-file could not be opened for reading.\n");

    parse_maze_file(chars, stream);
    fclose(stream);

    init_maze(maze);
    make_graph(maze, chars);

    // print_edges(maze);
    initscr();
    display_maze();
}

void finalize_movement(void) {
    endwin();
}

void move_forward(int8_t n) {
    if (current_direction == NORTH) {
        if (!(y_pos + n >= HEIGHT) && maze[x_pos][y_pos].north)
            y_pos += n;
        else
            assert(false && "y_pos >= HEIGHT or tried to move through a wall");
    } else if (current_direction == SOUTH) {
        if (!(y_pos - n < 0) && maze[x_pos][y_pos].south)
            y_pos -= n;
        else
            assert(false && "y_pos < 0 or tried to move through a wall");
    } else if (current_direction == EAST) {
        if (!(x_pos + n >= WIDTH) && maze[x_pos][y_pos].east)
            x_pos += n;
        else
            assert(false && "x_pos >= WIDTH or tried to move through a wall");
    } else if (current_direction == WEST) {
        if (!(x_pos - n < 0) && maze[x_pos][y_pos].west)
            x_pos -= n;
        else
            assert(false && "x_pos < 0 or tried to move through a wall");
    } else {
        assert(false && "Unknown direction");
    } 
    display_maze();
}

void rotate_left(void) {
    if (current_direction == NORTH)
        current_direction = WEST;
    else if (current_direction == WEST)
        current_direction = SOUTH;
    else if (current_direction == SOUTH)
        current_direction = EAST;
    else if (current_direction == EAST)
        current_direction = NORTH;
    else
        assert(false && "Unknown direction");
    display_maze();
}

void rotate_right(void) {
    if (current_direction == NORTH)
        current_direction = EAST;
    else if (current_direction == WEST)
        current_direction = NORTH;
    else if (current_direction == SOUTH)
        current_direction = WEST;
    else if (current_direction == EAST)
        current_direction = SOUTH;
    else
        assert(false && "Unknown direction");
    display_maze();
}

bool has_wall(direction_t direction) {
    assert(false && direction &&"has_wall unimplemented");
    return false;
}

static void display_maze(void) { 
    char pointer;
    if (current_direction == NORTH) {
        pointer = '^';
    } else if (current_direction == SOUTH) {
        pointer = 'v';
    } else if (current_direction == EAST) {
        pointer = '>';
    } else if (current_direction == WEST) {
        pointer = '<';
    } else {
        assert(false && "Unknown direction");
    } 

    int8_t i = 0;
    int8_t j = 0;
    int8_t k = HEIGHT-1;
    mvprintw(0, 0, " ");
    for (j = 0; j < WIDTH; j++) {
        mvprintw(0, 4*j+1, "___ ");
    }   

    for (i = 0; i < HEIGHT; i++) {
        mvprintw(2*i+1, 0, "|");
        mvprintw(2*i+2, 0, "|");
        for (j = 0; j < WIDTH; j++) {
            mvprintw(2*i+1, 4*j+1, "%c%c%c%c",' ',
                j == x_pos && k == y_pos ? pointer : ' ' ,' ',
                maze[j][k].east == NULL ? '|' : ' ');
            mvprintw(2*i+2, 4*j+1, "%s%c",
                maze[j][k].south == NULL ? "___" : "   ",
                maze[j][k].east == NULL ?  '|': ' ');
        }
        k--;
    }
    mvprintw(2*HEIGHT+1, 0, "\n");

    refresh();
    usleep(250000);
}

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream) {
    int8_t col = 0;
    int8_t row = HEIGHT;
    int8_t k = 0;

    /* parse top line */
    accept(space, "a space", &chars[col][row], fgetc(stream)); col++;
    for (k = 0; k < WIDTH-1; k++) {
        accept(underscore, "an underscore", &chars[col][row], fgetc(stream));
        col++;
        accept(space, "a space", &chars[col][row], fgetc(stream)); col++;
    }
    accept(underscore, "an underscore", &chars[col][row], fgetc(stream)); col++;
    ignore_trailing_whitespace(stream);  col=0; row--;

    /* parse body */
    for (k = 0; k < HEIGHT; k++) {
        accept(pipe_char, "a pipe_char", &chars[col][row], fgetc(stream)); col++;
        int8_t l = 0;
        for (l = 0; l < WIDTH-1; l++) {
            accept(underscore_or_space, "an underscore or space",
                   &chars[col][row], fgetc(stream)); col++;
            accept(pipe_char_or_space, "a pipe_char or space", &chars[col][row],
                   fgetc(stream)); col++;
        }
        accept(underscore_or_space, "an underscore or space", &chars[col][row],
                fgetc(stream)); col++;
        accept(pipe_char, "a pipe_char", &chars[col][row], fgetc(stream)); col++;
        ignore_trailing_whitespace(stream); col=0; row--;
    }
}

static void make_graph(struct cell maze[WIDTH][HEIGHT],
                       char chars[CHAR_WIDTH][CHAR_HEIGHT]) {
    int8_t i = 0;
    int8_t j = 0;
    int8_t k = 1;
    for (j = 0; j < CHAR_HEIGHT-1; j++) {
        for (k = 1; k < CHAR_WIDTH-1; k+=2) {
            i = k/2;
            if (chars[k][j+1] == ' ')
                maze[i][j].north = &(maze[i][j+1]);
            if (chars[k][j] == ' ')
                maze[i][j].south = &(maze[i][j-1]);
            if (chars[k+1][j] == ' ')
                maze[i][j].east = &(maze[i+1][j]);
            if (chars[k-1][j] == ' ')
                maze[i][j].west = &(maze[i-1][j]);
        }
    }
}

static void accept(bool (*accept_f)(char), char* name, char* dest, char cur) {
    assert(cur != EOF && "Error: End of file was reached while still parsing.");
    if (accept_f(cur)) {
        *dest = cur;
        return;
    }
    fprintf(stderr, "Error: expected %s, but found '%c'\n", name, cur);
    exit(1);
}

static void ignore_trailing_whitespace(FILE* stream) {
    int8_t cur = fgetc(stream);
    while (space(cur)) {
        cur = fgetc(stream);
    }
    char _;
    accept(newline, "a newline", &_, cur);
}

#if 0
static void print_edges(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            printf("maze[%d][%d]\n", i, j);
            if (maze[i][j].north != NULL)
                printf("maze[%d][%d].north = (%d,%d)\n", i, j, maze[i][j].north->x,
                        maze[i][j].north->y);
            if (maze[i][j].south != NULL)
                printf("maze[%d][%d].south = (%d,%d)\n", i, j, maze[i][j].south->x,
                        maze[i][j].south->y);
            if (maze[i][j].east != NULL)
                printf("maze[%d][%d].east = (%d,%d)\n", i, j, maze[i][j].east->x,
                        maze[i][j].east->y);
            if (maze[i][j].west != NULL)
                printf("maze[%d][%d].west = (%d,%d)\n", i, j, maze[i][j].west->x,
                        maze[i][j].west->y);
            if (i != WIDTH-1 || j != HEIGHT-1) {
                printf("\n");
            }
        }
    }
}
#endif
