#include <stdio.h>
#include <math.h>
#include <string.h>

// Prime test by trial division
int is_prime(unsigned int n)
{
	unsigned int i;
	for (i = 2; i < n; i++)
	{
		if ((n % i) == 0)
		{
			return 0;
		}
	}
	return 1;
}

// Find all prime numbers up to CAP
int main(int argc, char** argv)
{
	unsigned int i;
	unsigned int cap = atoi(argv[1]);

	printf("Starting search for primes up to %d\n", cap);
	for (i = 2; i < cap; i++)
	{
		if (is_prime(i) == 1)
		{
			printf("%d\n", i);
		}
	}
	return 0;
}
