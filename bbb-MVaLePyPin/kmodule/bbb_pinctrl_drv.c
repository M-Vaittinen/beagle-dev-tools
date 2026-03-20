// SPDX-License-Identifier: GPL-2.0
/*
 * bbb_pinctrl_drv.c - BeagleBone Black register access kernel module
 *
 * Provides a misc char device /dev/bbb_pinctrl that allows user-space to
 * read and write physical registers in the AM335x Control Module and GPIO
 * banks without requiring /dev/mem (useful when CONFIG_STRICT_DEVMEM is set).
 *
 * Only addresses within the following regions are permitted:
 *   Control Module : 0x44E10000 - 0x44E11FFF
 *   GPIO0          : 0x44E07000 - 0x44E07FFF
 *   GPIO1          : 0x4804C000 - 0x4804CFFF
 *   GPIO2          : 0x481AC000 - 0x481ACFFF
 *   GPIO3          : 0x481AE000 - 0x481AEFFF
 *
 * Usage (from user-space):
 *   #include <linux/ioctl.h>
 *   #define BBB_PINCTRL_MAGIC 'B'
 *   struct bbb_pinctrl_req { uint32_t phys_addr; uint32_t value; };
 *   #define BBB_PINCTRL_READ  _IOWR(BBB_PINCTRL_MAGIC, 0, struct bbb_pinctrl_req)
 *   #define BBB_PINCTRL_WRITE _IOW (BBB_PINCTRL_MAGIC, 1, struct bbb_pinctrl_req)
 *
 * Build:
 *   make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
 *   insmod bbb_pinctrl_drv.ko
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#error "This is 100% untested, may damage your hardware! Use at own risk!"

#define DEVICE_NAME "bbb_pinctrl"

/* ioctl interface (must match user-space header) */
#define BBB_PINCTRL_MAGIC 'B'

struct bbb_pinctrl_req {
    uint32_t phys_addr;
    uint32_t value;
};

#define BBB_PINCTRL_READ  _IOWR(BBB_PINCTRL_MAGIC, 0, struct bbb_pinctrl_req)
#define BBB_PINCTRL_WRITE _IOW (BBB_PINCTRL_MAGIC, 1, struct bbb_pinctrl_req)

/* Allowed physical address regions */
struct allowed_region {
    uint32_t base;
    uint32_t size;
    const char *name;
};

static const struct allowed_region allowed_regions[] = {
    { 0x44E10000U, 0x2000U, "Control Module" },
    { 0x44E07000U, 0x1000U, "GPIO0"          },
    { 0x4804C000U, 0x1000U, "GPIO1"          },
    { 0x481AC000U, 0x1000U, "GPIO2"          },
    { 0x481AE000U, 0x1000U, "GPIO3"          },
};
#define NUM_REGIONS ARRAY_SIZE(allowed_regions)

static int addr_allowed(uint32_t phys_addr)
{
    size_t i;
    for (i = 0; i < NUM_REGIONS; i++) {
        if (phys_addr >= allowed_regions[i].base &&
            phys_addr <  allowed_regions[i].base + allowed_regions[i].size)
            return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* ioctl handler                                                       */
/* ------------------------------------------------------------------ */

static long bbb_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct bbb_pinctrl_req req;
    void __iomem *vaddr;

    if (_IOC_TYPE(cmd) != BBB_PINCTRL_MAGIC)
        return -ENOTTY;

    if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
        return -EFAULT;

    /* Enforce 4-byte alignment */
    if (req.phys_addr & 0x3) {
        pr_warn(DEVICE_NAME ": unaligned address 0x%08X\n", req.phys_addr);
        return -EINVAL;
    }

    if (!addr_allowed(req.phys_addr)) {
        pr_warn(DEVICE_NAME ": address 0x%08X outside allowed regions\n",
                req.phys_addr);
        return -EPERM;
    }

    vaddr = ioremap(req.phys_addr, sizeof(uint32_t));
    if (!vaddr)
        return -ENOMEM;

    switch (cmd) {
    case BBB_PINCTRL_READ:
        req.value = readl(vaddr);
        iounmap(vaddr);
        if (copy_to_user((void __user *)arg, &req, sizeof(req)))
            return -EFAULT;
        break;

    case BBB_PINCTRL_WRITE:
        writel(req.value, vaddr);
        iounmap(vaddr);
        break;

    default:
        iounmap(vaddr);
        return -ENOTTY;
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* File operations                                                     */
/* ------------------------------------------------------------------ */

static const struct file_operations bbb_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = bbb_ioctl,
};

static struct miscdevice bbb_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &bbb_fops,
    .mode  = 0660,
};

/* ------------------------------------------------------------------ */
/* Module init / exit                                                  */
/* ------------------------------------------------------------------ */

static int __init bbb_pinctrl_init(void)
{
    int ret;

    ret = misc_register(&bbb_miscdev);
    if (ret) {
        pr_err(DEVICE_NAME ": misc_register failed: %d\n", ret);
        return ret;
    }

    pr_info(DEVICE_NAME ": registered /dev/%s (minor %d)\n",
            DEVICE_NAME, bbb_miscdev.minor);
    pr_info(DEVICE_NAME ": allowed regions: Control Module, GPIO0-GPIO3\n");
    return 0;
}

static void __exit bbb_pinctrl_exit(void)
{
    misc_deregister(&bbb_miscdev);
    pr_info(DEVICE_NAME ": unregistered\n");
}

module_init(bbb_pinctrl_init);
module_exit(bbb_pinctrl_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("bbb-pinctrl");
MODULE_DESCRIPTION("BeagleBone Black pinmux/GPIO register access driver");
MODULE_VERSION("1.0");
