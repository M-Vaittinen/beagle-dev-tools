/* detect.h - BeagleBone Black platform detection */

#ifndef DETECT_H
#define DETECT_H

/* Returns 1 if running on a BeagleBone Black, 0 otherwise.
 * Detection is done once and cached. */
int detect_is_bbb(void);

/* Returns a human-readable platform string. */
const char *detect_platform_string(void);

#endif /* DETECT_H */
