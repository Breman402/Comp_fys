#include <stdio.h>

int
main()
{ 
    int m = 2;
    int *pm = &m;
    /* A pointer to a pointer */
    //int **ppm = &pm;

    printf("m + 1 = %i\n", m + 1);
    printf("(*pm) + 1 = %i\n", (*pm) + 1);
    printf("pm = %p\n", pm);
    printf("*(&m) + 1 = %i\n", *(&m) + 1);
    return 0;
}
