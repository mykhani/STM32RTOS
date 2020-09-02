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
 * gpio.c
 *
 *  Created on: 22 Aug 2020
 *      Author: ykhan
 */

#include <gpio.h>
#include <string.h>
#include "gpio-private.h"

struct list_head gpio_lookup_list;
struct list_head gpio_chips; // list of gpio controllers

int gpio_init(void)
{
	int ret;

	INIT_LIST_HEAD(&gpio_chips);
	INIT_LIST_HEAD(&gpio_lookup_list);

	// create a list of available gpio controllers
	ret = gpiochip_probe();
	if (ret) {
		printf("%s: failed to probe the gpio controllers\n", __func__);
		return -1;
	}

	return 0;
}

static int gpio_configure_flags(struct gpio_desc *desc, enum gpio_flags flags)
{
	int ret;

	// no flags requested
	if (!(flags & GPIO_FLAGS_BIT_DIR_SET)) {
		return 0;
	}

	// Process flags
	if (flags & GPIO_FLAGS_BIT_DIR_OUT) {
		ret = gpio_direction_output(desc, flags & GPIO_FLAGS_BIT_DIR_VAL);
	} else {
		ret = gpio_direction_input(desc);
	}

	return ret;
}

struct gpio_desc *gpiochip_get_desc(struct gpio_chip *chip, uint16_t pin_hwnum)
{
	if (pin_hwnum > chip->ngpio) {
		printf("%s: GPIO pin %u out of range for gpio %s\n", __func__, pin_hwnum, chip->name);
		return NULL;
	}

	return &chip->descs[pin_hwnum];
}

struct gpio_chip *find_chip_by_name(const char *name)
{
	struct gpio_chip *chip;
	struct list_head *iter;

	for (iter = gpio_chips.next; iter; iter = iter->next) {
		chip = container_of(iter, struct gpio_chip, list);
		if (!strcmp(chip->name, name)) {
			return chip;
		}
	}

	return NULL;
}

void gpio_add_lookup_table(struct gpio_lookup_table *table)
{
	list_add(&table->list, &gpio_lookup_list);
}

static struct gpio_desc *gpio_find(const char *pin_label)
{
	struct gpio_desc *desc = NULL;
	struct gpio_chip *chip = NULL;
	struct gpio_lookup_table *lookup_table;
	struct gpio_lookup *lookup;
	struct list_head *iter;



	//list_for_each_entry(table, &gpio_lookup_list, list)

	// #define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

	// #define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

	// #define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

	for (iter = gpio_lookup_list.next; iter && !desc; iter = iter->next) {
		lookup_table = container_of(iter, struct gpio_lookup_table, list);

		for (lookup = &lookup_table->table[0]; lookup->pin_label; lookup++) {
			if (!strcmp(pin_label, lookup->pin_label)) {
				chip = find_chip_by_name(lookup->chip_label);
				if (!chip) {
				printf("%s: failed to find gpio chip for %s\n", __func__, pin_label);
					return NULL;
				}
				desc = gpiochip_get_desc(chip, lookup->pin_hwnum);
				break;
			}
		}
	}

	return desc;
}

uint8_t gpio_get_hwnum(struct gpio_desc *desc)
{
	return desc - &desc->chip->descs[0];
}

struct gpio_desc *gpio_get(const char *pin_label, enum gpio_flags flags)
{
	struct gpio_desc *desc = NULL;

	desc = gpio_find(pin_label);

	gpio_configure_flags(desc, flags);

	return desc;
}

int gpio_direction_output(struct gpio_desc *desc, int value)
{
	struct gpio_chip *chip;
	uint8_t pin_hwnum;
	int ret = -1;

	if (!desc || !desc->chip) {
		printf("%s: invalid GPIO\n", __func__);
		return -1;
	}

	chip = desc->chip;

	if (!chip->ops || !chip->ops->set || !chip->ops->direction_output  ) {
		printf("%s: missing set() or direction_output() ops\n", __func__);
		return -1;
	}

	pin_hwnum = gpio_get_hwnum(desc);

	ret = chip->ops->direction_output(chip, pin_hwnum, value);

	return ret;
}

int gpio_direction_input(struct gpio_desc *desc)
{
	struct gpio_chip *chip;

	int ret = -1;

	if (!desc || !desc->chip) {
		printf("%s: invalid GPIO\n", __func__);
		return -1;
	}

	chip = desc->chip;

	if (!chip->ops || !chip->ops->get || !chip->ops->direction_input  ) {
		printf("%s: missing get() or direction_input() ops\n", __func__);
		return -1;
	}

	ret = chip->ops->direction_input(chip, 1);

	return ret;
}

void gpio_set_value(struct gpio_desc *desc, int value)
{
	struct gpio_chip *chip;
	uint8_t pin_hwnum;

	if (!desc || !desc->chip) {
		printf("%s: invalid GPIO\n", __func__);
	}

	chip = desc->chip;

	if (!chip->ops || !chip->ops->set) {
		printf("%s: missing set() or gpio ops\n", __func__);
	}

	pin_hwnum = gpio_get_hwnum(desc);

	chip->ops->set(chip, pin_hwnum, value);
}
