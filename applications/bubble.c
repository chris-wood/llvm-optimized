#include <stdio.h>
#include <stdlib.h>

#define MAX 10000

void bubbleSort(int* array, int size)
 {
    int swapped;
    int i;
    for (i = 1; i < size; i++)
    {
        swapped = 0;    //this flag is to check if the array is already sorted
        int j;
        for(j = 0; j < size - i; j++)
        {
            if(array[j] > array[j+1])
            {
                int temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
                swapped = 1;
            }
        }
        if(!swapped){
            break; //if it is sorted then stop
        }
    }
 }

int main(int argc, char** argv)
{
    int size = atoi(argv[1]);
    int* array = (int*)malloc(size * sizeof(int));
    unsigned int i;
    for (i = 0; i < size; i++)
    {
        array[i] = rand() % MAX;
    }
    //printf("Pre-sort: ");
    //for (i = 0; i < size; i++) printf("%d ", array[i]);
    //printf("\n");
    bubbleSort(array, size);
    //printf("Post-sort: ");
    //for (i = 0; i < size; i++) printf("%d ", array[i]);
    //printf("\n");
	return 0;
}
