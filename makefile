PROG = "Sim-Comp"
CC = gcc
SRCS = simcomp.c
CFLAGS = -Wall -std=c99

all: $(PROG)

$(PROG):   $(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm $(PROG)

run:
	./$(PROG)
