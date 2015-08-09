CFLAGS=$(CCFLAGS)

all:	dow

dow:	tst.o dow.o
	$(CC) -o dow tst.o dow.o $(LDFLAGS)

clean:
	rm -f *.o core dow
