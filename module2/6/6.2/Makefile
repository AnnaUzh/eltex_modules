CC = gcc
CFLAGS = -I. -fPIC
LDFLAGS = -L. -Wl,-rpath,.

TARGET = program
LIB_SHARED = libmylib.so
HEADER = struct.h

all: $(TARGET)

# Создание исполняемого файла
$(TARGET): main.o $(LIB_SHARED)
	$(CC) $(CFLAGS) -o $@ main.o $(LDFLAGS) -lmylib

# Создание динамической библиотеки
$(LIB_SHARED): functions.c $(HEADER)
	$(CC) $(CFLAGS) -shared -o $@ functions.c

# Компиляция main.c
main.o: main.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(LIB_SHARED) *.o

rebuild: clean all

# Проверка зависимостей
check:
	ldd $(TARGET)

.PHONY: all clean rebuild check