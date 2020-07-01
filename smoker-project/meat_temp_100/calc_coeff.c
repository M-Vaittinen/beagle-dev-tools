#include <stdio.h>
#include <math.h>
#include <stdint.h>


static int read_values(FILE *r, long double *R, long double *T)
{
	int i;

	for (i = 0; i < 3; i++) {
		if (2 != fscanf(r, "%Lf:%Lf\n",&R[i], &T[i]))
			return -1;

		T[i] += 272.15;

		printf ("%d: R:T pair %Lf:%Lf\n", i, R[i], T[i]);
	}

	return 0;
}

static long double power(long double val, long double exp)
{
	int i;
	long double res = 1;

	for (i = 0; i < exp; i++)
		res *= val;

	return res;
}

int calc_coefficients(long double *R, long double *T, long double *A,
		      long double *B, long double *C)
{
	long double a, b, c, help100, help2, bterm100, bterm2, aterm100, aterm2, aterm3;
	

	a = logl(R[0]);
	b = logl(R[1]);
	c = logl(R[2]);

	printf("a=%Lf\n",a);
	printf("b=%Lf\n",b);
	printf("c=%Lf\n",c);

	//help1 = 1 / T[2] - 1 / T[0] - (1/T[1] - 1/T[0]) * (c - a) / (b - a);
	help100 = 100 / T[2] - 100 / T[0] - (100/T[1] - 100/T[0]) * (c - a) / (b - a);
	printf("help100 = %Lf\n", help100);
	help2 = power(c, 3) - power(a, 3) - (power(b, 3) - power(a, 3)) / ( b - a) * (c - a);
	printf("help2 = %Lf\n", help2);

//	*C = help1 / help2;
	*C = help100 / help2;

	printf("C100 = %Lf\n", *C);

//	bterm1 = (1/T[1] - 1/T[0])/(b - a);
	bterm100 = (100/T[1] - 100/T[0])/(b - a);
	bterm2 = *C * (power(b, 3) - power(a, 3))/(b - a);

	printf("bterm100 = %Lf\n", bterm100);
	printf("bterm2 (100) = %Lf\n", bterm2);

//	*B = (1/T[1] - 1/T[0])/(b - a) - *C * (power(b, 3) - power(a, 3))/(b - a);
	*B = bterm100 - bterm2;

	printf("B100 = %Lf\n", *B);

//	*B = *C * (power(a, 3) - power(b, 3)) / (b - a) +
//	     (1/T[1] - 1/T[0]) / (b - a);

	aterm100 = 100/T[0];
//	aterm1 = 1/T[0];
	aterm2 = *B*a;
	aterm3 = *C*power(a,3);

	printf("aterm100 = %Lf\n", aterm100);


//	*A = 1/T[0] - *B*a - *C*a^3;
	*A = aterm100 - aterm2 - aterm3;

	printf("aterm1 (100) = %Lf\n", aterm100);
	printf("aterm2 (100) = %Lf\n", aterm2);
	printf("aterm3 (100) = %Lf\n", aterm3);

	printf("A100 = %Lf\n", *A);
/*
	*A /= 100;
	*B /= 100;
	*C /= 100;
*/
	return 0;
}

int write_coeff(char *fn, long double A, long double B, long double C)
{
	FILE *w;

	w = fopen(fn, "w");
	if (!w) {
		perror("fopen\n");
		return -1;
	}

	fprintf(w, "%.15Lf\n%.15Lf\n%.15Lf\n", A, B, C);
	fclose(w);

	return 0;
}

int main(int argc, char *argv[])
{
	FILE *r;
	long double R[3], T[3], A, B, C;

	if (argc != 3) {
		printf("Need resistance - temperature value file and coeff output file\n");
		return -1;
	}
	r = fopen(argv[1], "r");
	if (!r) {
		perror("Failed to open calib file\n");
		return -1;
	}
	if (read_values(r, R, T)) {
		printf("Failed to get values - bad file format?\n");
	}
	fclose(r);

	if (calc_coefficients(R, T, &A, &B, &C)) {
		printf ("Failed to calc coefficients\n");
		return -1;
	}
	printf("A=%.10Lf, B=%.10Lf, C=%.10Lf\n", A, B, C);
	write_coeff(argv[2], A, B, C);

	return 0;
}

