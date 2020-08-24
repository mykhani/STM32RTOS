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
 * os_syscalls.c
 *
 *  Created on: 12 Aug 2020
 *      Author: ykhan
 */

#include "syscalls.h"

#include "scheduler.h"

void handle_svc(uint8_t svcnum, uint32_t **return_values, uint32_t *num_return_values)
{
	*return_values = NULL;
	*num_return_values = 0;

	switch (svcnum) {
	case 0:
		switch_to_privileged();
	}
}

void os_syscall_handler(uint32_t *stack_ptr)
{
	uint32_t program_counter = stack_ptr[6];
	uint32_t *svc_instruction = (uint32_t *)(program_counter - 2); // Thumb mode: previous instruction is 2 bytes before the PC value
	uint8_t svcnum = *svc_instruction & 0xFF;		// lower byte of svc instruction is the svc number
	uint32_t *return_values;				// used to keep track of the return values for the syscall
	uint32_t num_return_values;				// used to keep track of the number of return values for the syscall

	handle_svc(svcnum, &return_values, &num_return_values);

}

__attribute__((naked)) void SVC_Handler(void)
{
	// decode EXC_RETURN to determine if the interrupted context was using MSP or PSP
	/* saved stack frame for interrupted context
	 * XPSR
	 * PC
	 * LR
	 * R12
	 * R3
	 * R2
	 * R1
	 * R0
	 */
	__asm volatile(
		"MOV r1, LR\n\t"
		"TST r1, #4\n\t"			// check bit[2], 0: MSP, 1: PSP
		"BEQ 2f \n\t"				// bit[2] = 0
		"1: MRS r0, PSP \n\t"		// interrupted SP is PSP
		"B os_syscall_handler \n\t"
		"2: MRS r0, MSP \n\t"		// interrupted SP is MSP
		"B os_syscall_handler \n\t"
	);
}

