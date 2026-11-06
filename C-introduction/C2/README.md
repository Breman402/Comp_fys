---
header-includes:
- \usepackage[a4paper, total={6in, 9in}]{geometry}
- \usepackage{fancyhdr}
- \pagestyle{fancy}
- \fancyhead[L]{FKA121/FIM540 Computational physics}
- \fancyhead[R]{C2 First program}
colorlinks: true
output: pdf_document
---
Exercise C2: First program
====

Software
====
For this task you need to install GSL, the GNU Scientific Library. You can
think of this as the C version of Python's Numpy and Scipy libraries.
To do this on the stuDAT computers, run the included installation script as:

```
bash ./install_gsl.sh
```

This should work on other Linux computers as well, however, some package managers
have it in their repositories.
This script does not work on Mac or Windows computers, however!

There is example code how to generate random numbers using GSL in the file
gsl_example.c.
compile as

```
export loc=${HOME}/.local
gcc gsl_example.c -I${loc}/include -L${loc}/lib -Wl,-rpath,${loc}/lib -lgsl -lgslcblas -o gslt
```

Explanation of the flags:

 - -I${loc}/include tells the compiler to look for header files in the directory ${loc}/include
 - -L${loc}/lib tells the compiler to look for libraries in the directory ${loc}/lib
 - -lgsl -lgslcblas tell the linker to include the libraries gsl and gslcblas
 - -Wl,-rpath,${loc}/lib tells the linker where to look for the libraries
 - -o gslt tells gcc to name the program as gslt


Introduction
===
In this exercise you will have to start coding yourself, the program that you
will have to write covers some topics that will help you throughout the course.
For example, how to write your data to a file.
In particular, it will prepare you for the C4 exercise.

Dynamic memory allocation and arrays
===
 * Write a program with a function that computes the scalar product between two
   arrays.
   In the main function, read a length from the command line and create two
   vectors of that length for which you calculate the scalar product.
   Hint: Allocate the arrays dynamically and use the scanf function to read an
   int representing the length from the command line.
 * Write a function that dynamically allocates a 2D array and use it to create
   an nx3 array  (i.e. having n rows and 3 columns) filled with doubles of your choice.
   Interpreting each row in this array as the coordinate vector of a point in
   space, write a function that calculates the distance between two given
   points.
 * Write the matrix to a file in a csv format.

Header files and separate sources
===

__Header files__

Header files contains function prototypes for its corresponding C file.
For example, if you have a C file, linalg.c, that can, e.g., calculate the
scalar product.
Then, if a second C file, main.c, wants to calculate the scalar product
it has to include the linalg header file.
Remember that header files should have [include guards](https://en.wikiedia.org/wiki/Pragma_once).
Include guards makes sure that you are not including the header file twice.

This could look something like

_linalg.h_:
```
#pragma once // This is the include guard 
double scalar_product(double *, double *);
```

_linalg.c_:
```
double scalar_product(double *a, double *b){
...
}
```

_main.c_:
```
// "" looks for the linalg.h file in the current directory
// <> looks for the linalg.h file in the system libraries
// Therefore, use "" for user defined header files
#include "linalg.h"

int main()
{
    ...
    scalar_product(a, b);
    ...
}
```

Rewrite your code from the previous exercise so that the two functions
(scalar product and pairdistance) are located in a separate C-file.
Create a corresponding header file where you put the function prototypes.
Recompile your program and make sure that everything works.

Using external libraries, Makefiles
===
Use GSL to generate a large array filled with random numbers drawn from the
uniform distribution on [0,1].
Write the array to a file and import the data into Python and make
a histogram to verify that the distribution is indeed uniform.
Use the template Makefile located in this repo to link
the GSL libraries.
