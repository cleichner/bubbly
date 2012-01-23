#include <assert.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "maze.h"
#include "movement.h"

#define CHAR_WIDTH 2*WIDTH+1
#define CHAR_HEIGHT HEIGHT+1

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream);
static void make_graph(struct cell maze[WIDTH][HEIGHT],
                       char chars[CHAR_WIDTH][CHAR_HEIGHT]);
static void print_maze(struct cell maze[WIDTH][HEIGHT]);

static void accept(bool (*accept_f)(char), char* name, char* dest, char cur);
static bool space(char cur) { return cur == ' ' || cur == '\t'; }
static bool underscore(char cur) { return cur == '_'; }
static bool newline(char cur) { return cur == '\n'; }
static bool pipe(char cur) { return cur == '|'; }
static bool underscore_or_space(char cur) {
    return underscore(cur) || space(cur);
}
static bool pipe_or_space(char cur) { return pipe(cur) || space(cur); }
static void ignore_trailing_whitespace(FILE* stream);

static struct cell maze[WIDTH][HEIGHT];

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
    print_maze(maze);
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
        accept(pipe, "a pipe", &chars[col][row], fgetc(stream)); col++;
        int8_t l = 0;
        for (l = 0; l < WIDTH-1; l++) {
            accept(underscore_or_space, "an underscore or space",
                   &chars[col][row], fgetc(stream)); col++;
            accept(pipe_or_space, "a pipe or space", &chars[col][row],
                   fgetc(stream)); col++;
        }
        accept(underscore_or_space, "an underscore or space", &chars[col][row],
                fgetc(stream)); col++;
        accept(pipe, "a pipe", &chars[col][row], fgetc(stream)); col++;
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
            if (chars[k][j+1] == ' ') {
                maze[i][j].north = &(maze[i][j+1]);
            }
            if (chars[k][j] == ' ') {
                maze[i][j].south = &(maze[i][j-1]);
            }
            if (chars[k+1][j] == ' ') {
                maze[i][j].east = &(maze[i+1][j]);
            }

            if (chars[k-1][j] == ' ') {
                maze[i][j].west = &(maze[i-1][j]);
            }
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

static void print_maze(struct cell maze[WIDTH][HEIGHT]) {
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

