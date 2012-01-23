all:
	clang -g -c -Wall -Werror -Wextra -pedantic movement_sim.c 
	clang -g -c -Wall -Werror -Wextra -pedantic main.c       
	clang -g -lcurses -o softbot main.o movement_sim.o

clean:
	rm -f *.o softbot
 
.PHONY: clean
