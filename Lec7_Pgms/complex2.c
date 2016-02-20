// CS 350, Spring 2016
//
// complex2.c: Functions on complex number structures
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>

// In version 2, we declare and use functions to
// manipulate struct complex values.  For space
// efficiency and to simulate call-by-reference,
// the functions take pointers to struct complex values.

// Declare struct complex to be a structure
// with real and imaginary fields
//
struct complex {
    double real_part;
    double imag_part;
};  // <-- note semicolon here

// Prototypes
void set_cpx(struct complex *p, double a, double b);
void cpx_print(struct complex *x);

// Create and manipulate a struct complex value
//
int main(void) {
    struct complex val, *x = &val;
    set_cpx(x, 1.1, 2.2);
    cpx_print(x);
    return 0;
}

// set_cpx(x, a, b) sets *x to a + bi
//
void set_cpx(struct complex *x, double a, double b) {
    (*x).real_part = a;
    (*x).imag_part = b;
}

// cpx_print(x) prints *x in a + bi format.
//
void cpx_print(struct complex *x) {
    printf("%f + %f i\n", (*x).real_part, (*x).imag_part );
}