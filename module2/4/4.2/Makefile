CC = gcc
CFLAGS = -std=c99
TARGET = program
OBJ = main.o source.o

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c header.h
source.o: source.c header.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)