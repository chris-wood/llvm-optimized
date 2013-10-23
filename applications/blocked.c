/* File:     Blocked matrix multiplication
 *
 * Purpose:  Compare the run time of the standard matrix multiplication
 *           algorithm with blocked matrix multiplication.
 *
 * Compile:  gcc -g -Wall -I. [-DDEBUG] -o blocked blocked.c
 * Run:      ./blocked <order of matrices> <order of blocks> [i]
 *              <-> required argument, [-] optional argument
 *
 * Input:    If the "i" flag is given on the command line,
 *           the two matrices must be input.
 * Output:   Elapsed time for the two multiplication methods.
 *           If the DEBUG flag is set, the product matrix as
 *           computed by each method is also output.
 *
 * Notes:
 * 1.  The file timer.h should be in the directory containing
 *     the source file.
 * 2.  The order of the blocks (b) must evenly divide the order 
 *     of the matrices (n)
 * 3.  Set the DEBUG flag to see the product matrices
 * 4.  If the command line flag "i" isn't present, matrices are 
 *     generated using a random number generator.
 * 5.  There are a number of optimizations that can be made to 
 *     the source code that will improve the performance of both
 *     algorithms.
 * 6.  Note that unless the DEBUG flag is set the product matrices will, 
 *     in general, be different using the two algorithms, since the two 
 *     algorithms use identical storage for A and B, but they assume 
 *     the storage has different structures.
 * 7.  If the matrix order is n and the block size is b, define
 *     
 *        n_bar = n/b = number of block rows = number of block cols
 *        b_sqr = b*b = size of the blocks
 *
 *     If we're in block row i_bar, block column j_bar, then the 
 *     address of the first entry in matrix X will be 
 *
 *        X + (i_bar*n_bar + j_bar)*b_sqr
 *
 *     The remainder of the b x b block is stored in the next b^2
 *     memory locations.
 * 8.  This has received *very* little testing.  Students who find
 *     and correct bugs will receive many gold stars.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset

#define MAX 100

// Global Variables 
double *A, *B, *C;
int n, i, j;

// Prototypes
void mmult(void);
void printmat(double C[], int n);

int main(int argc, char* argv[]) 
{
   n = atoi(argv[1]);
   A = malloc(n*n*sizeof(double));
   B = malloc(n*n*sizeof(double));
   C = malloc(n*n*sizeof(double));
   if (A == NULL || B == NULL || C == NULL) {
      fprintf(stderr, "Can't allocate storage!\n");
      exit(-1);
   }

   // init the matrices
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
      {
         A[i*n + j] = rand() % MAX;
         B[i*n + j] = rand() % MAX;
      }
   }

//   printmat(A, n);
//   printmat(B, n);
   
   printf("Multiying...\n");
   mmult();
   printf("Result...\n");
//   printmat(C, n);

   return 0;
}  

void mmult(void) 
{
   int i, j, k;
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
         C[i*n + j] = 0.0;
         for (k = 0; k < n; k++)
            C[i*n + j] += A[i*n + k] * B[k*n + j];
      }
   }
} 

void printmat(double C[], int n) 
{
   int i, j;
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++)
         printf("%.2e ", C[i*n+j]);
      printf("\n");
   }
}  
