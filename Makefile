CC = cc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude

TARGET = myls
SRC = src/main.c src/options.c src/listing.c src/sorting.c src/display.c
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

