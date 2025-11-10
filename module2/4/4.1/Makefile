CC = gcc
CFLAGS = -std=c99
TARGET = program
OBJ = main.o functions.o

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

main.o: main.c struct.h
functions.o: functions.c struct.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
