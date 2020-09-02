#CC=arm-none-eabi-gcc
#LD=arm-none-eabi-ld
#MACHINE=cortex-m4
#CFLAGS= -c -mcpu=$(MACHINE) -mthumb -Wall
#LDFLAGS = -nostdlib -T stm32f401re.ld -Wl,-Map=image.map
.PHONY: clean

image.elf: startup.o
	$(CC) $(LDFLAGS) *.o -o image.elf

startup.o: startup.c platform_stm32f401re.h
	$(CC) $(CFLAGS) startup.c -o startup.o

clean:
	rm -rf *.o *.map *.elf
