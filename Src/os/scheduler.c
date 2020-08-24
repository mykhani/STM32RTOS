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
 * scheduler.c
 *
 *  Created on: 11 Aug 2020
 *      Author: ykhan
 */
#include "scheduler.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../platform/platform.h"
#include "../drivers/systick_timer/systick_timer.h"

struct task_list_node *task_list_head = NULL;

uint32_t exception_stack_addr = EXCEPTION_STACK_TOP;
uint32_t process_stack_addr = PROCESS_STACK_TOP;

struct task_struct *current_task;
struct task_struct *idle_task;

uint32_t tick_counter = 0;

static void update_task_ticks(void)
{
	struct task_list_node *node; // iterator

	for (node = task_list_head; node; node = node->next) {

		// skip if it is the idle task
		if (node->task.prio == idle_task->prio) {
			continue;
		}

		if (node->task.state == TASK_WAIT && tick_counter == node->task.wait_ticks) {
			node->task.state = TASK_READY;
		}
	}
}

void SysTick_Handler(void)
{
	tick_counter++;

	update_task_ticks();

	schedule();
}

void HardFault_Handler(void)
{
	printf("In hard fault handler\n");

	while (1);
}

void MemManage_Handler(void)
{
	printf("In mem manage fault handler\n");

	while (1);
}

void BusFault_Handler(void)
{
	printf("In bus fault handler\n");

	while (1);
}

__attribute__((naked)) void UsageFault_Handler(void)  // using naked handler to avoid C epilogue/prologue modifying MSP
{
	// check which stack is used for pushing the contents: MSP or PSP?
	__asm volatile(
		"MOV r0, LR\n\t"		// read LR (EXC_RETURN)
		"TST r0, #0x4\n\t"		// check bit[2] of EXC_RETURN. 1: PSP, 0: MSP
		"BEQ 2f\n\t"
		"1:\n\t"
		"MRS r0, PSP\n\t"
		"B UsageFault_Handler_C\n\t"
		"2:\n\t"
		"MRS r0, MSP\n\t"		// AAPCS: r0 becomes first argument
		"B UsageFault_Handler_C\n\t"
	);
}

void UsageFault_Handler_C(uint32_t *stack_frame_top)
{
	printf("Stack frame top: %p\n", stack_frame_top);

	printf("Value of R0: %lx\n", stack_frame_top[0]);
	printf("Value of R1: %lx\n", stack_frame_top[1]);
	printf("Value of R2: %lx\n", stack_frame_top[2]);
	printf("Value of R3: %lx\n", stack_frame_top[3]);
	printf("Value of R12: %lx\n", stack_frame_top[4]);
	printf("Value of LR: %lx\n", stack_frame_top[4]);
	printf("Value of PC: %lx\n", stack_frame_top[6]);
	printf("Value of XPSR: %lx\n", stack_frame_top[7]);
	printf("Value of R4: %lx\n", stack_frame_top[8]);
	printf("Value of R5: %lx\n", stack_frame_top[9]);
	printf("Value of R6: %lx\n", stack_frame_top[10]);
	printf("Value of R7: %lx\n", stack_frame_top[11]);
	printf("Value of R8: %lx\n", stack_frame_top[12]);
	printf("Value of R9: %lx\n", stack_frame_top[13]);
	printf("Value of R10: %lx\n", stack_frame_top[14]);
	printf("Value of R11: %lx\n", stack_frame_top[15]);

	uint16_t *ufsr = (uint16_t *)UFSR_ADDR;

	printf("In usage fault handler\n");
	printf("Usage fault status reg: %02x\n", *ufsr);

	while (1);
}

void clear_pendsv(void)
{
	uint32_t *icsr = (uint32_t *)ICSR_ADDR;

	*icsr |= (1 << ICSR_PENDSVCLR);
}

void set_pendsv(void)
{
	uint32_t *icsr = (uint32_t *)ICSR_ADDR;

	*icsr |= (1 << ICSR_PENDSVSET);
}

void idle_task_handler(void *data)
{
	// start scheduler timer
	systick_timer_init(TICK_HZ);

	while (1) {
		// put microcontroller to sleep?
	}
}

__attribute__((naked)) void switch_to_privileged(void)
{
	__asm volatile("MRS r0, CONTROL");
	__asm volatile("BIC r0, r0, #1");	// clear bit[0] of CONTROL register
	__asm volatile("MSR CONTROL, r0");
	__asm volatile("BX LR");
}

__attribute__((naked)) void switch_to_unprivileged(void)
{
	__asm volatile("MRS r0, CONTROL");
	__asm volatile("ORR r0, r0, #1");	// set bit[0] of CONTROL register
	__asm volatile("MSR CONTROL, r0");
	__asm volatile("BX LR");
}

static void restore_task_context(struct task_struct *task)
{
	uint32_t task_stack;

	task_stack = task->stack_addr;

	__asm volatile("MOV r0, %0":: "r" (task_stack));
	//__asm volatile("LDMIA r0!, {r4-r11}");
	__asm volatile("MSR PSP, r0");
}

