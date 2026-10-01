CC = gcc
CFLAGS = -Wall -Wextra -std=c11
CPPFLAGS = -IHeaders
TARGET = papa_base
SRC = Código/Buzon.c Código/papa_base.c Código/juego.c Código/invasor.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run
