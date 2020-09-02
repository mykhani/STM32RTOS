include ../parameters.mk

.PHONY: clean

all: built-in.o

SRCS = $(wildcard *.c)
SRCS := $(SRCS) $(wildcard clk/*.c)
SRCS := $(SRCS) $(wildcard gpio/*.c)
SRCS := $(SRCS) $(wildcard pinmux/*.c)
SRCS := $(SRCS) $(wildcard systick_timer/*.c)

OBJS=$(SRCS:.c=.o)

built-in.o: $(OBJS)
	@echo srcs: $(SRCS)
	#$(AR) $(ARFLAGS) $@ $^
	#$(CC) -Wl,-r $^ -o $@
	$(LD) -r -o $@ $^

clean:
	rm -rf *.o
	rm -f $(OBJS)
