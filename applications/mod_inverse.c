#include <stdio.h>
#include <stdlib.h>
 
int mul_inv(int a, int b)
{
	int b0 = b, t, q;
	int x0 = 0, x1 = 1;
	if (b == 1) return 1;
	while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if (x1 < 0) x1 += b0;
	return x1;
}
 

// NOTE: input must be a prime...
int main(int argc, char** argv) 
{
	int prime = atoi(argv[1]);
	unsigned int e;
	printf("Finding the inverse of all elements in GF(%d)\n", prime);

	for (e = 0; e < prime; e++)
	{
		printf("%d^-1 mod %d = %d\n", e, prime, mul_inv(e, prime));
	}

	return 0;
}
