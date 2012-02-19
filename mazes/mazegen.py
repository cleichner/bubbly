import random

from collections import defaultdict
from sys import argv, exit

def print_graph(graph,size):
    maze = list()
    maze.append(' ')
    
    for x in range(size):
        maze.append('_ ')
    maze.append('\n')
    default = size-1;
    for j in range(size):
        maze.append('|')
        for i in range(size):
            if (i, (default-j-1)) not in graph[(i,default-j)]:
                maze.append('_')
            else:
                maze.append(' ')
            if (i+1, default-j) not in graph[(i,default-j)]:
                maze.append('|')
            else:
                maze.append(' ')
        maze.append('\n')
    maze_printout = ''.join(maze)
    print (maze_printout)

if __name__ == "__main__":
    if len(argv) < 2:
        print ('Usage: maze_gen length_of_side_of_maze')
        exit(1)
    
    size = int(argv[1])
    
    if size < 4 or size%2 == 1:
        print ('Incorrect size. Must be even. Min: 6')
        exit(1)

    visited = set()
    neighbors = defaultdict(set)
    next_set = set()
    graph = defaultdict(set)

    for node in [(x,y) for x in range(size) for y in range(size)]:
        x, y = node
        if x != 0:
            neighbors[node].add((x-1,y))
        if y != 0:
            neighbors[node].add((x,y-1))
        if x != size-1:
            neighbors[node].add((x+1,y))
        if y != size-1:
            neighbors[node].add((x,y+1))

    starting_node = random.choice(tuple(neighbors))
    #Implementation of a growing tree algorithm to decide how nodes are connected
    visited.add(starting_node)
    next_set.add(starting_node)
    while next_set:
        next_node = random.choice(tuple(next_set))
        if all(n in visited for n in neighbors[next_node]):
            next_set.remove(next_node)
        else:
            for n in neighbors[next_node]:
                if n not in visited:
                    next_set.add(n)
                    visited.add(n)
                    graph[next_node].add(n)
                    graph[n].add(next_node)
                    break            

    print_graph(graph,size)
