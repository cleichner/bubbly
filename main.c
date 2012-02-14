#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "maze.h"
#include "hardware.h"
#define ACTION_SIZE 2*WIDTH*HEIGHT
static int8_t inf = INT8_MAX;
static struct action nop = { .move = turn_right, .times = 0 };
static direction_t current_direction = NORTH;
static struct point position;

void init_maze(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            maze[i][j].x = i;
            maze[i][j].y = j;
            maze[i][j].path[NORTH] = NULL;
            maze[i][j].path[SOUTH] = NULL;
            maze[i][j].path[EAST] = NULL;
            maze[i][j].path[WEST] = NULL;
        }
    }
}

void connect_maze(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i;
    int8_t j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            if (j != HEIGHT-1)
                maze[i][j].path[NORTH] = &(maze[i][j+1]);
            if (j != 0)
                maze[i][j].path[SOUTH] = &(maze[i][j-1]);
            if (i != WIDTH-1)
                maze[i][j].path[EAST] = &(maze[i+1][j]);
            if (i != 0)
                maze[i][j].path[WEST] = &(maze[i-1][j]);
        }
    }
}

void add_wall(struct cell maze[WIDTH][HEIGHT], int8_t x, int8_t y,
              direction_t dir) {
    direction_t opposite[] = {SOUTH, NORTH, WEST, EAST};
    int8_t pair_x = maze[x][y].path[dir]->x;
    int8_t pair_y = maze[x][y].path[dir]->y;
    maze[x][y].path[dir] = NULL;
    maze[pair_x][pair_y].path[opposite[dir]] = NULL;
}

struct cell* min_cell(struct cell maze[WIDTH][HEIGHT],
                      int8_t dist[WIDTH][HEIGHT], 
                      bool visited[WIDTH][HEIGHT]) {
    // return the cell in maze with the lowest value in dist, that hasn't been
    // visited
    struct cell* min = NULL;
    int8_t min_distance = inf;
    int i,j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
           if (dist[i][j] < min_distance && !visited[i][j]) {
               min = &maze[i][j];
               min_distance = dist[i][j];
           }
        }
    }
    return min;
}

bool all(bool visited[WIDTH][HEIGHT]) {
    int i,j;
    for (i = 0; i < WIDTH; i++)
        for (j = 0; j < HEIGHT; j++)
            if (!visited[i][j])
                return false;
    return true;
}

// inputs: maze, source, target
// output: sequence
void dijkstra(struct cell maze[WIDTH][HEIGHT], 
              struct point source, struct point target,
              struct cell* sequence[WIDTH*HEIGHT]) {
    int8_t dist[WIDTH][HEIGHT];
    struct cell* previous[WIDTH][HEIGHT];
    bool visited[WIDTH][HEIGHT];
    int8_t i, j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            dist[i][j] = inf; // Unknown distance from source to [i][j]
            previous[i][j] = NULL; // previous node in optimal path from source
            visited[i][j] = false;
        }
    }  

    // build previous table
    dist[source.x][source.y] = 0;
    while (!all(visited)) {
        struct cell* u = min_cell(maze, dist, visited);
        visited[u->x][u->y] = true;
        if (dist[u->x][u->y] == inf)
            break;

        if (target.x == u->x && target.y == u->y) // we found the target!
            break;

        // go over all neighbors
        for (i = 0; i < 4; i++) {
            struct cell* v = u->path[i];
            if (v == NULL) // v isn't an edge
                continue;
            if (visited[v->x][v->y]) // skip visited
                continue;
            int8_t alt = dist[u->x][u->y] + 1; // distance between u and v
            if (alt < dist[v->x][v->y]) {
                dist[v->x][v->y] = alt;
                previous[v->x][v->y] = u;
            }
        }
    }

    // map sequence
    struct cell* u = &maze[target.x][target.y];
    int16_t s = WIDTH*HEIGHT-1;
    while (previous[u->x][u->y]) {
        assert(s >= 0 && "index out of range for sequence");
        sequence[s] = u; // fill the sequence from the back
        u = previous[u->x][u->y];
        s--;
    }
    sequence[s] = &maze[source.x][source.y];
}

int16_t add_backward(struct action actions[ACTION_SIZE], int16_t a) {
    actions[a].move = turn_right;
    actions[a].times = 1;
    a++;
    actions[a].move = turn_right;
    actions[a].times = 1;
    a++;
    actions[a].move = move_forward;
    actions[a].times = 1;
    return a;
}

int16_t add_forward(struct action actions[ACTION_SIZE], int16_t a) {
    actions[a].move = move_forward;
    actions[a].times = 1;
    return a;
}

int16_t add_left(struct action actions[ACTION_SIZE], int16_t a) {
    actions[a].move = turn_left;
    actions[a].times = 1;
    a++;
    actions[a].move = move_forward;
    actions[a].times = 1;
    return a;
}

int16_t add_right(struct action actions[ACTION_SIZE], int16_t a) {
    actions[a].move = turn_right;
    actions[a].times = 1;
    a++;
    actions[a].move = move_forward;
    actions[a].times = 1;
    return a;
}