static void save_task_context(struct task_struct *task)
{
	uint32_t task_stack;

	/* save registers
	 * r0, r1, r2, r3, r12, lr, pc, xpsr are already saved on PSP
	 * the remaining registers to store are
	 * r4, r5, r6, r7, r8, r9, r10, r11
	 */
	__asm volatile("MRS r0, PSP");
	//__asm volatile("STMDB r0!, {r4-r11}");
	__asm volatile("MOV %0, r0":"=r" (task_stack));


	task->stack_addr = task_stack;
}

struct task_struct *get_current_task(void)
{
	/*
	 * Two ways this could be implemented (platform dependent):
	 * 1. Either we store the struct task_struct of the current task at
	 *    the end of the kernel stack (MSP). Consumes memory
	 * 2. We use one of the general purpose registers to store the pointer
	 *    to the current task. I will go for this approach for now as we have
	 *    plenty of registers on Cortex M4 and use R12 to keep track of current
	 *    task. Remember R12 is also stored on process stack upon exception
	 *    entry. Tried this it fails badly, turns out some library functions (printf)
	 *    modify r12 contents.
	 */

	return current_task;
}

static void save_current_task(struct task_struct *task)
{
	current_task = task;
}

static void set_exception_stack_space(uint32_t stack_addr)
{
	__asm volatile("MSR MSP, R0");
}

__attribute__((naked)) void switch_sp_to_psp(uint32_t stack_addr)
{
	__asm volatile("MSR PSP, R0");
	__asm volatile("MOV R0, #0x2");
	__asm volatile("MSR CONTROL, R0");
	__asm volatile("BX LR");
}

static void switch_task(struct task_struct *task)
{

	// perform context switch i.e.
	// restore PSP
	if (task->npriv) {
		//switch_to_unprivileged();
	} else {
		//switch_to_privileged();
	}

	// restore registers {r4-r11} -> stack frame 2
	restore_task_context(task);

	//save_current_task(task);  // make this task the current task
}

static void do_scheduling(void)
{
	struct task_struct *current_task = get_current_task();
	struct task_struct *scheduled_task = NULL; // new task to be scheduled
	int highest_prio = 0;

	/* check for task which is:
	 * 1. ready and not current task
	 * 2. has the highest priority
	 * if found: switch to that task.
	 * otherwise, schedule the current task again
	 */
	// TODO: implement priority queue maybe for keeping the tasks instead
	// of the plain linked-list
	struct task_list_node *node = NULL; // iterator

	for (node = task_list_head; node; node = node->next) {

		// skip if it is the idle task
		if (node->task.prio == idle_task->prio) {
			continue;
		}

		// skip if task not ready
		if (node->task.state != TASK_READY) {
			continue;
		}

		if (node->task.prio > highest_prio) {
			scheduled_task = &node->task;
			highest_prio = node->task.prio;
		}
	}

	// If no such task is found, run the idle task
	if (scheduled_task == NULL) {
		scheduled_task = idle_task;
	} else if (scheduled_task == current_task) {
		return; // same as current task, no need to switch
	}

	// save current task's context
	if (current_task) {
		save_task_context(current_task);
	}

	// perform context switch
	switch_task(scheduled_task);

	save_current_task(scheduled_task);
}

__attribute__((naked)) void PendSV_Handler(void)
{
	// save r4 - r11 on PSP (the registers not yet saved by exception stacking)
	__asm volatile("MRS R0, PSP");
	__asm volatile("STMDB R0!, {R4-R11}");
	__asm volatile("MSR PSP, R0");
	__asm volatile("PUSH {LR}");

	// call do_scheduling to determine task to switch to
	// effectively, do_scheduling updates the PSP corresponding to the current task to run
	__asm volatile("BL do_scheduling");
	// restore r4 - r11, if os_schedule didn't switch any task, same PSP is used
	// to restore r4 - r11, else the switched task's PSP is used to restore
	// r4 - r11
	__asm volatile("MRS R0, PSP");
	__asm volatile("LDMIA R0!, {R4-R11}");
	__asm volatile("MSR PSP, R0");
	__asm volatile("POP {LR}");
	__asm volatile("BX LR");
}

static void enable_processor_faults()
{
	uint32_t *shcsr = (uint32_t *)SHCSR_ADDR;

	*shcsr |= (1 << SHCSR_USGFAULTENA);
	*shcsr |= (1 << SHCSR_BUSFAULTENA);
	*shcsr |= (1 << SHCSR_MEMFAULTENA);
}

void schedule()
{
	set_pendsv();
}

void os_start(void)
{
	// change to non-privileged thread mode so that tasks run in non-privileged mode
	// start scheduling tasks

	// initialize systick timer for scheduler

	/* allocate MSP and PSP
	 * MSP will be used by the kernel and handler mode (exceptions, interrupts)
	 * PSP will be used by the individual tasks
	 */
	idle_task = __task_create(idle_task_handler,
							  NULL /* argument */,
							  1024 /* 1KB task stack */,
							  0 /* lowest priority */,
							  0 /* privileged */);

	switch_sp_to_psp(idle_task->stack_addr);

	set_exception_stack_space(exception_stack_addr);

	save_current_task(idle_task);

	idle_task_handler(NULL);
}

void os_init(void)
{
	// By default MSP is active, in PRIVLIGED access mode
	enable_processor_faults();
}
