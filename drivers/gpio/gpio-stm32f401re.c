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
 * gpio-stm32f401re.c
 *
 *  Created on: Aug 26, 2020
 *      Author: ykhan
 */
#include <clk.h>
#include <util.h>
#include <gpio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio-private.h"
#include <clocks/stm32f401re-clks.h>

// GPIO registers
#define GPIOA_BASE	0x40020000
#define GPIOB_BASE	0x40020400
#define GPIOC_BASE	0x40020800
#define GPIOD_BASE	0x40020C00
#define GPIOE_BASE	0x40021000
#define GPIOH_BASE	0x40021C00

#define GPIOx_MODER_OFFSET		0x0
#define GPIOx_OTYPER_OFFSET		0x4
#define GPIOx_OSPEEDR_OFFSET		0x8
#define GPIOx_PUPDR_OFFSET		0xC
#define GPIOx_IDR_OFFSET		0x10
#define GPIOx_ODR_OFFSET		0x14
#define GPIOx_BSRR_OFFSET		0x18
#define GPIOx_AFRL_OFFSET		0x20
#define GPIOx_AFRH_OFFSET		0x24

struct list_head gpio_chips;
extern struct list_head gpio_lookup_list;

int stm32f401re_get(struct gpio_chip *chip, uint8_t offset)
{
	return 0;
}

void stm32f401re_set(struct gpio_chip *chip, uint8_t offset, int value)
{
	volatile uint32_t *gpio_bsrr = (uint32_t *)(chip->base + GPIOx_BSRR_OFFSET);

	if (value) {
		// set the bit, LSB of BSRR register
		*gpio_bsrr |= (1 << offset);
	} else {
		// reset the bit, MSB of BSRR register
		*gpio_bsrr |= (1 << (offset + 16));
	}
//	volatile uint32_t *gpio_odr = (uint32_t *)(chip->base + GPIOx_ODR_OFFSET);
//
//	if (value) {
//		*gpio_odr |= (1 << offset);
//	} else {
//		*gpio_odr &= ~(1 << offset);
//	}
}

int stm32f401re_direction_input(struct gpio_chip *chip, uint8_t offset)
{
	volatile uint32_t *gpio_mode = (uint32_t *)(chip->base + GPIOx_MODER_OFFSET);

	return 0;
}

int stm32f401re_direction_output(struct gpio_chip *chip, uint8_t offset, int value)
{
	volatile uint32_t *gpio_mode = (uint32_t *)(chip->base + GPIOx_MODER_OFFSET);

	// reset the mode
	*gpio_mode &= ~(0x3 << (offset * 2));

	*gpio_mode |= (0x1 << (offset * 2));

	return 0;
}

struct gpio_chip_ops stm32f401re_gpio_ops = {
		.get				= stm32f401re_get,
		.set 				= stm32f401re_set,
		.direction_output	= stm32f401re_direction_output,
		.direction_input	= stm32f401re_direction_input,
};

struct gpio_chip gpio_a = {
		.name = "gpio.a",
		.ngpio = 16,
		.clk_id = CLK_GPIOA,
		.base 	= GPIOA_BASE,
		.ops 	= &stm32f401re_gpio_ops
};

struct gpio_chip gpio_b = {
		.name = "gpio.b",
		.ngpio = 16,
		.clk_id = CLK_GPIOB,
		.base 	= GPIOB_BASE,
		.ops 	= &stm32f401re_gpio_ops
};

struct gpio_chip gpio_c = {
		.name = "gpio.c",
		.ngpio = 16,
		.clk_id = CLK_GPIOC,
		.base = GPIOC_BASE,
		.ops = &stm32f401re_gpio_ops
};

struct gpio_chip gpio_d = {
		.name = "gpio.d",
		.ngpio = 16,
		.clk_id = CLK_GPIOD,
		.base = GPIOD_BASE,
		.ops = &stm32f401re_gpio_ops
};

struct gpio_chip gpio_e = {
		.name = "gpio.e",
		.ngpio = 16,
		.clk_id = CLK_GPIOE,
		.base = GPIOE_BASE,
		.ops = &stm32f401re_gpio_ops
};

struct gpio_chip gpio_h = {
		.name = "gpio.h",
		.ngpio = 16,
		.clk_id = CLK_GPIOH,
		.base = GPIOH_BASE,
		.ops = &stm32f401re_gpio_ops
};

struct gpio_chip *stm32f401re_gpio_chips[] = {
		&gpio_a,
		&gpio_b,
		&gpio_c,
		&gpio_d,
		&gpio_e,
		&gpio_h,
};

static int gpiochip_set_descs(struct gpio_chip *chip)
{
	struct gpio_desc *descs = NULL;
	char *labels = NULL;
	uint32_t label_size;
	int i;

	label_size = strlen(chip->name) + sizeof("_XY");

	descs = calloc(chip->ngpio, sizeof(struct gpio_desc));
	labels = calloc(chip->ngpio, label_size);
	//labels_table = calloc(chip->ngpio, sizeof(const char *));

	if (!descs || !labels) {
		printf("%s: failed to allocate mem for gpio pin label\n", __func__);
		free(descs);
		free(labels);
		return -1;
	}

	for (i = 0; i < chip->ngpio; i++) {
		struct gpio_desc *desc = &descs[i];
		char *label;

		label = &labels[i * label_size];
		sprintf(label, "%s_%u%u", chip->name, i/8, i%8);

		desc->chip = chip;
		desc->label = label;
		desc->flags = 0;
	}

	chip->descs = descs;

	return 0;
}

static int probe(struct gpio_chip *chip)
{
	struct clk* gpio_clk;
	int ret;

	ret = gpiochip_set_descs(chip);
	if (ret) {
		printf("%s: probe failed for gpio %s\n", __func__, chip->name);
		return -1;
	}

	// enable clk
	volatile uint32_t *pRccAhb1enr = (uint32_t*)0x40023830;

	*pRccAhb1enr |= ( 1 << 0);

	gpio_clk = clk_get(chip->clk_id);
	if (!gpio_clk) {
		printf("%s: failed to get GPIO clk\n", __func__);
		return -1;
	}

	list_add(&chip->list, &gpio_chips);

	return 0;
}

int gpiochip_probe(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(stm32f401re_gpio_chips); i++) {
		ret = probe(stm32f401re_gpio_chips[i]);
		if (ret) {
			printf("%s: probe failed for gpio chip\n", __func__);
			return -1;
		}
	}

	return 0;
}
