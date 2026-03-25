# bbb-MVaLePyPin

## 99% Untested! Try 100% at your own risk!

Interactive shell for querying and controlling pinmux and GPIO settings on
the BeagleBone Black (P8 / P9 expansion connectors).


## Features

- Query **mux modes** (MODE0–MODE7) for any P8 or P9 pin
- Show **pinmux register** address and bitfield meaning
- Show **GPIO** bank/bit, sysfs number, and register addresses
- On **BeagleBone Black**: live register **read and write** via `/dev/mem`
- Fallback to `/dev/bbb_pinctrl` kernel module if `/dev/mem` is restricted
- Works on **PC** for all informational commands (no hardware required)
- `readline`-based shell with command history

## Quick Start

```
bbbLePy> ÄoÄ               # important guidance for user
bbbLePy> info P9_15        # full pin summary
bbbLePy> mux  P8_13        # show mux mode table
bbbLePy> reg  P9_15        # show pinmux register details
bbbLePy> gpio P9_12        # show GPIO info and sysfs commands
bbbLePy> list P9           # list all P9 pins
bbbLePy> read  mux  P9_15  # (BBB only) read current mux setting
bbbLePy> write mux  P9_15 7  # (BBB only) set to GPIO mode
bbbLePy> write gpio P9_15 out        # (BBB only) set as output
bbbLePy> write gpio P9_15 1        # (BBB only) drive high
bbbLePy> read  gpio P9_15          # (BBB only) read direction/value
bbbLePy> readreg 0x44E10840        # (BBB only) read physical register
```

## Building

### Prerequisites

```bash
# No external library dependencies — only a C compiler and POSIX libc are needed.
# Ubuntu / Debian example:
sudo apt-get install build-essential
```

### Native build (on BBB or PC)

```bash
cd bbb-MVaLePyPin
make
```

### Cross-compile for BBB (from PC)

```bash
sudo apt-get install gcc-arm-linux-gnueabihf libreadline-dev:armhf
make CROSS_COMPILE=arm-linux-gnueabihf-
# Then scp bbb-MVaLePyPin debian@192.168.7.2:~
```

## Usage

### Interactive mode

```bash
./bbb-MVaLePyPin
```

### One-shot mode (show info and exit)

```bash
./bbb-MVaLePyPin P9_15 P8_13
```

### Command reference

| Command | Description |
|---------|-------------|
| `help` | Show all commands |
| `info <pin>` | Full pin summary |
| `mux <pin>` | Mux mode table (MODE0–MODE7) |
| `reg <pin>` | Pinmux register address + bitfield |
| `gpio <pin>` | GPIO bank/bit, sysfs number, register addresses |
| `list [P8\|P9]` | List all pins for a connector |
| `read mux <pin>` | *(BBB)* Read current pinmux setting |
| `write mux <pin> <mode>` | *(BBB)* Set MUXMODE (0–7) |
| `write mux <pin> raw <hex>` | *(BBB)* Write raw value to pinmux register |
| `read gpio <pin>` | *(BBB)* Read GPIO direction and value |
| `write gpio <pin> <0\|1>` | *(BBB)* Drive GPIO output |
| `write gpio <pin> <in\|out>` | *(BBB)* Set GPIO direction |
| `readreg <hex_addr>` | *(BBB)* Read 32-bit physical register |
| `writereg <hex_addr> <hex_val>` | *(BBB)* Write 32-bit physical register |
| `quit` / `exit` | Exit the shell |

## Hardware Memory Map

| Region | Base Address |
|--------|-------------|
| Control Module (pinmux) | `0x44E10000` |
| GPIO0 | `0x44E07000` |
| GPIO1 | `0x4804C000` |
| GPIO2 | `0x481AC000` |
| GPIO3 | `0x481AE000` |

### Pinmux register bitfields (`conf_*`, 32-bit)

```
Bits 31:7  Reserved
Bit  6     SLEWCTRL    0=fast slew, 1=slow slew
Bit  5     RXACTIVE    0=output only, 1=input buffer enabled
Bit  4     PULLTYPESEL 0=pull-down, 1=pull-up
Bit  3     PULLUDEN    0=pull enabled, 1=pull DISABLED
Bits 2:0   MUXMODE     Mux select: 0=Mode0 (primary) … 7=GPIO
```

Common values (see bitfield table above):
| Value | MUXMODE | PULLUDEN | PULLTYPESEL | RXACTIVE | Meaning |
|-------|---------|----------|-------------|----------|---------|
| `0x07` | 7 | 0 (pull ON) | 0 (down) | 0 | GPIO output, pull-down |
| `0x0F` | 7 | 1 (pull OFF) | — | 0 | GPIO output, no pull |
| `0x17` | 7 | 0 (pull ON) | 1 (up) | 0 | GPIO output, pull-up |
| `0x27` | 7 | 0 (pull ON) | 0 (down) | 1 | GPIO input, pull-down |
| `0x2F` | 7 | 1 (pull OFF) | — | 1 | GPIO input, no pull |
| `0x37` | 7 | 0 (pull ON) | 1 (up) | 1 | GPIO input, pull-up |

### GPIO register offsets (same for GPIO0–GPIO3)

| Offset | Register | Description |
|--------|----------|-------------|
| `0x134` | `GPIO_OE` | Output enable (1=input, 0=output) |
| `0x138` | `GPIO_DATAIN` | Sampled input pin state (read-only) |
| `0x13C` | `GPIO_DATAOUT` | Output data register |
| `0x190` | `GPIO_CLEARDATAOUT` | Write 1 to clear output bit (atomic) |
| `0x194` | `GPIO_SETDATAOUT` | Write 1 to set output bit (atomic) |

sysfs GPIO number: `bank × 32 + bit`  (e.g. GPIO1_16 → gpio48)

## Kernel Module (optional)

If `/dev/mem` is not available (kernel built with `CONFIG_STRICT_DEVMEM`),
the tool falls back to `/dev/bbb_pinctrl` provided by the kernel module.

```bash
cd kmodule
make
sudo insmod bbb_pinctrl_drv.ko
sudo chmod a+rw /dev/bbb_pinctrl   # or use udev rule
```

The module exposes an ioctl interface accepting read/write of 32-bit registers
only within the allowed address ranges (Control Module + GPIO0–GPIO3).

### Unload

```bash
sudo rmmod bbb_pinctrl_drv
```

## Data Sources

- BeagleBone Black System Reference Manual, Tables 7.1 (P8), 7.2 (P9), 6.7–6.8 (PRU)
- TI AM335x Technical Reference Manual SPRUH73Q:
  - Section 2 (Memory Map)
  - Section 9 (Control Module — Table 9-10, pad register bitfields)
  - Section 25 (GPIO register offsets)

## Pin Coverage

- **P8**: 44 pins (P8_03 – P8_46; P8_01/02 are GND)
- **P9**: 23 configurable pins (P9_11–P9_31, P9_41, P9_42)
  - P9_01/02 = GND; P9_03/04 = 3.3V; P9_05/06 = VDD_5V; P9_07/08 = SYS_5V
  - P9_09 = PWR_BUT; P9_10 = SYS_RESETn
  - P9_32 = VADC; P9_33–P9_40 = ADC-only (no pinmux); P9_34 = AGND
  - P9_43–P9_46 = GND
