/* pindata.h - BeagleBone Black P8/P9 connector pin data structures
 *
 * Data sourced from:
 *   - BeagleBone Black System Reference Manual (BBB SRM), Tables 7.1, 7.2, 6.7, 6.8
 *   - TI AM335x Technical Reference Manual (SPRUH73Q), Section 9 (Control Module)
 *
 * Hardware memory map:
 *   Control Module (pinmux):  0x44E10000  (conf_* regs start at +0x800)
 *   GPIO0:                    0x44E07000
 *   GPIO1:                    0x4804C000
 *   GPIO2:                    0x481AC000
 *   GPIO3:                    0x481AE000
 *
 * conf_* register bitfields (32-bit):
 *   bits 2:0  MUXMODE     Mux select: 0=Mode0 (primary) ... 7=GPIO
 *   bit  3    PULLUDEN    Pull enable: 0=pull enabled, 1=pull disabled
 *   bit  4    PULLTYPESEL Pull type:   0=pulldown,    1=pullup
 *   bit  5    RXACTIVE    Input buf:   0=disabled,    1=enabled (required for inputs)
 *   bit  6    SLEWCTRL    Slew rate:   0=fast,        1=slow
 *
 * GPIO register offsets (same for GPIO0-GPIO3):
 *   0x134  GPIO_OE           Output enable (1=input, 0=output; reset=all 1)
 *   0x138  GPIO_DATAIN       Sampled input state (read-only)
 *   0x13C  GPIO_DATAOUT      Output data register
 *   0x190  GPIO_CLEARDATAOUT Atomic: write 1 to clear output bit
 *   0x194  GPIO_SETDATAOUT   Atomic: write 1 to set output bit
 *
 * sysfs GPIO number: bank * 32 + bit  (e.g. GPIO1_6 = 1*32+6 = 38)
 */

#ifndef PINDATA_H
#define PINDATA_H

#include <stdint.h>

#define CM_BASE     0x44E10000U  /* Control Module base */
#define GPIO0_BASE  0x44E07000U  /* GPIO bank 0 */
#define GPIO1_BASE  0x4804C000U  /* GPIO bank 1 */
#define GPIO2_BASE  0x481AC000U  /* GPIO bank 2 */
#define GPIO3_BASE  0x481AE000U  /* GPIO bank 3 */

/* GPIO register offsets (same for all banks) */
#define GPIO_OE           0x134U
#define GPIO_DATAIN       0x138U
#define GPIO_DATAOUT      0x13CU
#define GPIO_CLEARDATAOUT 0x190U
#define GPIO_SETDATAOUT   0x194U

/* conf_* register bitfield values */
#define CONF_MUXMODE(x)   ((x) & 0x7U)
#define CONF_PULLUDEN     (1U << 3)  /* 1 = pull disabled */
#define CONF_PULLTYPESEL  (1U << 4)  /* 1 = pullup */
#define CONF_RXACTIVE     (1U << 5)  /* 1 = input buffer enabled */
#define CONF_SLEWCTRL     (1U << 6)  /* 1 = slow slew */

/* Common conf values */
#define CONF_GPIO_OUTPUT  (CONF_MUXMODE(7) | CONF_PULLUDEN)
#define CONF_GPIO_INPUT   (CONF_MUXMODE(7) | CONF_PULLUDEN | CONF_RXACTIVE)

#define NUM_MODES 8  /* MODE0 .. MODE7 (MODE7 always GPIO) */

typedef struct {
    const char  *name;            /* Header pin name, e.g. "P8_03" */
    const char  *ball;            /* AM3358 ZCZ ball ID, e.g. "R9" */
    const char  *conf_reg_name;   /* Control Module register name, e.g. "conf_gpmc_ad6" */
    uint32_t     conf_reg_offset; /* Offset from CM_BASE, e.g. 0x818 */
    int          gpio_bank;       /* GPIO bank (0-3), or -1 if no GPIO */
    int          gpio_bit;        /* GPIO bit within bank (0-31) */
    const char  *mode[NUM_MODES]; /* Signal names for MODE0..MODE7 */
    const char  *note;            /* Optional note (may be NULL) */
} pin_info_t;

/* Connector P8: 44 pins (P8_01/02 = GND, P8_03..P8_46 usable) */
extern const pin_info_t p8_pins[];
extern const int        p8_pin_count;

/* Connector P9: entries for all configurable P9 pins */
extern const pin_info_t p9_pins[];
extern const int        p9_pin_count;

/* Look up pin by name (e.g. "P8_03" or "p8_03").  Returns NULL if not found. */
const pin_info_t *pin_find(const char *name);

/* Return GPIO base address for the given bank (0-3), or 0 if invalid. */
uint32_t gpio_base(int bank);

#endif /* PINDATA_H */
