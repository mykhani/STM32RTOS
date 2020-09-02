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
 * gpio-private.h
 *
 *  Created on: Aug 26, 2020
 *      Author: ykhan
 */

#ifndef DRIVERS_GPIO_GPIO_PRIVATE_H_
#define DRIVERS_GPIO_GPIO_PRIVATE_H_

#include <clk.h>
#include <list.h>
#include <stdint.h>

struct gpio_desc {
	struct gpio_chip *chip;
	const char *label;
	uint32_t flags;
};

struct gpio_chip;

struct gpio_chip_ops {
	int  (*get)(struct gpio_chip *chip, uint8_t offset);
	void (*set)(struct gpio_chip *chip, uint8_t offset, int value);
	int (*direction_input)(struct gpio_chip *chip, uint8_t offset);
	int (*direction_output)(struct gpio_chip *chip, uint8_t offset, int value);
};

struct gpio_chip {
	const char *name;
	uint32_t base;
	uint16_t ngpio;
	uint32_t clk_id;
	struct clk *clk;
	struct list_head list;
	struct gpio_desc *descs;
	struct gpio_chip_ops *ops;
};

int gpiochip_probe(void);

#endif /* DRIVERS_GPIO_GPIO_PRIVATE_H_ */
