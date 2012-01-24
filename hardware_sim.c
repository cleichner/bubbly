#include <assert.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "maze.h"
#include "hardware.h"

#define CHAR_WIDTH 2*WIDTH+1
#define CHAR_HEIGHT HEIGHT+1

static void parse_maze_file(char chars[CHAR_WIDTH][CHAR_HEIGHT], FILE* stream);
static void make_graph(struct cell maze[WIDTH][HEIGHT],
                       char chars[CHAR_WIDTH][CHAR_HEIGHT]);
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
static direction_t current_direction = NORTH;


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

    init_maze(maze);
    make_graph(maze, chars);

    // print_edges(maze);
    initscr();
    display_maze();
}

void finalize_hardware(void) {
    endwin();
}

void move_forward(int8_t n) {
    if (n == 0) {
        display_maze();
        return;
    }

    int8_t i; 
    for (i = 0; i < n; i++) {
        if (current_direction == NORTH) {
            if (!(y_pos + 1 >= HEIGHT) && maze[x_pos][y_pos].north)
                y_pos += 1;
            else
                assert(false && "y_pos >= HEIGHT or tried to move through a wall");
        } else if (current_direction == SOUTH) {
            if (!(y_pos - 1 < 0) && maze[x_pos][y_pos].south)
                y_pos -= 1;
            else
                assert(false && "y_pos < 0 or tried to move through a wall");
        } else if (current_direction == EAST) {
            if (!(x_pos + 1 >= WIDTH) && maze[x_pos][y_pos].east)
                x_pos += 1;
            else
                assert(false && "x_pos >= WIDTH or tried to move through a wall");
        } else if (current_direction == WEST) {
            if (!(x_pos - 1 < 0) && maze[x_pos][y_pos].west)
                x_pos -= 1;
            else
                assert(false && "x_pos < 0 or tried to move through a wall");
        } else {
            assert(false && "Unknown direction");
        } 
        display_maze();
    }
}

void turn_right(void) {
    direction_t right[] = {EAST, WEST, SOUTH, NORTH};
    current_direction = current_direction[right];
    display_maze();
}

void turn_left(void) {
    direction_t left[] = {WEST, EAST, NORTH, SOUTH};
    current_direction = current_direction[left];
    display_maze();
}

/* TODO turn this into a table */
bool has_wall(side_t side) {
    if (side == FRONT) {
        if (current_direction == NORTH)
            return !maze[x_pos][y_pos].north;
        if (current_direction == SOUTH)
            return !maze[x_pos][y_pos].south;
        if (current_direction == EAST)
            return !maze[x_pos][y_pos].east;
        if (current_direction == WEST)
            return !maze[x_pos][y_pos].west;
    }
    if (side == LEFT) {
        if (current_direction == NORTH)
            return !maze[x_pos][y_pos].west;
        if (current_direction == SOUTH)
            return !maze[x_pos][y_pos].east;
        if (current_direction == EAST)
            return !maze[x_pos][y_pos].north;
        if (current_direction == WEST)
            return !maze[x_pos][y_pos].south;
    }
    if (side == RIGHT) {
        if (current_direction == NORTH)
            return !maze[x_pos][y_pos].east;
        if (current_direction == SOUTH)
            return !maze[x_pos][y_pos].west;
        if (current_direction == EAST)
            return !maze[x_pos][y_pos].south;
        if (current_direction == WEST)
            return !maze[x_pos][y_pos].north;
    }

    assert(false && "unknown side");
    return true;
}

static void display_maze(void) { 
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
                j == x_pos && k == y_pos ? current_direction[arrow] : ' ' ,
                maze[j][k].east ? ' ' : '|');

            mvprintw(2*i+2, 4*j+1, "%s%c", maze[j][k].south ? "   " : "___",
                maze[j][k].east ?  ' ': '|');
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