void build_actions(struct cell* sequence[WIDTH*HEIGHT],
                   struct action actions[ACTION_SIZE]) {
    // build the actions to move through the sequence
    int16_t s;
    for (s = 0; sequence[s] == NULL; s++)
        ; // find the start of the sequence

    // first index is what direction the next square is from where you are
    // second index is what direction you are facing
    int16_t (*add_movement_between[4][4])(struct action[ACTION_SIZE],
                                          int16_t) = {
        {add_forward, add_backward, add_left, add_right},
        {add_backward, add_forward, add_right, add_left},
        {add_right, add_left, add_forward, add_backward},
        {add_left, add_right, add_backward, add_forward},
    };

    int16_t a = 0;
    direction_t heading = current_direction;
    for (; s < WIDTH*HEIGHT-1; s++) {
        assert(a < ACTION_SIZE && "index out of range for actions");
        direction_t path_dir;
        for (path_dir = NORTH; path_dir <= WEST; path_dir++)
            if (sequence[s]->path[path_dir] == sequence[s+1])
                break;

        a = add_movement_between[path_dir][heading](actions, a);
        heading = path_dir;
        a++;
    }

    // nop out the rest of the actions
    for (; a < ACTION_SIZE; a++)
        actions[a] = nop;
}

void find_path(struct action actions[ACTION_SIZE],
               struct cell maze[WIDTH][HEIGHT],
               struct point source,
               struct point target) {
    struct cell* sequence[WIDTH*HEIGHT] = {0};
    dijkstra(maze, source, target, sequence);
    build_actions(sequence, actions);
}

void init_actions(struct action actions[ACTION_SIZE]) {
    int8_t i;
    for (i = 0; i < ACTION_SIZE; i++) {
        actions[i] = nop;
    }
}

bool safe_execute_actions(struct cell maze[WIDTH][HEIGHT],
                          struct action actions[ACTION_SIZE]) {
    direction_t left[] = {WEST, EAST, NORTH, SOUTH};
    direction_t right[] = {EAST, WEST, SOUTH, NORTH};
    int8_t i;
    int8_t j;
    for (i = 0; i < ACTION_SIZE; i++) {
        for (j = 0; j < actions[i].times; j++) {
            if (actions[i].move == nop.move && actions[i].times == nop.times) {
                break;
            } else if (actions[i].move == move_forward) {
                if (current_direction == NORTH) {
                    position.y += 1;
                } else if (current_direction == SOUTH) {
                    position.y -= 1;
                } else if (current_direction == EAST) {
                    position.x += 1;
                } else if (current_direction == WEST) {
                    position.x -= 1;
                } else {
                    assert(false && "Unknown direction");
                } 
            } else if (actions[i].move == turn_left) {
                current_direction = current_direction[left];
            } else if (actions[i].move == turn_right) {
                current_direction = current_direction[right];
            } else {
                assert(false && "Unknown action");
            }
            actions[i].move(1);
            uint8_t x = position.x;
            uint8_t y = position.y;
            maze[x][y].visited = true;
            direction_t front = current_direction;
            direction_t left_side = current_direction[left];
            direction_t right_side = current_direction[right];
            // correct our path
            bool corrected = false;
            if (maze[x][y].path[front] && has_wall(FRONT)) {
                add_wall(maze, x, y, front);
                corrected = true;
            }
            if (maze[x][y].path[left_side] && has_wall(LEFT)) {
                add_wall(maze, x, y, left_side);
                corrected = true;
            }
            if (maze[x][y].path[right_side] && has_wall(RIGHT)) {
                add_wall(maze, x, y, right_side);
                corrected = true;
            }
            if (corrected) {
                return false;
            }
        }
    }
    return true;
}

bool has_not_visted(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i,j;
    for (i = 0; i < WIDTH; i++)
        for (j = 0; j < HEIGHT; j++)
            if (!maze[i][j].visited)
                return true;
    return false;
}

struct point not_visited_in(struct cell maze[WIDTH][HEIGHT]) {
    int8_t i,j;
    struct point pos = {.x = 0, .y = 0};
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            if (!maze[i][j].visited) {
                pos.x = maze[i][j].x;
                pos.y = maze[i][j].y;
            }
        }
    }
    return pos;
}

int main(int argc, char* argv[]) {
    struct point goal = {.x = WIDTH/2, .y = HEIGHT/2};
    struct point start = {.x = 0, .y = 0};
    position = start;
    
    struct cell maze[WIDTH][HEIGHT];
    initialize_hardware(argc, argv);
    init_maze(maze);
    connect_maze(maze);
    struct action actions[ACTION_SIZE];
    init_actions(actions);
    maze[start.x][start.y].visited = true;

    // find the center
    do {
        find_path(actions, maze, position, goal);
    } while (!safe_execute_actions(maze, actions));

    // map the maze
    while (has_not_visted(maze)) {
        do {
            find_path(actions, maze, position, not_visited_in(maze));
        } while (!safe_execute_actions(maze, actions));
    }

    // go back to the start
    do {
        find_path(actions, maze, position, start);
    } while (!safe_execute_actions(maze, actions));

    // find the center
    // optimize the path, not implemented 
    // run it as fast as possible, not implemented
    do {
        find_path(actions, maze, position, goal);
    } while (!safe_execute_actions(maze, actions));
    
    // go back to the start
    do {
        find_path(actions, maze, position, start);
    } while (!safe_execute_actions(maze, actions));

    finalize_hardware();
    return 0;
}

