# Based on libCello 1.1.7 makefile
CC ?= gcc
AR ?= ar

SRC := $(shell find ../src -type f -name '*.c')
OBJ := $(addprefix obj/,$(notdir $(SRC:.c=.o)))

INC := -I ../../3rdparty/uthash/src -I ../../tilespritegen/include
STATICLIB := ../../h3mtools/Release/tilespritegen.a

# We forfeit -Wpedantic because flexible array members are used inside unions.
# They could be replaced with array[1], but then -1 is needed at each sizeof
# which is an unnecessary complication
CFLAGS = $(INC) -std=c99 -DNO_ZLIB -O3 -Wall -Werror -Wno-unused
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

obj/%.o: ../src/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../src/gen/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../src/gen/passability/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../src/h3m_parsing/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@

obj/%.o: ../src/gzip_empty/%.c | obj
	$(CC) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	cp $(DYNAMIC) ../../h3mtools/Release
	cp $(STATIC) ../../h3mtools/Release
