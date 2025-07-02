print-%:; @echo $($*)

CC =gcc
LD = gcc


CCFLAGS =

CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -Wpedantic
CCFLAGS += -Wfloat-equal
CCFLAGS += -Wstrict-aliasing
CCFLAGS += -Wswitch-default
CCFLAGS += -Wformat=2
CCFLAGS += -Wno-unused-parameter

#CCFLAGS += -I/usr/x86_64-w64-mingw32/include -I/usr/x86_64-w64-mingw32/include/SDL2 -Dmain=SDL_main


LDFLAGS = -lm

#LDFLAGS += -L/usr/x86_64-w64-mingw32/lib -lmingw32 -mwindows

BIN = bin

SRC=$(wildcard src/*.c)
OBJ=$(filter-out $(SRC), $(SRC:.c=.o))


LDFLAGS += -lSDL2

all: game dirs

dirs:
	mkdir -p bin

%.o: %.c
	$(CC) -o $@ -MMD -c $< $(CCFLAGS)


game: dirs $(OBJ)
	$(LD) -o bin/game $(OBJ) $(LDFLAGS)



clean:
	rm bin/game
	rm src/*.o
	rm src/*.d
