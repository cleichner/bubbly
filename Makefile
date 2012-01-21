# BASECFLAGS=-O3 -pedantic -Wall -Werror
# CFLAGS+=$(BASECFLAGS) -std=c99
# CXXFLAGS+=$(BASECFLAGS)
# LDFLAGS+=-lcurses -lboost_program_options -lueye_api -lm -lopencv
# 
# PRG=$(patsubst %.cc,%,$(wildcard *.cc))
# PRG+=$(patsubst %.c,%,$(wildcard *.c))
# 
# all: $(PRG)
# 
# clean:
# 	rm -f *.o *.pyc $(PRG)
# 
# .PHONY: clean

all:
	clang -c -Wall -Werror -Wextra  movement_sim.c 
	clang -c -Wall -Werror -Wextra main.c       
	clang -lcurses -o softbot main.o movement_sim.o

clean:
	rm -f *.o bubbly #$(PRG)
 
.PHONY: clean
