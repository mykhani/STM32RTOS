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
 * systick_timer_drv.c
 *
 *  Created on: 13 Aug 2020
 *      Author: ykhan
 */
#include "systick_timer.h"

#include <stdio.h>

#include "../../os/scheduler.h"
#include "../../platform/platform.h"

void systick_timer_init(uint32_t tick_hz)
{
	uint32_t count_value = SYSTICK_CLK / tick_hz;
	uint32_t *syst_rvr = (uint32_t *)SYST_RVR;	// only lower 24-bits valid
	uint32_t *syst_csr = (uint32_t *)SYST_CSR;

	// check if value of tick_hz fits inside the lower 24-bits of SYST_RVR register
	if (tick_hz & 0xFF000000) {
		printf("ERROR: value of TICK_HZ is greater than the systick counter register.\n");
		while (1);
	}

	// clear the counter reload value
	*syst_rvr &= ~(0x00FFFFFF);
	// load the counter reload value
	*syst_rvr |= (count_value - 1); // let say we want systick interrupt after 4 systick clk cycles,
	                             // if we put 4 into reload registers, the interrupt is generated
	                             // on the 5th cycle i.e.
	                             //   value of counter                      systick interrupt - value reloaded
	                             //      ^                                  ^
	                             //      4  ->  3  ->  2  ->  1  ->  0  ->  4
	                             //      1 clk  1 clk  1 clk  1 clk  1 clk
	                             //  i.e. systick interrupt comes afer 5 cycles. Therefore, alway
	                             // put value which is 1 less than the required number of systick clk cycles
	// reset systick clk src
	*syst_csr &= ~(1 << SYST_CSR_CLKSOURCE);
	// set processor clk as systick src
	*syst_csr |= (SYST_CLK_SRC_PROCESSOR << SYST_CSR_CLKSOURCE);
	// enable systick timer interrupt
	*syst_csr |= (1 << SYST_CSR_TICKINT);
	// enable the systick timer
	*syst_csr |= (1 << SYST_CSR_ENABLE);
}
