/* gpio.c - GPIO information display */

#include "gpio.h"
#include <stdio.h>

void gpio_print_info(const struct pin_info *pin)
{
	uint32_t base;
	int sysfs_num;

	if (pin->gpio_bank < 0) {
		printf("  %s: no GPIO function available\n", pin->name);
		return;
	}

	base = gpio_base(pin->gpio_bank);
	sysfs_num = pin->gpio_bank * 32 + pin->gpio_bit;

	printf("  GPIO information for %s:\n", pin->name);
	printf("    GPIO number : GPIO%d[%d]  (gpio%d in sysfs)\n",
		pin->gpio_bank, pin->gpio_bit, sysfs_num);
	printf("    MUXMODE     : 7  (set conf_* register bits [2:0] = "
	       "0b111)\n");
	printf("\n");
	printf("  sysfs interface (/sys/class/gpio/):\n");
	printf("    echo %d > /sys/class/gpio/export\n", sysfs_num);
	printf("    echo in  > /sys/class/gpio/gpio%d/direction   # set as "
	       "input\n",
		sysfs_num);
	printf("    echo out > /sys/class/gpio/gpio%d/direction   # set as "
	       "output\n",
		sysfs_num);
	printf("    cat       /sys/class/gpio/gpio%d/value        # read\n",
		sysfs_num);
	printf("    echo 1   > /sys/class/gpio/gpio%d/value       # write "
	       "high\n",
		sysfs_num);
	printf("    echo %d > /sys/class/gpio/unexport\n", sysfs_num);
	printf("\n");
	printf("  GPIO bank %d register addresses (physical):\n",
		pin->gpio_bank);
	printf("    Base:                    0x%08X\n", base);
	printf("    GPIO_OE           (+%03X): 0x%08X  (1=input, 0=output; bit "
	       "%d)\n",
		GPIO_OE, base + GPIO_OE, pin->gpio_bit);
	printf("    GPIO_DATAIN       (+%03X): 0x%08X  (read pin state; bit "
	       "%d)\n",
		GPIO_DATAIN, base + GPIO_DATAIN, pin->gpio_bit);
	printf("    GPIO_DATAOUT      (+%03X): 0x%08X  (output value; bit "
	       "%d)\n",
		GPIO_DATAOUT, base + GPIO_DATAOUT, pin->gpio_bit);
	printf("    GPIO_CLEARDATAOUT (+%03X): 0x%08X  (write 1 to clear; bit "
	       "%d)\n",
		GPIO_CLEARDATAOUT, base + GPIO_CLEARDATAOUT, pin->gpio_bit);
	printf("    GPIO_SETDATAOUT   (+%03X): 0x%08X  (write 1 to set;   bit "
	       "%d)\n",
		GPIO_SETDATAOUT, base + GPIO_SETDATAOUT, pin->gpio_bit);
	printf("\n");
	printf("  Bit mask for this pin: 0x%08X  (1 << %d)\n",
		1U << pin->gpio_bit, pin->gpio_bit);
}

void gpio_print_state(const struct pin_info *pin, uint32_t oe_val,
	uint32_t datain_val, uint32_t dataout_val)
{
	uint32_t mask = 1U << pin->gpio_bit;
	int is_input = (oe_val & mask) ? 1 : 0;
	int input_val = (datain_val & mask) ? 1 : 0;
	int out_val = (dataout_val & mask) ? 1 : 0;

	printf("  GPIO%d[%d] (gpio%d) live state:\n", pin->gpio_bank,
		pin->gpio_bit, pin->gpio_bank * 32 + pin->gpio_bit);
	printf("    Direction   : %s\n", is_input ? "INPUT" : "OUTPUT");
	if (is_input)
		printf("    Input value : %d\n", input_val);
	else
		printf("    Output value: %d\n", out_val);
}
