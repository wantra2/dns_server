CC= gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -pedantic -g2

SRC= opt.c
OBJ= $(SRC:.c=.o)
BIN= opt

all: $(BIN)

$(BIN): $(OBJ)

clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)
