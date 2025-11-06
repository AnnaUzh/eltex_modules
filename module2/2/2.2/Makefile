CC = gcc
CFLAGS = -std=c99
TARGET = program
OBJ = menu.o options.o

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

main.o: menu.c head.h
functions.o: options.c head.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
