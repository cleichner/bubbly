all:
	clang -g -c -Wall -Werror -Wextra -pedantic display_ncurses.c 
	clang -g -c -Wall -Werror -Wextra -pedantic display_text.c 
	clang -g -c -Wall -Werror -Wextra -pedantic hardware_sim.c 
	clang -g -c -Wall -Werror -Wextra -pedantic main.c       
	clang -g -lcurses -o softbot main.o hardware_sim.o display_ncurses.o
	clang -g -lcurses -o textbot main.o hardware_sim.o display_text.o

clean:
	rm -f *.o softbot textbot
 
.PHONY: clean
