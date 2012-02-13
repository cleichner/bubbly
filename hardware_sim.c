#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "hardware.h"
#include "maze.h"

#define CHAR_WIDTH 2*WIDTH+1
#define CHAR_HEIGHT HEIGHT+1

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream);
static void make_graph(struct cell maze[WIDTH][HEIGHT],
                       char chars[CHAR_WIDTH][CHAR_HEIGHT]);

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
static struct point pos = {.x = 0,  .y = 0};
static direction_t current_direction = NORTH;

void handler(int sig) {
    if (sig == SIGINT) {
        finalize_display();
        exit(0);
    }
}

void initialize_hardware(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s maze-file\n", argv[0]);
        exit(1);
    }

    char chars[CHAR_WIDTH][CHAR_HEIGHT];
    FILE* stream = fopen(argv[1], "r");
    assert(stream && "Maze-file could not be opened for reading.\n");

    parse_maze_file(chars, stream);
    fclose(stream);

    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    init_maze(maze);
    make_graph(maze, chars);

    init_display();
    display_maze(maze, pos, current_direction);
}

void finalize_hardware(void) {
    finalize_display();
}

void move_forward(int8_t n) {
    if (n == 0) {
        display_maze(maze, pos, current_direction);
        return;
    }

    int8_t i; 
    for (i = 0; i < n; i++) {
        if (current_direction == NORTH) {
            assert(!(pos.y + 1 >= HEIGHT) && maze[pos.x][pos.y].path[NORTH] && 
                   "pos.y >= HEIGHT or tried to move through a wall");
            pos.y += 1;
        } else if (current_direction == SOUTH) {
            assert(!(pos.y - 1 < 0) && maze[pos.x][pos.y].path[SOUTH] &&
                   "pos.y < 0 or tried to move through a wall");
            pos.y -= 1;
        } else if (current_direction == EAST) {
            assert(!(pos.x + 1 >= WIDTH) && maze[pos.x][pos.y].path[EAST] &&
                   "pos.x >= WIDTH or tried to move through a wall");
            pos.x += 1;
        } else if (current_direction == WEST) {
            assert(!(pos.x - 1 < 0) && maze[pos.x][pos.y].path[WEST] &&
                   "pos.x < 0 or tried to move through a wall");
            pos.x -= 1;
        } else {
            assert(false && "Unknown direction");
        } 
        display_maze(maze, pos, current_direction);
    }
}

void turn_right(int8_t n) {
    direction_t right[] = {EAST, WEST, SOUTH, NORTH};
    int8_t i = 0;
    for (i = 0; i < n; i++) {
        current_direction = current_direction[right];
        display_maze(maze, pos, current_direction);
    }
}

void turn_left(int8_t n) {
    direction_t left[] = {WEST, EAST, NORTH, SOUTH};
    int8_t i = 0;
    for (i = 0; i < n; i++) {
        current_direction = current_direction[left];
        display_maze(maze, pos, current_direction);
    }
}

bool has_wall(side_t side) {
    direction_t absolute[][4] = {{NORTH, SOUTH, EAST, WEST},
                                 {WEST, EAST, NORTH, SOUTH},
                                 {EAST, WEST, SOUTH, NORTH}};
    direction_t abs_side = absolute[side][current_direction];
    return !maze[pos.x][pos.y].path[abs_side];
}

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream) {
    int8_t col = 0;
    int8_t row = HEIGHT;
    int8_t k = 0;

    // parse top line
    accept(space, "a space", &chars[col][row], fgetc(stream)); col++;
    for (k = 0; k < WIDTH-1; k++) {
        accept(underscore, "an underscore", &chars[col][row], fgetc(stream));
        col++;
        accept(space, "a space", &chars[col][row], fgetc(stream)); col++;
    }
    accept(underscore, "an underscore", &chars[col][row], fgetc(stream)); col++;
    ignore_trailing_whitespace(stream);  col=0; row--;

    // parse body
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
                maze[i][j].path[NORTH] = &(maze[i][j+1]);
            if (chars[k][j] == ' ')
                maze[i][j].path[SOUTH] = &(maze[i][j-1]);
            if (chars[k+1][j] == ' ')
                maze[i][j].path[EAST] = &(maze[i+1][j]);
            if (chars[k-1][j] == ' ')
                maze[i][j].path[WEST] = &(maze[i-1][j]);
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
