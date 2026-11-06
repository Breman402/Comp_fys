#include <stdlib.h>
#include <stdio.h>
#include "test_main.h"

#include "vector.h"

/* *************************************
 * This is a simple test framework that we recommend
 * that you use to test each part of your own code.
 *
 * If you are interested in digging
 * deeper into testing, we recommend
 * cmocka whcih is a better designed
 * testing framework. However, it requiers
 * a somewhat more advanced understanding
 * of C.
 * ************************************/

/* Utility function to create a vector with random values
 *
 * Random numbers are drawn from a uniform distribution in the range [a, b]
 *
 * Note that you should not use rand() when you want random numbers
 * for a simulation. Use GSL instead!! Here it is fine to use, as
 * we don't mind if the values are correlated, we just want some numbers
 * to test our functions on.
 *
 *
 * Parameters
 * ----------
 *  min: Minimum of uniform distribution
 *  max: Maximum of uniform distribution
 *  v1: Vector where random numbers will be written
 *  len: Length of vectors
 */
void
write_random_vector(
        double min,
        double max,
        double *v,
        unsigned int len)
{
    for (int i = 0; i < len; i++) {
        double r = (double)rand()/(double)(RAND_MAX);
        v[i] = min + (max - min) * r;
    }
}

/* ************************************
 * Here follows the first test we want
 * to run
 * ***********************************/
START_TEST(test_elementwise_addition)
{
    // We want to test the result of some known addition
    // For example we know that [1, 2, 3] + [11, 22, 33] should be equal to [12, 24, 36]
    double v1[] = {1., 2., 3.};
    double v2[] = {11., 22., 33.};

    // For statically allocated arrays we can get the length like this
    // Note that this ONLY works for statically allocated arrays
    // which have a size known at compile time
    const unsigned int ndims = sizeof(v1)/sizeof(v1[0]);

    // Create an empty vector to store the answer
    double *answer = (double *) malloc(sizeof(double) * ndims);

    // Calculate the addition
    elementwise_addition(answer, v1, v2, ndims);

    // Now test! Is the calculated answer (test answer) equal to the known answer?
    // The ch_assert_double_eq_tol function checks that the first two arguments
    // are equal up to a certain tolerance (third argument). When working with
    // doubles we will always have some rounding errors and we must allow for a
    // small error
    ck_assert_double_eq_tol(answer[0], 12., 1e-10);
    ck_assert_double_eq_tol(answer[1], 24., 1e-10);
    ck_assert_double_eq_tol(answer[2], 36., 1e-10);

    free(answer);
}

/* ************************************
 * Here is the next test
 * ***********************************/
START_TEST(test_constant_multiplication)
{
    // We want to test the result of some known multiplication
    // For example we know that [1, 2, 3] * 0.5 should be equal to [0.5, 1.0, 1.5]
    double v[] = {1., 2., 3.};
    double a = 0.5;

    // For statically allocated arrays we can get the length like this
    // Note that this ONLY works for statically allocated arrays
    // which have a size known at compile time
    const unsigned int ndims = sizeof(v)/sizeof(v[0]);

    // Create an empty vector to store the answer
    double *answer = (double *) malloc(sizeof(double) * ndims);

    // Calculate the addition
    constant_multiplication(answer, v, a, ndims);

    // Now test! Is the calculated answer (test answer) equal to the known answer?
    ck_assert_double_eq_tol(answer[0], 0.5, 1e-10);
    ck_assert_double_eq_tol(answer[1], 1.0, 1e-10);
    ck_assert_double_eq_tol(answer[2], 1.5, 1e-10);

    free(answer);
}

/* ************************************
 * More tests...
 * ***********************************/
START_TEST(test_dot_product)
{
    // We want to test the result of some known dot product
    // For example we know that [1, 2, 3] dot [11, 22, 33] should be equal to 143
    double v1[] = {1., 2., 3.};
    double v2[] = {11., 22., 33.};

    // For statically allocated arrays we can get the length like this
    // Note that this ONLY works for statically allocated arrays
    // which have a size known at compile time
    const unsigned int ndims = sizeof(v1)/sizeof(v1[0]);

    // Calculate the addition
    double answer = dot_product(v1, v2, ndims);

    // Now test! Is the calculated answer (test answer) equal to the known answer?
    ck_assert_double_eq_tol(answer, 154., 1e-10);
}

/* ************************************
 *
 * So far we have made tests based on known results of some operations.
 * It is also a good idea to generate random input vectors and test for
 * mathematical properties of the functions, such as associativity,
 * commutativity, etc..
 *
 * See the tests below
 *
 *
 * ***********************************/
