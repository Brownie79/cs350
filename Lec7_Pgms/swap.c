// CS 350, Spring 2016
//
// swap.c: Emulate call-by-reference using pointers
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>

// Function prototypes
void swap(int *v1, int *v2);
void badswap(int x1, int x2);

int main() {
    int x = 2, y = 3;
    printf("x and y are at %p and %p\n", &x, &y);
    printf("Before swap, x = %d, y = %d\n\n", x, y);

    swap(&x, &y);			// Now x == 3, y == 2
    printf("After swap, x = %d, y = %d\n\n", x, y);

    badswap(x, y);  		// Doesn't affect x or y
    printf("Back in main, after bad swap, x = %d, y = %d\n", x, y);

    return 0;
}

void swap(int *v1, int *v2) {
    printf("Swapping values at %p and %p\n", v1, v2);
    int temp = *v1;
    *v1 = *v2;
    *v2 = temp;
}

void badswap(int x1, int x2) {
    printf("Bad swap of values at %p and %p\n", &x1, &x2);
    printf("At start of bad swap, x1 = %d, x2 = %d\n", x1, x2);
    int temp = x1;
    x1 = x2;
    x2 = temp;
    printf("At end   of bad swap, x1 = %d, x2 = %d\n\n", x1, x2);
}