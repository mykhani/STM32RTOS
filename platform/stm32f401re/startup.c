/*******************************************************************************
 * Copyright (C) 2020, Yasir Khan <yasir_electronics@yahoo.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/
/*
 * startup.c
 *
 *  Created on: Aug 31, 2020
 *      Author: ykhan
 */

#include <stdint.h>
#include "platform_stm32f401re.h"

extern uint8_t _la_data;
extern uint8_t _sdata;
extern uint8_t _edata; 
extern uint8_t _sbss;
extern uint8_t _ebss;

extern int main(void);
extern void __libc_init_array(void);

void default_handler(void)
{
	while (1);
}

void initialize_bss(uint8_t *bss_addr, uint32_t size)
{
	uint32_t i = 0;

	for (i = 0; i < size; i++, bss_addr++) {
		*bss_addr = 0;
	}
}

void relocate_data_section(uint8_t *dst, uint8_t *src, uint32_t size)
{
	uint32_t i = 0;

	for (i = 0; i < size; i++, dst++, src++) {
		*dst = *src;
	}
}

void reset_handler(void)
{
	/* if start of .data section doesn't coincide with the end of .text section (FLASH address),
	   it means the .data section should be reloacted to RAM
	*/
	uint32_t data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
	uint32_t bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	uint8_t *data_load = &_la_data;
	uint8_t *data_start = &_sdata;
	uint8_t *bss_start = &_sbss;

	if (data_start != data_load) {
		relocate_data_section(data_start, data_load, data_size);	
	}

	initialize_bss(bss_start, bss_size);

	/* initialize std C library */
	__libc_init_array();

	main();
}

void NMI_handler(void) __attribute__((weak, alias("default_handler")));	// NMI_handler is weak function and an alias for default handler
void hard_fault_handler(void) __attribute__((weak, alias("default_handler")));
void mem_manage_handler(void) __attribute__((weak, alias("default_handler")));
void bus_fault_handler(void) __attribute__((weak, alias("default_handler")));
void usage_fault_handler(void) __attribute__((weak, alias("default_handler")));
void svcall_handler(void) __attribute__((weak, alias("default_handler")));
void pendsv_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void) __attribute__((weak, alias("default_handler")));

