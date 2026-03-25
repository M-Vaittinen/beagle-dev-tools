/* gpio.h - GPIO information display functions */

#ifndef GPIO_H
#define GPIO_H

#include "pindata.h"

/* Print GPIO bank/bit, sysfs number, and register addresses */
void gpio_print_info(const struct pin_info *pin);

/* Print the direction and state of a GPIO (requires hw_read) */
void gpio_print_state(const struct pin_info *pin, uint32_t oe_val,
	uint32_t datain_val, uint32_t dataout_val);

#endif /* GPIO_H */
