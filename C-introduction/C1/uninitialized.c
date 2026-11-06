#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mypow(
	  int base,
	  int power
         )
{
    int ret = 1;
    for(int i = 0; i < power; ++i){
	ret *= base;
    }
    return ret;
}


int main()
{
    /* ****************************************************
     *
     * This is incorrect code but shows you the pitfall
     * of using unitialized data.
     *
     * *****************************************************/
    
    float a[200];
    /* *****************************************************
     *
     * Here a = b[-2] which illustrates that you can
     * never for sure know what you memory will be 
     * initalized too
     *
     * ****************************************************/
    printf("float uncorrectly initalized\n");
    double sum_a = 0;
    for(int i = 0; i < sizeof(a) / sizeof(a[0]); ++i){
	sum_a += (double)a[i];
    }
    printf("sum(a) = %f\n", sum_a);
   
    /* ******************************************************
     * Note that the argument to malloc is number of bytes
     * and not the number elements in the "array".
     *
     * There is no guarantee what will be in this memory
     * you have to initalize it yourself.
     *
     * However, it might be so that every element is zero
     * by chance.
     * ******************************************************/
    printf("\nIncorrectly initalized heap array\n");
    for(int i = 0; i < 5; ++i){
	int *arr_heap = malloc(sizeof(int) * 9000);
    	int sum_b = 0;
    	for(int j = i; j < 9000; ++j){
    	    sum_b += arr_heap[j];
	    arr_heap[j] += (mypow(-1, j) * (j % 625)) * i;
    	}
    	printf("sum(heap) = %i\n", sum_b);
    	free(arr_heap); arr_heap = NULL;
    }
  

    /* ***************************************************
     * 
     * Calloc gives you memory that has been initialized
     * to 0
     *
     * ****************************************************/ 
    printf("\nCorrectly initalized heap array\n");
    for(int i = 0; i < 5; ++i){
	int *arr_heap = calloc(9000, sizeof(int));
    	int sum_b = 0;
    	for(int i = 0; i < 9000; ++i){
    	    sum_b += arr_heap[i];
	    arr_heap[i] += (double)mypow(-1, i) * (i % 626) * 2.1;
    	}
    	printf("sum(heap) = %i\n", sum_b);
    	free(arr_heap); arr_heap = NULL;
    }
    return 0;
}
