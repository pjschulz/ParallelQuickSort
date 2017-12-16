#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "omp.h"

void quickSortSerial(int* arr, int left, int right);
void quickSortParallel(int* arr, int arrayLength, int numThreads);
void internalQuickSort(int* array, int left, int right, int overallPivot);
int randFrom(int min, int max);


int main()
{
    int minValue = 1;
    int maxValue = 10000;
    int arrayLength = 40000000;
    int numThreads = 10;
    int msec = 0;
    double startTime1, endTime1, diff1, startTime2, endTime2, diff2;

    int* dataArray;
    int* dataArray2;
    dataArray = (int*) malloc(arrayLength* sizeof(int));
    dataArray2 = (int*) malloc(arrayLength* sizeof(int));


    srand(5); //Seeding the random number generator
    //printf("Array Before Sorting:\r\n");

    for (int i = 0; i < arrayLength; i++)
    {
        dataArray[i] = randFrom(minValue, maxValue);
        dataArray2[i] = dataArray[i];
        //printf("%d\r\n", dataArray[i]);
    }



    printf("Sorting with serial Quick Sort...\r\n");
    startTime1 = clock();
    quickSortSerial(dataArray, 0, arrayLength - 1);
    endTime1 = clock();
    diff1 = (endTime1 - startTime1)/CLOCKS_PER_SEC;

    printf("Sorting with Parallel Quick Sort...\r\n");
    startTime2 = omp_get_wtime();
    quickSortParallel(dataArray2, arrayLength, numThreads);
    endTime2 = omp_get_wtime();
    diff2 = (endTime2 - startTime2);

  /*
    for (int i = 0; i < arrayLength; i++)
    {
        printf("%d\r\n", dataArray[i]);
    }
*/
/*
  for (int i = 0; i < arrayLength; i++)
  {
      printf("%d\r\n", dataArray2[i]);
  }
  */



    printf("Serial Sort finished in : %f seconds\r\n", diff1);
    printf("Parallel Sort finished in : %f seconds\r\n",diff2);

    free(dataArray);
    free(dataArray2);

    return 0;
}

// Serial Quick Sort Program
void quickSortSerial(int* arr, int left, int right)
{
    int i = left, j = right;
    int tmp;
    int pivot = arr[right];

    /* PARTITION PART */
    while (i <= j)
    {
        while (arr[i] < pivot)
        {
            i++;
        }
        while (arr[j] > pivot)
        {
            j--;
        }
        if (i <= j) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    }

    /* RECURSION PART */
    if (left < j){ quickSortSerial(arr, left, j);  }
    if (i< right){ quickSortSerial(arr, i, right); }
}

void quickSortParallel(int* array, int lenArray, int numThreads){

    //pivot to broadcast
    int overallPivot = array[0];

#pragma omp parallel num_threads(numThreads)
    {
#pragma omp single nowait
        {
            internalQuickSort(array, 0, lenArray -1, overallPivot);
        }
    }

}
void internalQuickSort(int* array, int left, int right, int overallPivot)
{

    int i = left, j = right;
    int tmp;
    int pivot = array[right];


    {
        /* PARTITION PART */
        while (i <= j)
        {
            while (array[i] < pivot)
            {
                i++;
            }

            while (array[j] > pivot)
            {
                j--;
            }
            if (i <= j) {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
                i++;
                j--;
            }
        }

    }


    if ( ((right-left)<overallPivot) ){
        if (left < j){ internalQuickSort(array, left, j, overallPivot); }
        if (i < right){ internalQuickSort(array, i, right, overallPivot); }

    }else{
#pragma omp task
        { internalQuickSort(array, left, j, overallPivot); }
#pragma omp task
        { internalQuickSort(array, i, right, overallPivot); }
    }

}




/* generate a random integers from min to max */
int randFrom(int min, int max)
{
    int range = (max - min);
    int div = RAND_MAX / range;
    return min + (rand() / div);
}
