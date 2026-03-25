/* hwaccess.c - Hardware register access via /dev/mem or kernel module */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "hwaccess.h"
#include "pindata.h"

/* ------------------------------------------------------------------ */
/* Kernel module fallback (bbb_pinctrl_drv) ioctl interface           */
/* ------------------------------------------------------------------ */
#define BBB_PINCTRL_DEV "/dev/bbb_pinctrl"
#define BBB_PINCTRL_MAGIC 'B'
#define BBB_PINCTRL_READ _IOWR(BBB_PINCTRL_MAGIC, 0, struct bbb_pinctrl_req)
#define BBB_PINCTRL_WRITE _IOW(BBB_PINCTRL_MAGIC, 1, struct bbb_pinctrl_req)

struct bbb_pinctrl_req {
	uint32_t phys_addr;
	uint32_t value;
};

/* ------------------------------------------------------------------ */
/* Allowed address ranges (whitelist)                                 */
/* ------------------------------------------------------------------ */
#define PAGE_SIZE 0x1000U
#define PAGE_MASK (~(PAGE_SIZE - 1))

struct mem_region {
	uint32_t base;
	uint32_t size;
	const char *name;
};

static const struct mem_region allowed_regions[] = {
	{CM_BASE, 0x2000, "Control Module"},
	{GPIO0_BASE, 0x1000, "GPIO0"},
	{GPIO1_BASE, 0x1000, "GPIO1"},
	{GPIO2_BASE, 0x1000, "GPIO2"},
	{GPIO3_BASE, 0x1000, "GPIO3"},
};
#define NUM_REGIONS (int)(sizeof(allowed_regions) / sizeof(allowed_regions[0]))

static int addr_allowed(uint32_t phys_addr)
{
	int i;
	for (i = 0; i < NUM_REGIONS; i++) {
		if (phys_addr >= allowed_regions[i].base &&
			phys_addr < allowed_regions[i].base +
					    allowed_regions[i].size)
			return 1;
	}
	return 0;
}

/* ------------------------------------------------------------------ */
/* Mapped page cache                                                  */
/* ------------------------------------------------------------------ */
#define MAX_MAPPED_PAGES 8

struct mapped_page {
	uint32_t page_base;
	void *map;
};

static int g_devmem_fd = -1;
static int g_kmod_fd = -1;
static int g_available = 0;
static const char *g_method = "none";
static struct mapped_page g_pages[MAX_MAPPED_PAGES];
static int g_page_count = 0;

/* ------------------------------------------------------------------ */
/* init / close                                                        */
/* ------------------------------------------------------------------ */
int hw_init(void)
{
	memset(g_pages, 0, sizeof(g_pages));
	g_page_count = 0;

	/* Try /dev/mem first */
	g_devmem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (g_devmem_fd >= 0) {
		g_available = 1;
		g_method = "devmem";
		return 0;
	}

	/* Fall back to kernel module */
	g_kmod_fd = open(BBB_PINCTRL_DEV, O_RDWR);
	if (g_kmod_fd >= 0) {
		g_available = 1;
		g_method = "kmod";
		return 0;
	}

	fprintf(stderr, "hwaccess: cannot open /dev/mem (%s) or %s (%s)\n",
		strerror(errno), BBB_PINCTRL_DEV, strerror(errno));
	g_available = 0;
	g_method = "none";
	return -1;
}

void hw_close(void)
{
	int i;
	for (i = 0; i < g_page_count; i++) {
		if (g_pages[i].map)
			munmap(g_pages[i].map, PAGE_SIZE);
	}
	g_page_count = 0;
	if (g_devmem_fd >= 0) {
		close(g_devmem_fd);
		g_devmem_fd = -1;
	}
	if (g_kmod_fd >= 0) {
		close(g_kmod_fd);
		g_kmod_fd = -1;
	}
	g_available = 0;
	g_method = "none";
}

int hw_available(void)
{
	return g_available;
}

const char *hw_method(void)
{
	return g_method;
}

/* ------------------------------------------------------------------ */
/* /dev/mem implementation                                             */
/* ------------------------------------------------------------------ */
static void *devmem_map_page(uint32_t page_base)
{
	int i;
	void *map;

	/* Check cache */
	for (i = 0; i < g_page_count; i++) {
		if (g_pages[i].page_base == page_base)
			return g_pages[i].map;
	}

	/* Map new page */
	map = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		g_devmem_fd, (off_t)page_base);
	if (map == MAP_FAILED)
		return NULL;

	if (g_page_count < MAX_MAPPED_PAGES) {
		g_pages[g_page_count].page_base = page_base;
		g_pages[g_page_count].map = map;
		g_page_count++;
	}
	return map;
}

/* ------------------------------------------------------------------ */
/* Public read/write                                                   */
/* ------------------------------------------------------------------ */
int hw_read32(uint32_t phys_addr, uint32_t *value)
{
	if (!g_available) {
		fprintf(stderr, "hwaccess: not initialised\n");
		return -1;
	}
	if (!addr_allowed(phys_addr)) {
		fprintf(stderr,
			"hwaccess: address 0x%08X not in allowed range\n",
			phys_addr);
		return -1;
	}

	if (g_devmem_fd >= 0) {
		uint32_t page_base = phys_addr & PAGE_MASK;
		uint32_t offset = phys_addr & ~PAGE_MASK;
		void *page = devmem_map_page(page_base);
		if (!page) {
			fprintf(stderr,
				"hwaccess: mmap failed for 0x%08X: %s\n",
				phys_addr, strerror(errno));
			return -1;
		}
		*value = *(volatile uint32_t *)((char *)page + offset);
		return 0;
	}

	if (g_kmod_fd >= 0) {
		struct bbb_pinctrl_req req = {phys_addr, 0};
		if (ioctl(g_kmod_fd, BBB_PINCTRL_READ, &req) < 0) {
			fprintf(stderr, "hwaccess: ioctl READ failed: %s\n",
				strerror(errno));
			return -1;
		}
		*value = req.value;
		return 0;
	}

	return -1;
}

int hw_write32(uint32_t phys_addr, uint32_t value)
{
	if (!g_available) {
		fprintf(stderr, "hwaccess: not initialised\n");
		return -1;
	}
	if (!addr_allowed(phys_addr)) {
		fprintf(stderr,
			"hwaccess: address 0x%08X not in allowed range\n",
			phys_addr);
		return -1;
	}

	if (g_devmem_fd >= 0) {
		uint32_t page_base = phys_addr & PAGE_MASK;
		uint32_t offset = phys_addr & ~PAGE_MASK;
		void *page = devmem_map_page(page_base);
		if (!page) {
			fprintf(stderr,
				"hwaccess: mmap failed for 0x%08X: %s\n",
				phys_addr, strerror(errno));
			return -1;
		}
		*(volatile uint32_t *)((char *)page + offset) = value;
		return 0;
	}

	if (g_kmod_fd >= 0) {
		struct bbb_pinctrl_req req = {phys_addr, value};
		if (ioctl(g_kmod_fd, BBB_PINCTRL_WRITE, &req) < 0) {
			fprintf(stderr, "hwaccess: ioctl WRITE failed: %s\n",
				strerror(errno));
			return -1;
		}
		return 0;
	}

	return -1;
}
