CC = gcc
CFLAGS = -Wall -Wextra -g -O2
TARGET = a8
SRCS = main.c graph.c
OBJS = $(SRCS:.c=.o)
HEADERS = graph.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)