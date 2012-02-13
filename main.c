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

void execute_actions(struct action actions[ACTION_SIZE],
                     struct point* position) {
    // needs to update what we know about the current robot state
    // needs to check our assumptions about which walls exist
    // need a fast execute and cautious execute
    int8_t i;
    for (i = 0; i < ACTION_SIZE; i++) {
        actions[i].move(actions[i].times);
    }
    position->x = 1; // totally wrong but makes the compiler happy
}

int main(int argc, char* argv[]) {
    struct point goal = {.x = WIDTH/2, .y = HEIGHT/2};
    struct point position = {.x = 0, .y = 0};
    
    struct cell maze[WIDTH][HEIGHT];
    initialize_hardware(argc, argv);
    init_maze(maze);
    connect_maze(maze);
    struct action actions[ACTION_SIZE];
    init_actions(actions);
    find_path(actions, maze, position, goal);
    execute_actions(actions, &position);

    // find center
    //   make dijkstra path to goal
    //   take one step forward
    //   if there are any walls you don't know about, update the maze
    //      make dijkstra path to goal
    //
    // map the whole maze
    //   depth first search to all points not previously visited
    // return to the start
    // generate a path to the center
    // optimize the path
    // run it as fast as possible

    finalize_hardware();
    return 0;
}

