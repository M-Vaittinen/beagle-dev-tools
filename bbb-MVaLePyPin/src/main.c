/* main.c - BeagleBone Black Pinmux/GPIO interactive shell
 *
 * Provides an interactive shell (built-in line editor, no external deps)
 * for querying P8/P9 pin information.  On a BeagleBone Black the tool
 * also supports live hardware register read and write.  On a PC only
 * informational commands are available.
 *
 * Usage:
 *   bbb-MVaLePyPin [PIN]...     start interactive shell (or show info for pins)
 *
 * Build:
 *   make              (native)
 *   make CROSS_COMPILE=arm-linux-gnueabihf-  (cross-compile for BBB)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "miniline.h"

#include "pindata.h"
#include "pinmux.h"
#include "gpio.h"
#include "detect.h"
#include "hwaccess.h"

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static void str_upper(char *s)
{
    while (*s) { *s = (char)toupper((unsigned char)*s); s++; }
}

/* Trim leading/trailing whitespace in-place; return pointer to first non-space */
static char *str_trim(char *s)
{
    char *end;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

/* ------------------------------------------------------------------ */
/* Print full info for a pin                                           */
/* ------------------------------------------------------------------ */

static void cmd_info(const char *pin_name)
{
    const pin_info_t *pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'. Try 'list' to see available pins.\n", pin_name);
        return;
    }

    printf("\n=== %s ===\n", pin->name);
    printf("  Ball (AM3358 ZCZ): %s\n", pin->ball);
    if (pin->note)
        printf("  NOTE: %s\n", pin->note);
    printf("\n");

    pinmux_print_modes(pin);
    printf("\n");
    pinmux_print_reg(pin);
    printf("\n");
    gpio_print_info(pin);
}

/* ------------------------------------------------------------------ */
/* Command: list                                                       */
/* ------------------------------------------------------------------ */

static void cmd_list(const char *connector)
{
    int i;
    int show_p8 = 1, show_p9 = 1;

    if (connector && *connector) {
        char uc[8];
        strncpy(uc, connector, sizeof(uc) - 1);
        uc[sizeof(uc)-1] = '\0';
        str_upper(uc);
        if (strcmp(uc, "P8") == 0)       { show_p8 = 1; show_p9 = 0; }
        else if (strcmp(uc, "P9") == 0)  { show_p8 = 0; show_p9 = 1; }
        else {
            printf("  Unknown connector '%s'. Use P8 or P9.\n", connector);
            return;
        }
    }

    if (show_p8) {
        printf("  %-8s  %-6s  %-24s  %-12s  %-5s  %s\n",
               "Pin", "Ball", "Pad (Mode 0)", "GPIO", "sysfs", "Default");
        printf("  %-8s  %-6s  %-24s  %-12s  %-5s  %s\n",
               "--------", "------", "------------------------",
               "------------", "-----", "-------");
        for (i = 0; i < p8_pin_count; i++) {
            const pin_info_t *p = &p8_pins[i];
            char gpiostr[24] = "-";
            char sysfsstr[8] = "-";
            if (p->gpio_bank >= 0) {
                snprintf(gpiostr,  sizeof(gpiostr),  "GPIO%d[%d]",
                         p->gpio_bank, p->gpio_bit);
                snprintf(sysfsstr, sizeof(sysfsstr),  "%d",
                         p->gpio_bank * 32 + p->gpio_bit);
            }
            printf("  %-8s  %-6s  %-24s  %-12s  %-5s  %s\n",
                   p->name, p->ball,
                   p->mode[0] ? p->mode[0] : "-",
                   gpiostr, sysfsstr,
                   p->mode[7] ? p->mode[7] : "-");
        }
        if (show_p9) printf("\n");
    }

    if (show_p9) {
        printf("  %-8s  %-6s  %-28s  %-12s  %-5s  %s\n",
               "Pin", "Ball", "Pad (Mode 0)", "GPIO", "sysfs", "GPIO (Mode7)");
        printf("  %-8s  %-6s  %-28s  %-12s  %-5s  %s\n",
               "--------", "------", "----------------------------",
               "------------", "-----", "------------");
        for (i = 0; i < p9_pin_count; i++) {
            const pin_info_t *p = &p9_pins[i];
            char gpiostr[24] = "-";
            char sysfsstr[8] = "-";
            if (p->gpio_bank >= 0) {
                snprintf(gpiostr,  sizeof(gpiostr),  "GPIO%d[%d]",
                         p->gpio_bank, p->gpio_bit);
                snprintf(sysfsstr, sizeof(sysfsstr),  "%d",
                         p->gpio_bank * 32 + p->gpio_bit);
            }
            printf("  %-8s  %-6s  %-28s  %-12s  %-5s  %s\n",
                   p->name, p->ball,
                   p->mode[0] ? p->mode[0] : "-",
                   gpiostr, sysfsstr,
                   p->mode[7] ? p->mode[7] : "-");
        }
    }
}

