CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar

ifeq ($(USE_SEMIHOSTING),1)
	SPECS = rdimon.specs
else
	SPECS = nano.specs
endif

CFLAGS = -c -mcpu=$(MACHINE) -mthumb -mfloat-abi=soft -std=gnu11 -Wall -O0 -I$(ROOTDIR)/include
LDFLAGS = -mcpu=$(MACHINE) -mthumb -mfloat-abi=soft --specs=$(SPECS) -T $(LINKERSCRIPT) -Wl,-Map=image.map
ARFLAGS = -crs
