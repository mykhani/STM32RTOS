export MACHINE = cortex-m4
export LINKERSCRIPT = ../platform/stm32f401re/stm32f401re.ld
export OPENOCD_BOARD_CFG = st_nucleo_f4.cfg
export USE_SEMIHOSTING = 0

include ../common.mk

image.elf: main.o board_init.o $(LIB_RTOS)
	$(CC) $(LDFLAGS) $^ -o $@
	
main.o: main.c
	$(CC) $(CFLAGS) $^ -o $@ -I$(ROOTDIR)/include 

board_init.o: board_init.c
	$(CC) $(CFLAGS) $^ -o $@ -I$(ROOTDIR)/include

clean: clean_system
	rm -rf *.o *.elf *.map
