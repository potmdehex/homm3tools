# Based on libCello 1.1.7 makefile
CC ?= gcc
AR ?= ar

SRC := $(shell find ../../ -type f -name '*.c')
OBJ := $(addprefix obj/,$(notdir $(SRC:.c=.o)))

INC := -I ../../../../3rdparty/uthash/src -I ../../../h3mlib/
LIBDIR := ../../../../OUTPUT/gcc
STATICLIBS := $(LIBDIR)/h3mlib.a $(LIBDIR)/h3mtilespritegen.a

CFLAGS = $(INC) -std=c99 
LFLAGS = -L $(LIBDIR) 

PLATFORM := $(shell uname)

ifeq ($(OS),Windows_NT)
	EXE = h3mlib_helloworld.exe
else
	EXE = h3mlib_helloworld
endif

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(STATICLIBS) -o $@ $(LFLAGS)

obj/%.o: ../../%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	mkdir -p ../../../../OUTPUT/gcc
	cp $(EXE) ../../../../OUTPUT/gcc
	cp $(EXE) ../../../../OUTPUT/gcc