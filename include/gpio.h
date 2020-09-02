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
 * gpio.h
 *
 *  Created on: 22 Aug 2020
 *      Author: ykhan
 */

#ifndef DRIVERS_GPIO_GPIO_H_
#define DRIVERS_GPIO_GPIO_H_

#include <list.h>
#include <util.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>	//TODO: implement DEBUG header

#define GPIO_FLAGS_BIT_DIR_SET		BIT(0)
#define GPIO_FLAGS_BIT_DIR_OUT		BIT(1)
#define GPIO_FLAGS_BIT_DIR_VAL		BIT(2)
#define GPIO_FLAGS_BIT_OPEN_DRAIN	BIT(3)
#define GPIO_FLAGS_BIT_NONEXCLUSIVE	BIT(4)

enum gpio_flags {
	GPIO_ASIS		= 0,
	GPIO_IN			= GPIO_FLAGS_BIT_DIR_SET,
	GPIO_OUT_LOW	= GPIO_FLAGS_BIT_DIR_SET | GPIO_FLAGS_BIT_DIR_OUT,
	GPIO_OUT_HIGH	= GPIO_FLAGS_BIT_DIR_SET | GPIO_FLAGS_BIT_DIR_OUT |
					  GPIO_FLAGS_BIT_DIR_VAL,
	GPIO_OUT_LOW_OPEN_DRAIN = GPIO_OUT_LOW | GPIO_FLAGS_BIT_OPEN_DRAIN,
	GPIO_OUT_HIGH_OPEN_DRAIN = GPIO_OUT_HIGH | GPIO_FLAGS_BIT_OPEN_DRAIN,
};

struct gpio_desc;

struct gpio_lookup {
	const char *pin_label;
	const char *chip_label;
	uint16_t pin_hwnum;
};

struct gpio_lookup_table {
	const char *key;
	struct list_head list;
	struct gpio_lookup table[];
};

#define GPIO_LOOKUP(_pin_label, _chip_label, _pin_hwnum)	\
{ 															\
	.pin_label = _pin_label,								\
	.chip_label = _chip_label,								\
	.pin_hwnum = _pin_hwnum									\
}

int gpio_init(void);
int gpio_get_value(struct gpio_desc *desc);
int gpio_get_direction(struct gpio_desc *desc);
uint8_t gpio_get_hwnum(struct gpio_desc *desc);
int gpio_direction_input(struct gpio_desc *desc);
void gpio_set_value(struct gpio_desc *desc, int value);
void gpio_add_lookup_table(struct gpio_lookup_table *table);
int gpio_direction_output(struct gpio_desc *desc, int value);
struct gpio_desc *gpio_get(const char *pin_label, enum gpio_flags flags);

#endif /* DRIVERS_GPIO_GPIO_H_ */
