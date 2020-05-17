#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*
12 173000
21.5 115000
94 8120
95 7900
96 6900

#Coefficients from WWW
#A=1.322381786 e-3
#B=1.232997132 e-4
#C=3.975024984 e-7
*/

#define FIXED_RES 10000.0

/* 
#define S_A 0.001322381786
#define S_B 0.0001232997132
#define S_C 0.0000003975024984
*/

//#define S_A 0.0008897101882
//#define S_B 0.0001760898686
//#define S_C 0.0000001472455495

#define COEFF_FILE "coeff_golden.txt"

//first argument is input volts
//Second argument is coefficient file

int get_coeff_100(FILE *r, long double *S_A, long double *S_B, long double *S_C)
{
	int i;
	long double S[3];

	for (i = 0; i < 3; i++)
		if (1 != fscanf(r, "%Lf\n", &S[i]))
			return -1;
	*S_A = S[0];
	*S_B = S[1];
	*S_C = S[2];

	return 0;
}

int main(int argc, char * argv[])
{
	long double therm_resistance;
	long double temp, S_A, S_B, S_C;
	long double raw_volts;
	double cterm;
	double bterm;
	double divider;
	char *chkptr;
	FILE *f;

	if (argc != 2 && argc != 3)
		return -1;

	if (argc == 3)
		f = fopen(argv[2],"r");
	else
		f = fopen(COEFF_FILE, "r");

	if (!f)
		return -1;

	if(get_coeff_100(f, &S_A, &S_B, &S_C))
		return -1;

	fclose(f);

	raw_volts = (double)strtol(argv[1], &chkptr, 0);
	if (0 == raw_volts)
		raw_volts = 1.0;

//	printf ("Raw value %u\n",(unsigned)raw_volts);

	if (*chkptr && *chkptr != '\n')
		return -1;

	//therm_resistance = FIXED_RES*(raw_volts/(4096.0/raw_volts));
	
	therm_resistance = FIXED_RES*((4096.0/raw_volts)-1.0);

//	printf("Thermistor resistance %f\n", (double)therm_resistance);
//	temp = 1.0 / (S_A + S_B*log10l(therm_resistance) + S_C*powl((log10l(therm_resistance)),3));

	cterm = S_C*powl((logl(therm_resistance)),3);
//	printf ("Cterm = %f\n", cterm);
	
	bterm = S_B*logl(therm_resistance);
//	printf("Bterm = %f\n", bterm);
	
	divider = S_A + bterm + cterm;
//	printf("divier = %f\n", divider);

//	temp = 1.0 / divider;
	temp = 100.0 / divider;

	printf("%f\n",(double)temp-273.15);
	return 0;
}
