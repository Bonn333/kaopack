PROG=kaopack

CC=gcc
CFLAGS=
LD=gcc
LDFLAGS=

SRCDIR := src
OBJDIR := obj
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	-rm $(PROG)
	-rm $(OBJS)
	-rmdir $(OBJDIR)
