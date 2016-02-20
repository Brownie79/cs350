//CS 350 Lab 5
//Devanshu Bharel
#include <stdio.h>

int main(void){
	// 1. C declarations
	int *p, *q, *r = NULL;
	int b[4] = {2,4,2,8};
	
	p = &b[0];
	q = &b[1];
	r = &b[2];
	//printf("%p %p %p\n",p,q,r);
	
	//2. 
	//Do the following cause compile warnings
	//Or run time error
	//Evaulate to true or false
	
	if(q-p+q-p == q+q-p-p) 
	{ printf("True\n"); } else { printf("False\n"); }

}



