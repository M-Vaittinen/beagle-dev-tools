/* hwaccess.h - Direct hardware register access via /dev/mem
 *
 * On BeagleBone Black, /dev/mem allows mmap of physical registers.
 * If /dev/mem is unavailable, falls back to /dev/bbb_pinctrl (kernel module).
 *
 * All addresses must be 32-bit aligned.
 * Only Control Module (0x44E10000) and GPIO0-GPIO3 regions are allowed.
 */

#ifndef HWACCESS_H
#define HWACCESS_H

#include <stdint.h>

/* Initialise hardware access.  Returns 0 on success, -1 on error.
 * Must be called before hw_read32 / hw_write32. */
int hw_init(void);

/* Release resources acquired by hw_init(). */
void hw_close(void);

/* Returns non-zero if hardware access is available. */
int hw_available(void);

/* Returns a string describing the access method ("devmem" / "kmod" / "none").
 */
const char *hw_method(void);

/* Read a 32-bit register at the given physical address.
 * Returns 0 and sets *value on success; returns -1 on error. */
int hw_read32(uint32_t phys_addr, uint32_t *value);

/* Write a 32-bit value to the given physical address.
 * Returns 0 on success, -1 on error. */
int hw_write32(uint32_t phys_addr, uint32_t value);

#endif /* HWACCESS_H */
