#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cmdline.h"

float opt_alpha = 5.0;
int opt_numerator = 2;
int opt_denominator = 3;
int opt_zero_crossings = 13;

#define FAKE_INF 50
double I0(double x) {
	double f = 0;

	double kfac = 1;
	for(int k = 0; k < FAKE_INF; k++) {
		if(k > 0) {
			if(k == 1) kfac = 1;
			else kfac *= k;
		}

		double frac = pow(x/2, k) / kfac;
		f += frac * frac;
	}

	return f;
}

double kaiser(double M, double B, double n) {
	double I0B = I0(B);
	if(n > M / 2) return 0;
	return I0((double)B * sqrt(1 - pow(n / ((double)M / 2.0), 2.0))) / I0B;
}

double sinc(double x) {
	if(x == 0) return 1;
	return sin(x) / x;
}

int main(int argc, char **argv) {
	int optind = cmdline_parse_args(argc, argv, (struct cmdline_option[]){
		{
			'n', "numerator",
			"Numerator",
			"N",
			TYPE_REQUIRED,
			TYPE_INT, &opt_numerator
		},
		{
			'd', "denominator",
			"Denominator",
			"N",
			TYPE_REQUIRED,
			TYPE_INT, &opt_denominator
		},
		{
			'z', "zero-crossings",
			"Number of zero crossings",
			"N",
			TYPE_REQUIRED,
			TYPE_INT, &opt_zero_crossings
		},
		{
			'a', "alpha",
			"The alpha parameter for the kaiser window",
			"N",
			TYPE_REQUIRED,
			TYPE_FLOAT, &opt_alpha
		},
		CMDLINE_ARG_TERMINATOR
	}, 0, 0, 0);

	if(optind < 0) exit(-optind);

	int total_sinc_samples = opt_zero_crossings * opt_denominator;
	for(int n = 0; n <= total_sinc_samples; n++) {
		double k = kaiser(total_sinc_samples * 2.0, opt_alpha * M_PI, (double)n);
		double s = sinc((double)n * M_PI / opt_denominator);
		double ks = k * s;
		int ksi = ks * 32767;
		if(ksi > 32767) ksi = 32767;
		if(ksi < -32767) ksi = -32767;
		printf("% 6d, /* % 2.08f = %.08f * % 2.08f */\n", ksi, ks, k, s);
	}

	return 0;
}
