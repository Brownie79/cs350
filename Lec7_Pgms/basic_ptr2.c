// CS 350, Spring 2016
//
// basic_ptr2.c: Show aliasing of pointers
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>
int main(void) {
    int x = 4, *p, *q;
    p = q = &x;    // Make p, q point to x
    printf("%p %p %p\n", p, q, &x);         // all the same address
    printf("%d %d %d\n", *p, *q, (p == q)); // 4 4 true

    int y = 4;
    q = &y;        // Make q point to y
    printf("%d %d %d\n", *p, *q, (p == q)); // 4 4 false

    return 0;
}