CC=mpicc
CCFLAGS=-std=c99 -O2 -Wall -I. -lm

OBJ = gs.o

%.o: %.c
	$(CC) -c -o $@ $< $(CCFLAGS)

gs: $(OBJ)
	$(CC) -o $@ $^ $(CCFLAGS)

.PHONY: clean

clean:
	rm -f *.o gs
