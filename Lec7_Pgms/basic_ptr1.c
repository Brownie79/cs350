// CS 350, Spring 2016
//
// basic_ptr1.c: Declare and manipulate pointers
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>
int main(void) {
    int x = 4;                  // integer variable
    int *p = NULL;              // pointer to integer
    // (NULL is used to point to nowhere)

    p = &x;                     // Make p point to x
    printf("%p %p\n", &x, p);   // print same address twice
    printf("%d %d\n", x, *p);   // Print 4 and 4

    x = 5;                      // Change x (and also *p)
    printf("%d %d\n", x, *p);   // Print 5 and 5

    *p = 6;                     // Change *p (and also x)
    printf("%d %d\n", x, *p);   // Print 6 and 6

    *p = *p+1;                  // x and *p = 7 (= 6+1)
    printf("%d %d\n", x, *p);   // Print 7 and 7

    return 0;
}

/* Sample output (note addresses can differ on your machine)

0x7fff5b775ae8 0x7fff5b775ae8
4 4
5 5
6 6
7 7

*/