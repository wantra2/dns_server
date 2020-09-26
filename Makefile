CC= gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -pedantic -g2

SRC= server.c
OBJ= $(SRC:.c=.o)
BIN= my_dns

all: $(BIN)

$(BIN): $(OBJ)

clean:
	$(RM) $(BIN)
	$(RM) $(OBJ)