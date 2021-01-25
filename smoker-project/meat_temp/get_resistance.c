/*
 * Compute resistance based on bbb ADC value.
 *
 * Parameters:
 * 1. ADC value read from BBB
 * 2. Output file name
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define FIXED_RES 10000.0

int main(int argc, char * argv[])
{
	FILE *w;
	long double raw_volts;
	long double therm_resistance;
	char *chkptr;

	if (argc != 3)
		return -1;

	w = fopen(argv[2],"w");
	if (!w) {
		perror("fopen");
		return -1;
	}

	raw_volts = (double)strtol(argv[1], &chkptr, 0);
	if (0 == raw_volts)
		raw_volts = 1.0;

	therm_resistance = FIXED_RES*((4096.0/raw_volts)-1.0);

	fprintf(w,"%.15Lf\n", therm_resistance);

	return 0;
};
