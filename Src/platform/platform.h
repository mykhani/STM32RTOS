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
 * platform.h
 *
 *  Created on: 11 Aug 2020
 *      Author: ykhan
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#define DISABLE_INTERRUPTS()										\
							do {									\
								__asm volatile("MOV R0, #0x1");		\
								__asm volatile("MSR PRIMASK, R0");	\
							} while (0)


#define ENABLE_INTERRUPTS()											\
							do {									\
								__asm volatile("MOV R0, #0x0");		\
								__asm volatile("MSR PRIMASK, R0");	\
							} while (0)

#define RAM_START	0x20000000UL
#define RAM_SIZE	(96 * 1024UL)	// RAM size is 96KB

#define EXCEPTION_STACK_SIZE	(32 * 1024UL) // 32KB
#define EXCEPTION_STACK_TOP	(RAM_START + RAM_SIZE)

#define PROCESS_STACK_SIZE	(RAM_SIZE - EXCEPTION_STACK_SIZE)
#define PROCESS_STACK_TOP (EXCEPTION_STACK_TOP - EXCEPTION_STACK_SIZE)

/* clocks configuration */
// default system clk is HCI with 16MHz frequency
#define PRESCALAR_DIV_BY_1	(1)
#define PRESCALAR_DIV_BY_8	(1/8)

#define HSI_CLK	(16000000UL)
#define HSE_CLK	(24000000UL)

#define SYSCLK	HSI_CLK

#define SYSTICK_PRESCALAR	PRESCALAR_DIV_BY_1
#define SYSTICK_CLK	(SYSCLK * SYSTICK_PRESCALAR)

#define TICK_HZ	(1000U)	// 1000 HZ systick frequency

#define MSEC_PER_TICK 	(1000 / TICK_HZ)

// Micro-controller registers

#define SHCSR_ADDR		0xE000ED24

#define SHCSR_USGFAULTENA	18
#define SHCSR_BUSFAULTENA	17
#define SHCSR_MEMFAULTENA	16
#define SHCSR_PENDSVACT		10

#define ICSR_ADDR			0xE000ED04
#define ICSR_PENDSVSET		28
#define ICSR_PENDSVCLR		27

#define UFSR_ADDR		0xE000ED2A

#endif /* PLATFORM_H_ */
