/* detect.c - BeagleBone Black platform detection */

#include <stdio.h>
#include <string.h>
#include "detect.h"

#define MODEL_FILE "/proc/device-tree/model"
#define BBB_MODEL_SUBSTR "BeagleBone Black"

static int  g_detected = 0;
static int  g_is_bbb   = 0;
static char g_model[256] = { '\0' };

static void do_detect(void)
{
    FILE *f;

    if (g_detected)
        return;
    g_detected = 1;

    f = fopen(MODEL_FILE, "r");
    if (!f) {
        /* File not found -> not running on a BBB (or embedded Linux target) */
        snprintf(g_model, sizeof(g_model), "Unknown (no %s)", MODEL_FILE);
        g_is_bbb = 0;
        return;
    }

    if (!fgets(g_model, sizeof(g_model), f)) {
        g_model[0] = '\0';
    }
    fclose(f);

    /* Strip trailing newline or null-terminator from device-tree string */
    g_model[sizeof(g_model) - 1] = '\0';

    g_is_bbb = (strstr(g_model, BBB_MODEL_SUBSTR) != NULL) ? 1 : 0;
}

int detect_is_bbb(void)
{
    do_detect();
    return g_is_bbb;
}

const char *detect_platform_string(void)
{
    do_detect();
    return g_model[0] ? g_model : "Unknown";
}
