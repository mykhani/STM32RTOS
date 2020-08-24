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
 * task.h
 *
 *  Created on: 23 Aug 2020
 *      Author: ykhan
 */

#ifndef OS_TASK_H_
#define OS_TASK_H_

#include <stdint.h>

typedef void (*task_fn) (void *);

typedef enum _task_state {
	TASK_WAIT,
	TASK_READY
} task_state;

struct task_struct {
	task_fn fn;
	void *data;
	uint8_t prio;
	int8_t npriv;	// non-privileged task (user task vs kernel space task)
	task_state state;
	uint32_t wait_ticks;
	uint32_t stack_addr;
	uint32_t stack_size;
};

struct task_list_node {
	struct task_struct task;
	struct task_list_node *next;
};

/* internal functions */
struct task_struct *__task_create(task_fn fn, void *data, uint32_t stk_sz,
		                          uint8_t prio, uint8_t npriv);

void task_yield(void);
void task_delay_ms(uint32_t ms);
struct task_struct *get_current_task(void);
int task_create(task_fn fn, void *data, uint32_t stk_sz, uint8_t prio);

#endif /* OS_TASK_H_ */
