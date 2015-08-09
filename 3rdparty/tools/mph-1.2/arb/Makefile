CFLAGS=$(CCFLAGS)
OBJ=mph.o arena.o keys.o graph.o misc.o

all:	mph emitc mphm

mph:	$(OBJ)
	$(CC) -o mph $(OBJ) $(LDFLAGS) -lm

emitc:	emitc.o
	$(CC) -o emitc emitc.o $(LDFLAGS)

mphm:	mphm.o arena.o keys.o misc.o
	$(CC) -o mphm mphm.o arena.o keys.o misc.o $(LDFLAGS) -lm

$(OBJ):	*.h

clean:
	rm -f *.o core mph emitc mphm