/* ------------------------------------------------------------------ */
/* Hardware read/write commands                                        */
/* ------------------------------------------------------------------ */

#define HW_NOT_AVAIL_MSG \
    "  Hardware access not available on this platform.\n" \
    "  Run on a BeagleBone Black with /dev/mem or the bbb_pinctrl kernel module.\n"

static void cmd_read_mux(const char *pin_name)
{
    uint32_t val;
    uint32_t phys;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'.\n", pin_name);
        return;
    }

    phys = CM_BASE + pin->conf_reg_offset;
    if (hw_read32(phys, &val) < 0) return;

    printf("  Pinmux register for %s (%s @ 0x%08X):\n",
           pin->name, pin->conf_reg_name, phys);
    pinmux_decode_value(val);
    if (val < NUM_MODES && pin->mode[val & 0x7])
        printf("    Current signal : %s\n", pin->mode[val & 0x7]);
}

static void cmd_write_mux(const char *pin_name, int mode)
{
    uint32_t cur, nval;
    uint32_t phys;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    if (mode < 0 || mode > 7) {
        printf("  Mode must be 0-7.\n");
        return;
    }

    pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'.\n", pin_name);
        return;
    }

    phys = CM_BASE + pin->conf_reg_offset;
    if (hw_read32(phys, &cur) < 0) return;

    /* Preserve bits 7:3 (PULLUDEN, PULLTYPESEL, RXACTIVE, SLEWCTRL) */
    nval = (cur & ~0x7U) | ((uint32_t)mode & 0x7U);
    printf("  Writing 0x%08X -> %s @ 0x%08X  (was 0x%08X)\n",
           nval, pin->conf_reg_name, phys, cur);

    if (hw_write32(phys, nval) < 0) return;
    printf("  Done. New MODE = %d (%s)\n", mode,
           pin->mode[mode] ? pin->mode[mode] : "?");
}

static void cmd_write_mux_raw(const char *pin_name, uint32_t raw_val)
{
    uint32_t phys;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'.\n", pin_name);
        return;
    }

    phys = CM_BASE + pin->conf_reg_offset;
    printf("  Writing raw 0x%08X -> %s @ 0x%08X\n",
           raw_val, pin->conf_reg_name, phys);
    if (hw_write32(phys, raw_val) < 0) return;
    printf("  Done. Decoded:\n");
    pinmux_decode_value(raw_val);
}

static void cmd_read_gpio(const char *pin_name)
{
    uint32_t oe_val, datain_val, dataout_val;
    uint32_t base, mask;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'.\n", pin_name);
        return;
    }
    if (pin->gpio_bank < 0) {
        printf("  %s has no GPIO function.\n", pin->name);
        return;
    }

    base = gpio_base(pin->gpio_bank);
    mask = 1U << pin->gpio_bit;

    if (hw_read32(base + GPIO_OE,      &oe_val)      < 0) return;
    if (hw_read32(base + GPIO_DATAIN,  &datain_val)  < 0) return;
    if (hw_read32(base + GPIO_DATAOUT, &dataout_val) < 0) return;

    gpio_print_state(pin, oe_val, datain_val, dataout_val);

    printf("    Raw OE[%d]      = %d  (full reg: 0x%08X)\n",
           pin->gpio_bit, (oe_val & mask) ? 1 : 0, oe_val);
    printf("    Raw DATAIN[%d]  = %d  (full reg: 0x%08X)\n",
           pin->gpio_bit, (datain_val & mask) ? 1 : 0, datain_val);
    printf("    Raw DATAOUT[%d] = %d  (full reg: 0x%08X)\n",
           pin->gpio_bit, (dataout_val & mask) ? 1 : 0, dataout_val);
}

