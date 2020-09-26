CC= gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -pedantic -g2

SRC= zone.c
OBJ= $(SRC:.c=.o)
BIN= zone

all: $(BIN)

$(BIN): $(OBJ)

clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)
