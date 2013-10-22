#include <stdio.h>
#include <stdlib.h>

#define MAX 10000

void quickSort( int[], int, int);
int partition( int[], int, int);


void main(int argc, char** argv) 
{
  int size = atoi(argv[1]);
  int* a = (int*)malloc(size * sizeof(int));
	// int a[] = { 7, 12, 1, -2, 0, 15, 4, 11, 9};

	int i;
	printf("\n\nUnsorted array is:  ");
	for(i = 0; i < size; ++i)
		printf(" %d ", a[i]);

	quickSort( a, 0, size - 1);

	printf("\n\nSorted array is:  ");
	for(i = 0; i < size; ++i)
		printf(" %d ", a[i]);

}



void quickSort( int a[], int l, int r)
{
   int j;

   if( l < r ) 
   {
   	// divide and conquer
        j = partition( a, l, r);
       quickSort( a, l, j-1);
       quickSort( a, j+1, r);
   }
	
}



int partition( int a[], int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;
		
   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}






