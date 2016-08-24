#cross=arm-hisiv300-linux-
CC=$(cross)gcc
CPP=$(cross)g++
LIB_PATH=../lib/
CFLAGS = -g -Wall
LDFLAGS=-lzbase -L$(LIB_PATH) -Wl,-rpath,$(LIB_PATH) -lpthread
INCLUDES = -I$(LIB_PATH)/include/ -I./
CFLAGS += $(INCLUDES)

C_SRC = $(wildcard *.c)
C_OBJ = $(patsubst %c, %o, $(C_SRC))
CPP_SRC = $(wildcard *.cpp)
CPP_OBJ = $(patsubst %cpp, %o, $(CPP_SRC))

.PHONY:all clean

all:$(CPP_OBJ) $(C_OBJ)

.c.o:
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 
.cpp.o:
	$(CPP) $(CFLAGS) -o $@ $< $(LDFLAGS)


clean:
	rm *~ *.o -f
