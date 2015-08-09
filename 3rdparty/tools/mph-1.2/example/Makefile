CFLAGS=$(CCFLAGS)

all:	pascal gplus

pascal:	tst.o pascal.o
	$(CC) -o pascal tst.o pascal.o $(LDFLAGS)

gplus:	tst.o gplus.o
	$(CC) -o gplus tst.o gplus.o $(LDFLAGS)

tst:	tst.o hash.o
	$(CC) -o tst tst.o hash.o $(LDFLAGS)

clean:
	rm -f *.o core pascal gplus tst hash.c
