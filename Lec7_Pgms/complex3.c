// CS 350, Spring 2016
//
// complex3.c: Functions on complex number structures
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>

// In version 3, we declare and use Complex to be an
// abbreviation for struct complex, and we use ptr -> field
// syntax instead of (*ptr).field syntax.

// Declare Complex to be a structure
// with real and imaginary fields
//
struct complex {
	double real_part;
	double imag_part;
};  // <-- note semicolon here
typedef struct complex Complex;

// Prototypes
void set_cpx(Complex *p, double a, double b);
void cpx_print(Complex *x);

// Create and manipulate a Complex value
//
int main(void) {
	Complex x_val, *x = &x_val;
	set_cpx(x, 1.1, 2.2);
	cpx_print(x);
	return 0;
}

// set_cpx(x, a, b) sets *x to a + bi
//
void set_cpx(Complex *x, double a, double b) {
	x -> real_part = a;
	x -> imag_part = b;
}

// cpx_print(x) prints *x in a + bi format.
//
void cpx_print(Complex *x) {
	printf("%f + %f i\n", x -> real_part, x -> imag_part );
}