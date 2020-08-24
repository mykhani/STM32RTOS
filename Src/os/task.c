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
 * task.c
 *
 *  Created on: 23 Aug 2020
 *      Author: ykhan
 */
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../os/scheduler.h"
#include "../platform/platform.h"

extern uint32_t exception_stack_addr;
extern uint32_t process_stack_addr;
extern struct task_list_node *task_list_head; // defined in scheduler.c
extern struct task_struct *idle_task;
extern uint32_t tick_counter;

void task_delay_ms(uint32_t ms)
{
	struct task_struct *current_task = get_current_task();
	uint32_t delay_ticks;

	DISABLE_INTERRUPTS();

	if (current_task && current_task != idle_task) {
		delay_ticks = (ms + MSEC_PER_TICK - 1) / MSEC_PER_TICK; // align to minimum possible milliseconds per tick

		current_task->wait_ticks = tick_counter + delay_ticks; // optimization, instead of actually storing the
	                                                      // delay ticks, we store future snapshot of global_tick_counter
														  // when task should become ready. This way, we don't have to
	                                                      // decrement task->wait_ticks for each of the task and just
	                                                      // compare the value with the global_tick_counter value and
	                                                      // task becomes ready when wait_ticks >= global_tick_counter
		current_task->state = TASK_WAIT;

		// invoke scheduler to switch out the task
		schedule();
	}
	ENABLE_INTERRUPTS();
}

void task_yield(void)
{
	schedule();
}

static void create_dummy_context(struct task_struct *task)
{
	uint32_t stack_addr = task->stack_addr;
	uint32_t entry_addr = (uint32_t)task->fn;
	int i;

	__asm volatile("MOV R0, %0":: "r" (stack_addr));
	__asm volatile("MOV R1, #0"); // xPSR
	__asm volatile("MOV R2, #1");
	__asm volatile("ORR R1, R1, R2, LSL #24"); // set T-bit, bit[24]
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("MOV R1, %0":: "r" (entry_addr)); // PC
	__asm volatile("ORR R1, R1, #1"); // set LSB of PC (T-bit)
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("LDR R1, =0xFFFFFFFD"); // LR -> return to thread mode, exception return
	                                       // uses non-floating-point state from the PSP and
										   // execution uses PSP after return
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("MOV R1, %0":: "r" (task)); // R12 = current task pointer
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("LDR R1, =0xDEADBEEF"); // R3
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("LDR R1, =0xDEADBEEF"); // R2
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("LDR R1, =0xDEADBEEF"); // R1
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	__asm volatile("LDR R1, =0xDEADBEEF"); // R0
	__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store

	// save the remaining R4 - R11 registers (8 registers)
	for (i = 0; i < 8; i++) {
		__asm volatile("LDR R1, =0xDEADBEEF");
		__asm volatile("STR R1, [R0, #-4]!"); // pre-indexed store
	}

	__asm volatile("MOV %0, R0": "=r" (stack_addr));

	task->stack_addr = stack_addr;
}

static void add_task(struct task_list_node **list_head, struct task_list_node *task)
{
	task->next = *list_head;
	*list_head = task;
}

static uint32_t allocate_task_stack(uint32_t stk_sz)
{
	uint32_t stack_addr = 0x0;

	// check if space available
	if (process_stack_addr - stk_sz > RAM_START) {
		stack_addr = process_stack_addr;
		process_stack_addr = process_stack_addr - stk_sz; // update the user stack end for next allocation
	}

	return stack_addr;
}

static int task_create_helper(struct task_struct *task, task_fn fn,
		                      void *data, uint32_t stk_sz, uint8_t prio, uint8_t npriv)
{
	uint32_t stack_addr = allocate_task_stack(stk_sz);
	if (stack_addr == 0)
		return -1;

	task->fn = fn;
	task->npriv = npriv;
	task->prio = prio;
	task->data = data;
	task->state = TASK_READY;
	task->stack_size = stk_sz;
	task->stack_addr = stack_addr;

	return 0;
}

struct task_struct *__task_create(task_fn fn, void *data, uint32_t stk_sz,
		                          uint8_t prio, uint8_t npriv)
{
	// create a task and add it to READY queue
	struct task_list_node *task_node = malloc(sizeof(struct task_list_node));
	if (!task_node) {
		printf("ERROR: failed to allocate memory for task node\n");
		return NULL;
	}

	memset(task_node, 0, sizeof(*task_node));

	task_create_helper(&task_node->task, fn, data, stk_sz, prio, npriv);

	add_task(&task_list_head, task_node);

	return &task_node->task;
}

int task_create(task_fn fn, void *data, uint32_t stk_sz, uint8_t prio)
{
	struct task_struct *task;

	task = __task_create(fn, data, stk_sz, prio, 1);

	if (task == NULL)
		return -1;

	/* create a dummy task context since when task is created, it doesn't contain any context */
	create_dummy_context(task);

	return 0;
}