static void cmd_write_gpio(const char *pin_name, int value)
{
    uint32_t base;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    pin = pin_find(pin_name);
    if (!pin) {
        printf("  Unknown pin '%s'.\n", pin_name);
        return;
    }
    if (pin->gpio_bank < 0) {
        printf("  %s has no GPIO function.\n", pin->name);
        return;
    }

    base = gpio_base(pin->gpio_bank);
    uint32_t mask = 1U << pin->gpio_bit;
    uint32_t reg  = base + (value ? GPIO_SETDATAOUT : GPIO_CLEARDATAOUT);

    printf("  Writing GPIO%d[%d] = %d  (reg 0x%08X, mask 0x%08X)\n",
           pin->gpio_bank, pin->gpio_bit, value, reg, mask);

    if (hw_write32(reg, mask) < 0) return;
    printf("  Done.\n");
}

static void cmd_write_gpio_dir(const char *pin_name, int is_input)
{
    uint32_t base, oe_val, mask;
    const pin_info_t *pin;

    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    pin = pin_find(pin_name);
    if (!pin) { printf("  Unknown pin '%s'.\n", pin_name); return; }
    if (pin->gpio_bank < 0) {
        printf("  %s has no GPIO function.\n", pin->name);
        return;
    }

    base = gpio_base(pin->gpio_bank);
    mask = 1U << pin->gpio_bit;

    if (hw_read32(base + GPIO_OE, &oe_val) < 0) return;

    if (is_input)
        oe_val |=  mask;   /* set bit: input */
    else
        oe_val &= ~mask;   /* clear bit: output */

    printf("  Setting GPIO%d[%d] direction: %s\n",
           pin->gpio_bank, pin->gpio_bit, is_input ? "INPUT" : "OUTPUT");
    if (hw_write32(base + GPIO_OE, oe_val) < 0) return;
    printf("  Done.\n");
}

/* ------------------------------------------------------------------ */
/* Register read/write (raw physical address)                         */
/* ------------------------------------------------------------------ */

static void cmd_readreg(uint32_t phys_addr)
{
    uint32_t val;
    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    if (hw_read32(phys_addr, &val) < 0) return;
    printf("  [0x%08X] = 0x%08X  (%u)\n", phys_addr, val, val);
}

static void cmd_writereg(uint32_t phys_addr, uint32_t val)
{
    if (!detect_is_bbb()) { printf(HW_NOT_AVAIL_MSG); return; }
    if (!hw_available())  { printf(HW_NOT_AVAIL_MSG); return; }

    printf("  Writing 0x%08X -> [0x%08X]\n", val, phys_addr);
    if (hw_write32(phys_addr, val) < 0) return;
    printf("  Done.\n");
}

/* ------------------------------------------------------------------ */
/* Help                                                                */
/* ------------------------------------------------------------------ */

static void print_help(void)
{
    printf(
"  BeagleBone Black Pinmux/GPIO Shell\n"
"  ----------------------------------\n"
"  All pin names are case-insensitive (e.g. P8_03, p8_03, P9_15).\n"
"\n"
"  Information commands (available on PC and BBB):\n"
"    help                     Show this help\n"
"    info  <pin>              Full pin summary (modes, registers, GPIO)\n"
"    mux   <pin>              Mux mode table (MODE0..MODE7)\n"
"    reg   <pin>              Pinmux register address and bitfield\n"
"    gpio  <pin>              GPIO bank/bit, sysfs number, register addresses\n"
"    list  [P8|P9]            List all pins (optional: filter to one connector)\n"
"\n"
"  Hardware commands (BBB only - require /dev/mem or bbb_pinctrl kmod):\n"
"    read mux   <pin>         Read current pinmux setting from hardware\n"
"    write mux  <pin> <mode>  Set MUXMODE field (0-7) leaving other bits\n"
"    write mux  <pin> raw <hex>  Write raw value to pinmux register\n"
"    read gpio  <pin>         Read current GPIO direction and value\n"
"    write gpio <pin> <0|1>   Write GPIO output value (1=high, 0=low)\n"
"    write gpio <pin> <in|out>  Set GPIO direction\n"
"    readreg   <hex_addr>     Read 32-bit register at physical address\n"
"    writereg  <hex_addr> <hex_val>  Write register at physical address\n"
"\n"
"  Examples:\n"
"    info P9_15\n"
"    mux P8_13\n"
"    gpio P9_12\n"
"    list P8\n"
"    read mux P9_15\n"
"    write mux P9_15 7\n"
"    read gpio P9_15\n"
"    write gpio P9_15 1\n"
"    write gpio P9_15 out\n"
"    readreg 0x44E10840\n"
"\n"
"    quit / exit              Exit the shell\n"
    );
}

