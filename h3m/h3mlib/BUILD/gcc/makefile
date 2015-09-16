# Based on libCello 1.1.7 makefile
CC ?= gcc
AR ?= ar

SRC := $(shell find ../../ -type f -name '*.c')
OBJ := $(addprefix obj/,$(notdir $(SRC:.c=.o)))

INC := -I ../../../../3rdparty/uthash/src -I ../../../h3mtilespritegen
STATICLIB := ../../../../OUTPUT/gcc/h3mtilespritegen.a

# We forfeit -Wpedantic because flexible array members are used inside unions.
# They could be replaced with array[1], but then -1 is needed at each sizeof
# which is an unnecessary complication
CFLAGS = $(INC) -std=c99 -DNO_ZLIB -O3 -Wall -Werror -Wno-unused -Wno-unused-result -Wno-strict-aliasing
LFLAGS = -fPIC -shared $(STATICLIB)

PLATFORM := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

ifeq ($(LBITS),64)
	CFLAGS += -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
endif 
ifeq ($(OS),Windows_NT)
	DYNAMIC = h3mlib.dll
	STATIC = h3mlib.a
else
	DYNAMIC = h3mlib.so
	STATIC = h3mlib.a
	CFLAGS += -fPIC
endif

# Libraries

all: $(DYNAMIC) $(STATIC)

$(DYNAMIC): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $@
	
$(STATIC): $(OBJ)
	$(AR) rcs $@ $(OBJ) -o $@

obj/%.o: ../../%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../../h3m_code/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
    
obj/%.o: ../../h3m_conversion/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
    
obj/%.o: ../../gen/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../../gen/passability/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../../h3m_parsing/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../../meta/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
    
obj/%.o: ../../gzip_empty/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	mkdir -p ../../../../OUTPUT/gcc
	cp $(STATIC) ../../../../OUTPUT/gcc
	cp $(DYNAMIC) ../../../../OUTPUT/gcc