__attribute__((section(".vectors"))) uint32_t vector_tbl[] = {
	RAM_START + RAM_SIZE,		/* MSP */
	(uint32_t)&reset_handler,	/* Reset */
	(uint32_t)&NMI_handler,		/* NMI */
	(uint32_t)&hard_fault_handler,	/* HardFault */
	(uint32_t)&mem_manage_handler,	/* MemManage */
	(uint32_t)&bus_fault_handler,	/* BusFault */
	(uint32_t)&usage_fault_handler,	/* UsageFault */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&svcall_handler,	/* SVCall */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&default_handler,	/* Reserved */
	(uint32_t)&pendsv_handler,	/* PendSV */
	(uint32_t)&systick_handler,	/* Systick */
	(uint32_t)&default_handler,	/* IRQ0 */
	(uint32_t)&default_handler,	/* IRQ1 */
	(uint32_t)&default_handler,	/* IRQ2 */
	(uint32_t)&default_handler,	/* IRQ3 */
	(uint32_t)&default_handler,	/* IRQ4 */
	(uint32_t)&default_handler,	/* IRQ5 */
	(uint32_t)&default_handler,	/* IRQ6 */
	(uint32_t)&default_handler,	/* IRQ7 */
	(uint32_t)&default_handler,	/* IRQ8 */
	(uint32_t)&default_handler,	/* IRQ9 */
	(uint32_t)&default_handler,	/* IRQ10 */
	(uint32_t)&default_handler,	/* IRQ11 */
	(uint32_t)&default_handler,	/* IRQ12 */
	(uint32_t)&default_handler,	/* IRQ13 */
	(uint32_t)&default_handler,	/* IRQ14 */
	(uint32_t)&default_handler,	/* IRQ15 */
	(uint32_t)&default_handler,	/* IRQ16 */
	(uint32_t)&default_handler,	/* IRQ17 */
	(uint32_t)&default_handler,	/* IRQ18 */
	(uint32_t)0x0,			/* IRQ19 */
	(uint32_t)0x0,			/* IRQ20 */
	(uint32_t)0x0,			/* IRQ21 */
	(uint32_t)0x0,			/* IRQ22 */
	(uint32_t)&default_handler,	/* IRQ23 */
	(uint32_t)&default_handler,	/* IRQ24 */
	(uint32_t)&default_handler,	/* IRQ25 */
	(uint32_t)&default_handler,	/* IRQ26 */
	(uint32_t)&default_handler,	/* IRQ27 */
	(uint32_t)&default_handler,	/* IRQ28 */
	(uint32_t)&default_handler,	/* IRQ29 */
	(uint32_t)&default_handler,	/* IRQ30 */
	(uint32_t)&default_handler,	/* IRQ31 */
	(uint32_t)&default_handler,	/* IRQ32 */
	(uint32_t)&default_handler,	/* IRQ33 */
	(uint32_t)&default_handler,	/* IRQ34 */
	(uint32_t)&default_handler,	/* IRQ35 */
	(uint32_t)&default_handler,	/* IRQ36 */
	(uint32_t)&default_handler,	/* IRQ37 */
	(uint32_t)&default_handler,	/* IRQ38 */
	(uint32_t)0x0		,	/* IRQ39 */
	(uint32_t)&default_handler,	/* IRQ40 */
	(uint32_t)&default_handler,	/* IRQ41 */
	(uint32_t)&default_handler,	/* IRQ42 */
	(uint32_t)0x0,			/* IRQ43 */
	(uint32_t)0x0,			/* IRQ44 */
	(uint32_t)0x0,			/* IRQ45 */
	(uint32_t)0x0,			/* IRQ46 */
	(uint32_t)&default_handler,	/* IRQ47 */
	(uint32_t)0x0		,	/* IRQ48 */
	(uint32_t)&default_handler,	/* IRQ49 */
	(uint32_t)&default_handler,	/* IRQ50 */
	(uint32_t)&default_handler,	/* IRQ51 */
	(uint32_t)0x0,			/* IRQ52 */
	(uint32_t)0x0,			/* IRQ53 */
	(uint32_t)0x0,			/* IRQ54 */
	(uint32_t)0x0,			/* IRQ55 */
	(uint32_t)&default_handler,	/* IRQ56 */
	(uint32_t)&default_handler,	/* IRQ57 */
	(uint32_t)&default_handler,	/* IRQ58 */
	(uint32_t)&default_handler,	/* IRQ59 */
	(uint32_t)&default_handler,	/* IRQ60 */
	(uint32_t)0x0,			/* IRQ61 */
	(uint32_t)0x0,			/* IRQ62 */
	(uint32_t)0x0,			/* IRQ63 */
	(uint32_t)0x0,			/* IRQ64 */
	(uint32_t)0x0,			/* IRQ65 */
	(uint32_t)0x0,			/* IRQ66 */
	(uint32_t)&default_handler,	/* IRQ67 */
	(uint32_t)&default_handler,	/* IRQ68 */
	(uint32_t)&default_handler,	/* IRQ69 */
	(uint32_t)&default_handler,	/* IRQ70 */
	(uint32_t)&default_handler,	/* IRQ71 */
	(uint32_t)&default_handler,	/* IRQ72 */
	(uint32_t)&default_handler,	/* IRQ73 */
	(uint32_t)0x0,			/* IRQ74 */
	(uint32_t)0x0,			/* IRQ75 */
	(uint32_t)0x0,			/* IRQ76 */
	(uint32_t)0x0,			/* IRQ77 */
	(uint32_t)0x0,			/* IRQ78 */
	(uint32_t)0x0,			/* IRQ79 */
	(uint32_t)0x0,			/* IRQ80 */
	(uint32_t)&default_handler,	/* IRQ81 */
	(uint32_t)0x0,			/* IRQ82 */
	(uint32_t)0x0,			/* IRQ83 */
	(uint32_t)&default_handler,	/* IRQ84 */
};
