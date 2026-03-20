/* pinmux.c - Pinmux information display */

#include <stdio.h>
#include "pinmux.h"

void pinmux_print_modes(const pin_info_t *pin)
{
    int i;
    printf("  Pinmux modes for %s (pad: %s):\n", pin->name, pin->conf_reg_name);
    printf("  %-8s  %-36s  %s\n", "MODE", "Signal Name", "Notes");
    printf("  %-8s  %-36s  %s\n",
           "--------", "------------------------------------", "-----");
    for (i = 0; i < NUM_MODES; i++) {
        const char *sig = pin->mode[i] ? pin->mode[i] : "(not available)";
        const char *note = "";
        if (i == 7)
            note = "<-- GPIO mode (MUXMODE=7)";
        printf("  MODE%-4d  %-36s  %s\n", i, sig, note);
    }
}

void pinmux_print_reg(const pin_info_t *pin)
{
    uint32_t phys_addr = CM_BASE + pin->conf_reg_offset;

    printf("  Pinmux register: %s\n", pin->conf_reg_name);
    printf("  Physical address: 0x%08X  (Control Module base 0x%08X + offset 0x%03X)\n",
           phys_addr, CM_BASE, pin->conf_reg_offset);
    printf("\n");
    printf("  Bitfield layout (32-bit register, bits 31:7 reserved):\n");
    printf("  %-6s  %-4s  %-12s  %s\n", "Bits", "Val", "Field", "Description");
    printf("  %-6s  %-4s  %-12s  %s\n",
           "------", "----", "------------", "------------------------------------");
    printf("  [6]     0    SLEWCTRL     Fast slew rate\n");
    printf("  [6]     1    SLEWCTRL     Slow slew rate\n");
    printf("  [5]     0    RXACTIVE     Input buffer disabled (output-only)\n");
    printf("  [5]     1    RXACTIVE     Input buffer enabled (required for inputs)\n");
    printf("  [4]     0    PULLTYPESEL  Pull-down selected\n");
    printf("  [4]     1    PULLTYPESEL  Pull-up selected\n");
    printf("  [3]     0    PULLUDEN     Pull-up/down enabled\n");
    printf("  [3]     1    PULLUDEN     Pull-up/down disabled\n");
    printf("  [2:0]  0-7   MUXMODE      Selects MODE0..MODE7 (MODE7 = GPIO)\n");
    printf("\n");
    printf("  Common configuration values:\n");
    printf("    0x07  GPIO output (MODE7, pull-down enabled)\n");
    printf("    0x0F  GPIO output (MODE7, pull disabled)\n");
    printf("    0x17  GPIO output (MODE7, pull-up enabled)\n");
    printf("    0x27  GPIO input  (MODE7, pull-down enabled, input buf on)\n");
    printf("    0x2F  GPIO input  (MODE7, pull disabled,     input buf on)\n");
    printf("    0x37  GPIO input  (MODE7, pull-up enabled,   input buf on)\n");
}

void pinmux_decode_value(uint32_t value)
{
    int muxmode    = (value >> 0) & 0x7;
    int pulluden   = (value >> 3) & 0x1;
    int pulltypesel= (value >> 4) & 0x1;
    int rxactive   = (value >> 5) & 0x1;
    int slewctrl   = (value >> 6) & 0x1;

    printf("  Register value: 0x%08X\n", value);
    printf("    MUXMODE     = %d  (MODE%d)\n", muxmode, muxmode);
    printf("    PULLUDEN    = %d  (%s)\n",   pulluden,
           pulluden   ? "pull disabled" : "pull enabled");
    printf("    PULLTYPESEL = %d  (%s)\n",   pulltypesel,
           pulltypesel ? "pull-up" : "pull-down");
    printf("    RXACTIVE    = %d  (%s)\n",   rxactive,
           rxactive   ? "input buf enabled" : "input buf disabled");
    printf("    SLEWCTRL    = %d  (%s)\n",   slewctrl,
           slewctrl   ? "slow slew" : "fast slew");
}
