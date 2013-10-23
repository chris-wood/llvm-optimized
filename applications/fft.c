#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
 
double PI;
typedef double complex cplx;
 
void _fft(cplx buf[], cplx out[], int n, int step)
{
	int i;
	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (i = 0; i < n; i += 2 * step) {
			cplx t = cexp(-I * PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}
 
void fft(cplx buf[], int n)
{
	int i;
	cplx out[n];
	for (i = 0; i < n; i++) out[i] = buf[i];
 
	_fft(buf, out, n, 1);
}
void show(const char * s, cplx* buf, int size) {
	int i;
	printf("%s", s);
	for (i = 0; i < size; i++)
		if (!cimag(buf[i]))
			printf("%g ", creal(buf[i]));
		else
			printf("(%g, %g) ", creal(buf[i]), cimag(buf[i]));
}
 
int main(int argc, char** argv)
{
	int size = atoi(argv[1]);
	int i;
	PI = atan2(1, 1) * 4;
	
	cplx* buf = (cplx*)malloc(sizeof(cplx) * size);
 
	show("Data: ", buf, size);
	fft(buf, size);
	show("\nFFT : ", buf, size);
 
	return 0;
}
