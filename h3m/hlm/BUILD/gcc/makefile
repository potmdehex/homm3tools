# Based on libCello 1.1.7 makefile
CXX ?= g++
AR ?= ar

SRC := $(shell find ../src -type f -name '*.cpp')
OBJ := $(addprefix obj/,$(notdir $(SRC:.cpp=.o)))

INC := -I ../../h3mlib/include
LIBDIR := ../../h3mtools/Release
STATICLIBS := $(LIBDIR)/h3mlib.a $(LIBDIR)/tilespritegen.a

CFLAGS = $(INC) -std=c++1y -O3 -fPIC
LFLAGS = -fPIC -shared

PLATFORM := $(shell uname)

ifeq ($(OS),Windows_NT)
	DYNAMIC = hlm.dll
	STATIC = hlm.a
else
	DYNAMIC = hlm.so
	STATIC = hlm.a
endif

# Libraries

all: $(DYNAMIC) $(STATIC)

$(DYNAMIC): $(OBJ)
	$(CXX) $(OBJ) $(LFLAGS) $(STATICLIBS) -o $@
	
$(STATIC): $(OBJ)
	$(AR) rcs $@ $(OBJ) -o $@

obj/%.o: ../src/%.cpp | obj
	$(CXX) $< -c $(CFLAGS) -o $@
	
obj/%.o: ../src/conv/%.cpp | obj
	$(CXX) $< -c $(CFLAGS) -o $@
	
obj:
	mkdir -p obj

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC)
	
install: all
	cp $(DYNAMIC) ../../h3mtools/Release
	cp $(STATIC) ../../h3mtools/Release
