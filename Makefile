CC = gcc
CFLAGS = --std=gnu99 -Wall -Werror -Wextra -Wshadow -pedantic
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
TARGET := smallsh

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(TARGET)
