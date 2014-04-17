PROG = "Sim-Comp"
CC = gcc
SRCS = simcomp.c
CFLAGS = -w -g -std=c99

all: $(PROG)

$(PROG):   $(SRCS)
	$(CC) $(CFLAGS) -pthread -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm $(PROG)

run:
	./$(PROG)
