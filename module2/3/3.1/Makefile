CC = gcc
CFLAGS = -std=c99 -D_POSIX_C_SOURCE=200809L
TARGET = program
OBJ = main.o source.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

main.o: main.c header.h
	$(CC) $(CFLAGS) -c main.c

source.o: source.c header.h
	$(CC) $(CFLAGS) -c source.c

# Запуск с аргументами
run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean