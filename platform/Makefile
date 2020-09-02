include ../parameters.mk

.PHONY: clean

all: built-in.o

SRCS = $(wildcard *.c)
SRCS := $(SRCS) $(wildcard stm32f401re/*.c)
OBJS=$(SRCS:.c=.o)

ifeq ($(USE_SEMIHOSTING),1)
	SRCS := $(filter-out syscalls.c, $(SRCS))
endif

built-in.o: $(OBJS)
	@echo srcs: $(SRCS)
	#$(AR) $(ARFLAGS) $@ $^
	#$(CC) -Wl,-r $^ -o $@
	$(LD) -r -o $@ $^

clean:
	rm -rf *.o
	rm -f $(OBJS)
