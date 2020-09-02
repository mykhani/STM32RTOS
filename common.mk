
export ROOTDIR = $(dir $(lastword $(MAKEFILE_LIST)))
APP_DIR = $(dir $(firstword $(MAKEFILE_LIST)))

include $(ROOTDIR)/parameters.mk
LIB_RTOS = $(ROOTDIR)librtos.a

LIB_RTOS_DIRS = os platform drivers

LIB_RTOS_OBJS = $(patsubst %, $(ROOTDIR)/%/built-in.o, $(LIB_RTOS_DIRS))

.PHONY: clean_system $(LIB_RTOS_DIRS) $(LIB_RTOS_OBJS) load

all: $(LIB_RTOS) APP

APP: $(LIB_RTOS)
	$(MAKE) -C $(APP_DIR) image.elf

$(LIB_RTOS): $(LIB_RTOS_OBJS)
	@echo $(LIB_RTOS_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(LIB_RTOS_OBJS): $(LIB_RTOS_DIRS)
	@echo librtos objs: $(LIB_RTOS_OBJS)

$(LIB_RTOS_DIRS):
	@echo going into subdir $@
	$(MAKE) -C $(ROOTDIR)$@

clean_system: 
	$(foreach dir, $(LIB_RTOS_DIRS), $(MAKE) -C $(ROOTDIR)$(dir) clean;)
	rm -f $(LIB_RTOS)

load: APP
	$(ROOTDIR)/tools/xpack-openocd-0.10.0-14/bin/openocd -f board/$(OPENOCD_BOARD_CFG)
