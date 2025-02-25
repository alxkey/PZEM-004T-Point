CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = 
SRCS = app.c ini.c logger.c checksum.c pzem.c
OBJS = $(SRCS:.c=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
