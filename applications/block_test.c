#include <stdio.h>
#include <math.h>

#define CAP 10000000
#define MOD 1000

// Find all prime numbers up to CAP
int main(int argc, char* argv)
{
	unsigned int i;
	for (i = 0; i < CAP; i++)
	{
		if ((i % MOD) == 0)
		{
			printf("%d\n", i);
		}
	}
	return 0;
}
