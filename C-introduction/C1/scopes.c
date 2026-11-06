#include <stdio.h>
    
int
main()
{
    // This used to look like this:
    //{
	//int b = 1;
    //}
    // which made it out of scope and unable to compile
    // Instead I declare in scope:
    int b = 1;
    printf("b: %i\n", b);
    return 0;
}
