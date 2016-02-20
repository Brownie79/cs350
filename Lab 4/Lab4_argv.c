// CS 350, Spring 2016
// Sample program for argc/argv and file I/O
//
// Illinois Institute of Technology, (c) 2015, James Sasaki
//
#include <stdio.h>

// Usage: executable_filename file_to_read_in
// E.g.: ./a.out myfile.txt
// Print out the command line we were executed with.
// Open the specified file and repeatedly read a decimal integer
// and print it.  Stop if we hit end-of-file or we find something
// that doesn't look like a decimal integer.

int main (int argc, char *argv[])
{
	// First we print out the command line that we were executed with. 
	// argc has the number of words on the command line and argv[0], ...
	// contain the different words.  The name of the command (e.g.
	// "./a.out") is the initial word.
	//
	printf("The command line had %d part(s)\n", argc);
	int i;
	for (i = 0; i < argc; i++)
		printf("argv[%d] = %s\n", i, argv[i]);
	if (argc != 2) {
		printf("\nBad command; should be %s filename\n", argv[0]);
		return 1;
	}

	// Use the first command argument as the file to open.
	// If we can't open the file, we complain and quit.
	//
	char *filename = argv[1];			// file to read from
	FILE *in_file;
	in_file = fopen(filename, "r");		// try to open file
	if (in_file == NULL) {
		printf("Couldn't open file %s\n", filename);
		return 1;
	}

	// The file opened correctly, so now we can read a sequence
	// of numbers from the file.  Note: A call of fscanf returns
	// the number of values the call read successfully (in our
	// case, 1 if it read a value, 0 if it didn't).  
	//
	int nbr_values_read;	// # items read by call of fscanf
	int value_read;	
	nbr_values_read = fscanf(in_file, "%d", &value_read);
	while (nbr_values_read > 0) {
		printf("Read %d\n", value_read);
		nbr_values_read = fscanf(in_file, "%d", &value_read);
	}

	int close_err = fclose(in_file);	// yields false if the close succeeded
	if (close_err) {
		printf("File close failed!\n");
	}
	return 0;
}
