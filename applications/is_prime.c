#include <stdio.h>
#include <math.h>

#define CAP 250000

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
	for (i = 2; i < CAP; i++)
	{
		if (is_prime(i) == 1)
		{
			printf("%d\n", i);
		}
	}
	return 0;
}
