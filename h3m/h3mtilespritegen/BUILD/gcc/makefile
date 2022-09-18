# Based on libCello 1.1.7 makefile
CC ?= gcc
AR ?= ar

SRC := $(shell find ../../ -type f -name '*.c')
OBJ := $(addprefix obj/,$(notdir $(SRC:.c=.o)))

CFLAGS = -I ../../ -std=c89 -Wall -Wpedantic -Werror -Wno-unused -O3
LFLAGS = -shared

PLATFORM := $(shell uname)

ifeq ($(OS),Windows_NT)
	DYNAMIC = h3mtilespritegen.dll
	STATIC = h3mtilespritegen.a
else
	DYNAMIC = h3mtilespritegen.so
	STATIC = h3mtilespritegen.a
	CFLAGS += -fPIC
endif

# Libraries

all: $(DYNAMIC) $(STATIC)

$(DYNAMIC): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $@
	
$(STATIC): $(OBJ)
	$(AR) rcs $@ $(OBJ) $@

obj/%.o: ../../terrain/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../../river/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../../road/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

# This is OK on non-win too as code contains ifdefs
obj/%.o: ../../win/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../../%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	mkdir -p ../../../../OUTPUT/gcc
	cp $(STATIC) ../../../../OUTPUT/gcc
	cp $(DYNAMIC) ../../../../OUTPUT/gcc
