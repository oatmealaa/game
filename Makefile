print-%:; @echo $($*)

CC = gcc
LD = gcc

# library paths

CCFLAGS = 

LDFLAGS = -lm

BIN = bin

SRC=$(wildcard src/*.c)
OBJ=$(filter-out $(SRC), $(SRC:.c=.o))


LDFLAGS += -lSDL2

all: game dirs

dirs:
	mkdir -p bin

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)


game: dirs $(OBJ)
	$(LD) -o bin/game $(OBJ) $(LDFLAGS)

clean:
	rm -rf bin
	rm src/*.o