/* ------------------------------------------------------------------ */
/* Command dispatch                                                    */
/* ------------------------------------------------------------------ */

static int dispatch(char *line)
{
    char *cmd, *arg1, *arg2, *arg3, *p;
    char buf[256];

    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    cmd = str_trim(buf);
    if (!*cmd) return 0;

    /* Split into tokens */
    arg1 = arg2 = arg3 = NULL;
    p = cmd;
    while (*p && !isspace((unsigned char)*p)) p++;
    if (*p) {
        *p++ = '\0';
        arg1 = str_trim(p);
        if (*arg1) {
            p = arg1;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) {
                *p++ = '\0';
                arg2 = str_trim(p);
                if (*arg2) {
                    p = arg2;
                    while (*p && !isspace((unsigned char)*p)) p++;
                    if (*p) {
                        *p++ = '\0';
                        arg3 = str_trim(p);
                    }
                }
            }
        }
    }

    /* Normalise command to lowercase */
    for (p = cmd; *p; p++)
        *p = (char)tolower((unsigned char)*p);

    /* --- Dispatch --- */

    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "?") == 0) {
        print_help();
        return 0;
    }

    if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0 ||
        strcmp(cmd, "q")    == 0)
        return 1;  /* signal exit */

    if (strcmp(cmd, "info") == 0) {
        if (!arg1) { printf("  Usage: info <pin>\n"); return 0; }
        cmd_info(arg1);
        return 0;
    }

    if (strcmp(cmd, "mux") == 0) {
        const pin_info_t *pin;
        if (!arg1) { printf("  Usage: mux <pin>\n"); return 0; }
        pin = pin_find(arg1);
        if (!pin) { printf("  Unknown pin '%s'.\n", arg1); return 0; }
        pinmux_print_modes(pin);
        return 0;
    }

    if (strcmp(cmd, "reg") == 0) {
        const pin_info_t *pin;
        if (!arg1) { printf("  Usage: reg <pin>\n"); return 0; }
        pin = pin_find(arg1);
        if (!pin) { printf("  Unknown pin '%s'.\n", arg1); return 0; }
        pinmux_print_reg(pin);
        return 0;
    }

    if (strcmp(cmd, "gpio") == 0) {
        const pin_info_t *pin;
        if (!arg1) { printf("  Usage: gpio <pin>\n"); return 0; }
        pin = pin_find(arg1);
        if (!pin) { printf("  Unknown pin '%s'.\n", arg1); return 0; }
        gpio_print_info(pin);
        return 0;
    }

    if (strcmp(cmd, "list") == 0) {
        cmd_list(arg1);
        return 0;
    }

    if (strcmp(cmd, "read") == 0) {
        if (!arg1 || !arg2) {
            printf("  Usage: read mux <pin>  |  read gpio <pin>\n");
            return 0;
        }
        if (strcmp(arg1, "mux")  == 0) { cmd_read_mux(arg2);  return 0; }
        if (strcmp(arg1, "gpio") == 0) { cmd_read_gpio(arg2); return 0; }
        printf("  Unknown 'read' subcommand '%s'.\n", arg1);
        return 0;
    }

    if (strcmp(cmd, "write") == 0) {
        if (!arg1 || !arg2) {
            printf("  Usage: write mux <pin> <mode>  |  write gpio <pin> <val>\n");
            return 0;
        }
        if (strcmp(arg1, "mux") == 0) {
            if (!arg3) {
                printf("  Usage: write mux <pin> <mode 0-7>  |  write mux <pin> raw <hex>\n");
                return 0;
            }
            if (strcmp(arg2, "raw") == 0) {
                /* write mux raw <pin> <hex> - reparse: arg2 is pin if arg1=mux */
                /* Actually: write mux <pin> raw <hex_val> */
                /* arg2 = pin, arg3 = "raw" or a number ... */
                /* Re-check: cmd="write" arg1="mux" arg2=<pin> arg3=<mode_or_"raw"> */
                /* If arg3 == "raw": need one more token -- not captured; handle differently */
                printf("  Usage: write mux <pin> raw <hex_val>\n");
                printf("  (The 5-token form requires typing the full command)\n");
                return 0;
            }
            {
                char *endp;
                long mode = strtol(arg3, &endp, 0);
                if (*endp) {
                    printf("  Invalid mode '%s'. Use 0-7.\n", arg3);
                    return 0;
                }
                cmd_write_mux(arg2, (int)mode);
            }
            return 0;
        }
        if (strcmp(arg1, "gpio") == 0) {
            if (!arg3) {
                printf("  Usage: write gpio <pin> <0|1>  |  write gpio <pin> <in|out>\n");
                return 0;
            }
            if (strcmp(arg3, "in")  == 0) { cmd_write_gpio_dir(arg2, 1); return 0; }
            if (strcmp(arg3, "out") == 0) { cmd_write_gpio_dir(arg2, 0); return 0; }
            {
                char *endp;
                long val = strtol(arg3, &endp, 0);
                if (*endp) {
                    printf("  Invalid value '%s'. Use 0, 1, 'in', or 'out'.\n", arg3);
                    return 0;
                }
                cmd_write_gpio(arg2, val ? 1 : 0);
            }
            return 0;
        }
        printf("  Unknown 'write' subcommand '%s'.\n", arg1);
        return 0;
    }

    if (strcmp(cmd, "readreg") == 0) {
        if (!arg1) { printf("  Usage: readreg <hex_addr>\n"); return 0; }
        {
            char *endp;
            uint32_t addr = (uint32_t)strtoul(arg1, &endp, 0);
            if (*endp) { printf("  Invalid address '%s'.\n", arg1); return 0; }
            cmd_readreg(addr);
        }
        return 0;
    }

    if (strcmp(cmd, "writereg") == 0) {
        if (!arg1 || !arg2) {
            printf("  Usage: writereg <hex_addr> <hex_val>\n"); return 0;
        }
        {
            char *endp1, *endp2;
            uint32_t addr = (uint32_t)strtoul(arg1, &endp1, 0);
            uint32_t val  = (uint32_t)strtoul(arg2, &endp2, 0);
            if (*endp1) { printf("  Invalid address '%s'.\n", arg1); return 0; }
            if (*endp2) { printf("  Invalid value '%s'.\n",   arg2); return 0; }
            cmd_writereg(addr, val);
        }
        return 0;
    }

    /* Handle "write mux <pin> raw <val>" as a 5-token command */
    {
        char buf2[256];
        char *tok[6];
        int n = 0;
        strncpy(buf2, line, sizeof(buf2) - 1);
        buf2[sizeof(buf2)-1] = '\0';
        p = str_trim(buf2);
        while (n < 5 && *p) {
            tok[n++] = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) { *p++ = '\0'; p = str_trim(p); }
        }
        tok[n] = NULL;
        if (n == 5) {
            char lc0[16], lc1[16], lc3[16];
#define LCOPY(dst,src) do { strncpy(dst,src,sizeof(dst)-1); dst[sizeof(dst)-1]='\0'; \
    for(char *_p=dst;*_p;_p++) *_p=(char)tolower(*_p); } while(0)
            LCOPY(lc0, tok[0]); LCOPY(lc1, tok[1]); LCOPY(lc3, tok[3]);

            if (strcmp(lc0,"write")==0 && strcmp(lc1,"mux")==0 &&
                strcmp(lc3,"raw")==0) {
                char *endp;
                uint32_t raw = (uint32_t)strtoul(tok[4], &endp, 0);
                if (!*endp) { cmd_write_mux_raw(tok[2], raw); return 0; }
            }
        }
    }

    printf("  Unknown command '%s'. Type 'help' for a list of commands.\n", cmd);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

static void print_banner(void)
{
    printf("bbb-MVaLePyPin - BeagleBone Black Pinmux/GPIO Shell\n");
    printf("Platform  : %s\n", detect_platform_string());
    if (detect_is_bbb()) {
        if (hw_available())
            printf("HW access : %s (read/write commands enabled)\n", hw_method());
        else
            printf("HW access : unavailable (try running as root)\n");
    } else {
        printf("HW access : n/a (not running on BeagleBone Black)\n");
    }
    printf("Type 'help' for available commands, 'quit' to exit.\n\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    char *line;
    int done = 0;

    /* Initialise hardware access on BBB */
    if (detect_is_bbb())
        hw_init();

    print_banner();

    /* If pin names are given on the command line, show info and exit */
    if (argc > 1) {
        int i;
        for (i = 1; i < argc; i++)
            cmd_info(argv[i]);
        hw_close();
        return 0;
    }

    /* Interactive readline loop */
    ml_init_history();
    while (!done) {
        line = miniline("bbbLePy> ");
        if (!line) {
            printf("\n");
            break;
        }
        if (*str_trim(line))
            ml_add_history(line);

        done = dispatch(line);
        free(line);
    }

    hw_close();
    return 0;
}
