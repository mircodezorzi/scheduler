CC := clang
CFLAGS := -std=c99 -pedantic

HDRS :=
SRCS := main.c curse.c utils.c process.c #schedule.c queue.c process.c utils.c

OBJS := $(SRCS:.c=.o)
EXEC := scheduler
all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
