#include <stdio.h>

int
main()
{
    int m = 2; int n = 5;
    double x = 5.6;

    double mn = m / n;
    double xm = x / m;
    double y = m;
    int kx = x;
    int kxn = x - n;

    // %f prints a float
    // %i prints an integer
    printf("m / n = %f\n"
	   "x / m = %f\n"
	   "y = %f\n"
	   "kx = %i\n"
	   "kxn = %i\n", mn, xm, y, kx, kxn);
    return 0;
}
