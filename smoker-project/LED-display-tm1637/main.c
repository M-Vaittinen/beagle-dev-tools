#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tm1637.h"

int main(int argc, char **argv)
{
	int brightness;

	if (argc < 2) {
		printf("Usage: <commad> <brigthness> <string_to_display>\n");
		return -1;
	}
	brightness = (int) (*(argv[1]) - '0');

	if (brightness < 0 || brightness > 8) {
		printf("invalid brightness '%s'\n", argv[1]);
		return -1;
	}

	if (argc == 3 && !brightness) {
		printf("String given with brightness OFF\n");
		return -1;
	}

	tm1637Init(/*38, 40*/);

	if(argc == 2) {
  		tm1637SetBrightness(brightness);
		return 0;
	}
	if ((strlen(argv[2]) != 4) /* || ((argv[2])[2] != ' ' && (argv[2])[2] != ':')*/) {
		printf ("Invalid string format\n");
		return -1;
	}

	tm1637ShowDigits_nocolon(argv[2]);
  	tm1637SetBrightness(brightness);

	return 0;
} 