START_TEST(test_elementwise_addition_properties)
{
    // Since this test is done with random input vectors,
    // repeat it 10 times
    for (int i = 0; i < 10; i++) {

        // Vectors to hold the inputs and results
        double v1[5] = {0};
        double v2[5] = {0};
        double v3[5] = {0};
        double left[5] = {0};
        double right[5] = {0};
        const unsigned int ndims = sizeof(v1)/sizeof(v1[0]);

        // Generate three random vectors of length 5
        write_random_vector(-100.0, 100.0, v1, ndims);
        write_random_vector(-100.0, 100.0, v2, ndims);
        write_random_vector(-100.0, 100.0, v3, ndims);

        // Test the commutative property of addition:
        //    a + b = b + a
        elementwise_addition(left, v1, v2, ndims);
        elementwise_addition(right, v2, v1, ndims);

        check_vectors_equal(left, right, ndims, 1e-10);

        // Test the associative property of addition:
        //   (a + b) + c = a + (b + c)
        elementwise_addition(left, v1, v2, ndims);
        elementwise_addition(left, left, v3, ndims);
        elementwise_addition(right, v2, v3, ndims);
        elementwise_addition(right, right, v1, ndims);
        check_vectors_equal(left, right, ndims, 1e-10);

        // Test the identity property of addition:
        //   a + 0 = a
        write_random_vector(0.0, 0.0, v2, ndims);  // Write zeros to v2
        elementwise_addition(left, v1, v2, ndims);

        check_vectors_equal(left, v1, ndims, 1e-10);
    }
}

START_TEST(test_constant_multiplication_properties)
{
    // Since this test is done with random input vectors,
    // repeat it 10 times
    for (int i = 0; i < 10; i++) {

        // Vectors to hold the inputs and results
        double v1[5] = {0};
        double a1;
        double a2;
        double left[5] = {0};
        double right[5] = {0};
        const unsigned int ndims = sizeof(v1)/sizeof(v1[0]);

        // Generate a random vector of length 5
        write_random_vector(-100.0, 100.0, v1, ndims);

        // Generate two random numbers
        write_random_vector(-100.0, 100.0, &a1, 1);
        write_random_vector(-100.0, 100.0, &a2, 1);

        // Test the associative property of multiplication:
        //   (a * b) * c = a * (b * c)
        constant_multiplication(left, v1, a1, ndims);
        constant_multiplication(left, left, a2, ndims);
        constant_multiplication(right, v1, a1 * a2, ndims);
        check_vectors_equal(left, right, ndims, 1e-10);

        // Test the identity property of multiplication:
        //   a * 1 = a
        constant_multiplication(left, v1, 1.0, ndims);
        check_vectors_equal(left, v1, ndims, 1e-10);

        // Test multiplying by a number, and then its inverse
        //   (a * b) * (1/b) = a
        constant_multiplication(left, v1, a1, ndims);
        constant_multiplication(left, left, 1.0 / a1, ndims);

        check_vectors_equal(left, v1, ndims, 1e-10);
    }
}

START_TEST(test_dot_product_properties)
{
    // Since this test is done with random input vectors,
    // repeat it 10 times
    for (int i = 0; i < 10; i++) {

        // Vectors to hold the inputs and results
        double v1[5] = {0};
        double v2[5] = {0};
        double v3[5] = {0};
        double a1;
        double a2;
        double left;
        double right;
        double tmp[5] = {0};
        const unsigned int ndims = sizeof(v1)/sizeof(v1[0]);

        // Generate a random vector of length 5
        write_random_vector(-100.0, 100.0, v1, ndims);
        write_random_vector(-100.0, 100.0, v2, ndims);
        write_random_vector(-100.0, 100.0, v3, ndims);

        // Generate two random numbers
        write_random_vector(-100.0, 100.0, &a1, 1);
        write_random_vector(-100.0, 100.0, &a2, 1);

        // Test the commutative property of the dot product:
        //    a . b = b . a
        left = dot_product(v1, v2, ndims);
        right = dot_product(v2, v1, ndims);
        ck_assert_double_eq_tol(left, right, 1e-10);

        // Test the distributive property of the dot product:
        //    a . (b + c) = (a . b) + (a . c)
        elementwise_addition(tmp, v2, v3, ndims);
        left = dot_product(v1, tmp, ndims);
        right = dot_product(v1, v2, ndims);
        right += dot_product(v1, v3, ndims);
        ck_assert_double_eq_tol(left, right, 1e-10);
    }
}

int
main()
{
    // Some initalization and runs the setup function //
    test_setup("testing", "core");

    srand(42);  // Seed the random numbers. Since the seed is always
                // the same, the random numbers will also be the same
                // in subsequent runs of ./run-test (i.e. any failures
                // will be reproduced).

    // add the tests //
    add_test(test_elementwise_addition);
    add_test(test_constant_multiplication);
    add_test(test_dot_product);
    add_test(test_elementwise_addition_properties);
    add_test(test_constant_multiplication_properties);
    add_test(test_dot_product_properties);

    // Runs the tests and the teardown function and frees some data //
    test_teardown();
    return 0;
}
