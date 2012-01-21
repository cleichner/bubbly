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

static void ignore_trailing_whitespace(FILE* stream);

static void accept(bool (*accept_f)(char), char* name, char* dest, char cur);
static bool space(char cur) { return cur == ' ' || cur == '\t'; }
static bool underscore(char cur) { return cur == '_'; }
static bool newline(char cur) { return cur == '\n'; }
static bool pipe(char cur) { return cur == '|'; }
static bool underscore_or_space(char cur) {
    return underscore(cur) || space(cur);
}
static bool pipe_or_space(char cur) { return pipe(cur) || space(cur); }

// static struct cell maze[WIDTH][HEIGHT];

void initialize_movements(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s maze-file\n", argv[0]);
        exit(1);
    }

    char chars[CHAR_WIDTH][CHAR_HEIGHT];
    FILE* stream = fopen(argv[1], "r");
    assert(stream && "Maze-file could not be opened for reading.\n");

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
    fclose(stream);

    int8_t l;
    for (l = CHAR_HEIGHT-1; l >= 0; l--) {
        for (k = 0; k < CHAR_WIDTH; k++) {
            printf("%c", chars[k][l]);
        }
        printf("\n");
    }
    printf("\n");
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
