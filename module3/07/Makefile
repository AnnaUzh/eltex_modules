CC = gcc
CFLAGS = -std=c11
LDFLAGS = -lrt -pthread

all: chat_1 chat_2

chat_1: chat_1.c
	$(CC) $(CFLAGS) chat_1.c -o chat_1 $(LDFLAGS)

chat_2: chat_2.c
	$(CC) $(CFLAGS) chat_2.c -o chat_2 $(LDFLAGS)

clean:
	rm -f chat_1 chat_2

.PHONY: all clean