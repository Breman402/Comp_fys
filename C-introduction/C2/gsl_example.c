#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <time.h>

int
main()
{
    const gsl_rng_type * T;
    gsl_rng * r;

    int i, n = 10;
    gsl_rng_env_setup();

    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    // typically the seed is set to the current time.
    //time_t seed = time(NULL);
    int seed = 42;
    gsl_rng_set(r, seed); 

    for (i = 0; i < n; i++){
	double u = gsl_rng_uniform(r);
	double g = gsl_ran_gaussian(r, 1);
	printf ("%.5f %.5f\n", u, g);
    }

    gsl_rng_free (r);

    return 0;
}
