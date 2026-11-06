#include <stdlib.h>
#include <stdio.h>

int
main()
{   
    // Original value is 1000
    float *heap_array = (float *)malloc(sizeof(float) * 20971520000);
    if(heap_array == NULL){
	perror("malloc failed");
	exit(1);
    }
    heap_array[99] = 99;
    printf("%f\n", heap_array[99]);
    free(heap_array);
    return 0;
}
