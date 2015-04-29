
#include <complex.h>
#include "fftw3.h"

#include <stdlib.h>
#include <stdio.h>

#define N 10

int main(void) {

    double *channel0, *channel1, *channel2, *channel3;

	double *in;
	fftw_complex *out;
	fftw_plan p;

	in = (double*) calloc(N, sizeof(double));
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	p = fftw_plan_dft_r2c_1d(N, data, out, FFTW_ESTIMATE);

	fftw_execute(p); /* repeat as needed */

    int i;
    for (i = 0; i < N; i++) {

        printf("%f\r\n", creal(out[i]));
    }

	fftw_destroy_plan(p);
	//free(in);
	fftw_free(out);

	fflush(stdout);

	system("pause");
	return EXIT_SUCCESS;
}
