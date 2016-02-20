// Devanshu Bharel
// CS 350, Spring 2016
// Lab 4 -- Breaking up a bitstring

// This program processes each line of a file; the filename is
// specified as a command-line argument (argv[1]) or defaults to
// Lab4_data.txt. Each line should contain a hex integer value and
// two decimal integers, len1 and len2. We find the leftmost len1
// bits of the value, the next len2 bits of the value, and the
// rightmost len3 = (32 - (len1 + len2)) remaining bits. (All 3
// values should be > 0 else we signal an error and skip to the
// next line of input.)  We print the three bitstrings in hex and
// in decimal. The leftmost string is unsigned, the middle string
// is in 1's complement, and the rightmost string is in 2's
// complement.

#include <stdio.h>

int main (int argc, char *argv[]) {

	// Use the first command argument as the file to open.
	// If we can't open the file, we complain and quit.
	char *filename = argv[1];			// file to read from
	FILE *in_file;
	in_file = fopen(filename, "r");		// try to open file
	if (in_file == NULL) {
		printf("Couldn't open file %s\n", filename);
		printf("Defaulting to 'Lab4_data.txt'\n");
		in_file = fopen("Lab4_data.txt","r");
		if(in_file == NULL){
			printf("Default file 'Lab4_data.txt' not found. Exiting!\n");
			return 0;
		}
	}

	// Repeatedly read and process each line of the file.  The
	// line should have a hex integer and two integer lengths.
	//
	int val, len1, len2, len3;
	int nbr_vals_on_line
		= fscanf(in_file, "%x %d %d", &val, &len1, &len2);

	// Read until we hit end-of-file or a line without the 3 values.
	while (nbr_vals_on_line == 3) {
		// We're going to break up the value into bitstrings of
		// length len1, len2, and len3 (going left-to-right).
		// The user gives us len1 and len2, and we calculate
		// len3 = the remainder of the 32 bits of value.
		// All three lengths should be > 0, else we complain
		// and go onto the next line.
		//
		len3 = 32 - (len1 + len2);

		if(len1 <= 0 || len2 <= 0 || len3 <= 0){
			printf("Error! One or more lengths not > 0\n");
			printf("Value: %x \n Len1: %d \n Len2: %d \n Len3: %d \n\n",val,len1,len2,len3);
		} else {
			// Calculate the 3 bitstrings x1, x2, x3 of lengths
			// len1, len2, and len3 (reading left-to-right).
			
			//x1
			unsigned int x1 = val >> (len2+len3);
			
			//x2
			//first move it left
			unsigned int x2 = val << len1;
			//then push to the right side
			x2 = x2 >> (len1+len3);

			//x3
			//move the rightmost digits to the leftmost, filling the emptiness with 0s 
			unsigned int x3 = val << (len1+len2); 
			//then move it back to the right because 1010100000... is different than ...000010101
			x3 = x3 >> (len1+len2);

			// Calculate the value of x2 read as a 1's complement int.
			int x2_complement = ~x2; //flip all the bits
			x2_complement = x2_complement << (32-len2); //0 it out
			x2_complement = x2_complement >> (32-len2); //move it back to the low value position
			x2_complement = -x2_complement;//we took the complement so remember the (-)

			// Calculate the value of x3 read as a 2's complement int.
			//1's Complement
			int x3_complement = ~x3; //flip all the bits
			x3_complement <<= (32-len3); //0 it out
			x3_complement >>= (32-len3); //move it back to the low value position
			x3_complement = -(x3_complement+1); //add 1 cause 2's complement //we took the complement so remember the (-) 

			//x2 test
			int x2_test = ~x2;

			// Print out the original value as a hex string, print out
			// (going left-to-right) each length (in decimal) and selected
			// bitstring (in hex), and its decimal value.  We read x1 as
			// unsigned, x2 in 1's complement, and x3 in 2's complement.
			//
			printf("Value = %#08x = %d\n", val,val);
			printf("Its leftmost  %2d bits are %#x = %u as an unsigned integer\n", 		len1, x1, x1 );
			printf("Its next      %2d bits are %#x : %#x = %d as 1's Complement\n", 		len2, x2, x2_complement,x2_complement );
			printf("Its remaining %2d bits are %#x : %#x = %d as 2's Complement\n", 		len3, x3, x3_complement,x3_complement );
			printf("x2_test = %d = %x", x2_test,x2_test);
		}
		printf("\n");
		nbr_vals_on_line = fscanf(in_file, "%x %d %d", &val, &len1, &len2);
	}

	int close = fclose(in_file);
	if(close){
		printf("File close failed!\n");
		return 1;	
	}

	return 0;
}