CC = gcc
CFLAGS = -I.
AR = ar
ARFLAGS = rcs

TARGET = program
LIB = libmylib.a
SRCS = functions.c main.c
OBJS = $(SRCS:.c=.o)
LIB_OBJS = functions.o
HEADER = struct.h

all: $(TARGET)

$(TARGET): main.o $(LIB)
	$(CC) $(CFLAGS) -o $@ main.o -L. -lmylib

$(LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(LIB) *.o

rebuild: clean all

.PHONY: all clean rebuild