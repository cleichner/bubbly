from sys import argv, exit
import random
from collections import defaultdict


def print_graph(graph,size):
  str = ' '
  for x in range(size):
    str+= '_ '
  str+='\n'
  default = size-1;
  
  for j in range(size):
    str+='|'
    for i in range(size):
      if (i, (default-j-1)) not in graph[(i,default-j)]:
        str+='_'
      else:
        str+=' '
      if (i+1, default-j) not in graph[(i,default-j)]:
        str+='|'
      else:
        str+=' '
    str+='\n'
   
  print (str)

  
if __name__ == "__main__":
  if len(argv) < 2:
    print ('Usage: maze_gen height width')
    exit(1)
  
  size = int(argv[1])
  
  if size < 6 or size%2 == 1:
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

  first = random.choice(tuple(neighbors))
  next_set.add(first)
  
  while next_set:
    next_list = random.sample(next_set, 1)
    next_node = next_list.pop()
    count = 0
    for n in neighbors[next_node]:
      if n not in visited:
        next_set.add(n)
        visited.add(n)
        graph[next_node].add(n)
        break
      if count+1 == len(neighbors[next_node]):
        next_set.remove(next_node)
      count += 1
      
  print_graph(graph,size)