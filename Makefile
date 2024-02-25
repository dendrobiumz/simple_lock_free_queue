CC=gcc
OBJS = lfq.o
CFLAGS = 

all: test

%.o: %.c
  $(CC) -c -o $@ $<

test: $(OBJS)
	$(CC)