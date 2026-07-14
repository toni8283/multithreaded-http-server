CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = server

all:
	$(CC) $(CFLAGS) server.c -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)