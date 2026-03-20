/* pinmux.h - Pinmux information display functions */

#ifndef PINMUX_H
#define PINMUX_H

#include "pindata.h"

/* Print a table of all 8 mux modes for the pin */
void pinmux_print_modes(const pin_info_t *pin);

/* Print the pinmux control register address and bitfield description */
void pinmux_print_reg(const pin_info_t *pin);

/* Decode and print a raw conf_* register value */
void pinmux_decode_value(uint32_t value);

#endif /* PINMUX_H */
