#include <string.h> //memset
#include <stdio.h>

int *
return_stack()
{
    int array[100];
    for(int i = 0; i < 100; ++i){
       array[i] = i;
    }
    return &array[0];
}

int
main()
{
    int a = 0;
    int b = 5;
    int array_tmp[100];
    int *array = return_stack();
    memset(array_tmp, 0, sizeof(array_tmp));

    // This is likely one of the best ways to initalize
    // an array to be 0
    float array_2[10000];
    memset(array_2, 0, sizeof(array_2));

    for(int i = 0; i < 100; ++i){
	printf("%i\n", array[i]);
    }

    return 0;
}
