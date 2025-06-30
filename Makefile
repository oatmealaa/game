print-%:; @echo $($*)

CC = gcc
LD = gcc

# library paths

CCFLAGS =

CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -Wpedantic
CCFLAGS += -Wfloat-equal
CCFLAGS += -Wstrict-aliasing
CCFLAGS += -Wswitch-default
CCFLAGS += -Wformat=2
CCFLAGS += -Wno-unused-parameter

LDFLAGS = -lm

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
