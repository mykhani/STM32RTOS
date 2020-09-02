include ../parameters.mk

.PHONY: clean

all: built-in.o

SRCS = $(wildcard *.c)
OBJS=$(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

built-in.o: $(OBJS)
	@echo srcs: $(SRCS)
	#$(CC)  $(CFLAGS) -Wl,-r $^ -o $@
	$(LD) -r -o $@ $^

clean:
	rm -rf *.o
