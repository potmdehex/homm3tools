# Based on libCello 1.1.7 makefile
CC ?= gcc
AR ?= ar

SRC := $(shell find ../src -type f -name '*.c')
OBJ := $(addprefix obj/,$(notdir $(SRC:.c=.o)))

CFLAGS = -I ./include -std=c89 -Wall -Wpedantic -Werror -Wno-unused -O3
LFLAGS = -shared

PLATFORM := $(shell uname)

ifeq ($(OS),Windows_NT)
	DYNAMIC = tilespritegen.dll
	STATIC = tilespritegen.a
else
	DYNAMIC = tilespritegen.so
	STATIC = tilespritegen.a
	CFLAGS += -fPIC
endif

# Libraries

all: $(DYNAMIC) $(STATIC)

$(DYNAMIC): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $@
	
$(STATIC): $(OBJ)
	$(AR) rcs $@ $(OBJ) -o $@

obj/%.o: ../src/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../src/terrain/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../src/river/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../src/road/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../src/win/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	cp $(STATIC) ../../h3mtools/Release
	cp $(DYNAMIC) ../../h3mtools/Release
