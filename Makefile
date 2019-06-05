BIN = kaopack

CC = gcc
CFLAGS = -g -Wall -pedantic
LD = gcc
LDFLAGS =

SRCS = $(wildcard src/*.c)
DEPS = $(wildcard deps/*/*.c)
OBJS = $(SRCS:.c=.o)
DEPOBJS = $(DEPS:.c=.o)

.PHONY: all
all: $(BIN)

$(BIN): $(OBJS) $(DEPOBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ -Ideps $(CFLAGS)

.PHONY: clean
clean:
	-rm $(BIN)
	-rm $(OBJS)
	-rm $(DEPOBJS)
