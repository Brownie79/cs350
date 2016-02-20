// Compile this file on alpha.cs.iit.edu with gcc -Wall -std=c99 -lm Lab00_sample.c

// CS 350, Spring 2016
// Lab 1: Basic C Programming
//
// Illinois Institute of Technology, (c) 2016, James Sasaki

#include <stdio.h>  // to access printf, scanf
#include <math.h>   // to access sqrt (be sure to compile with -lm for math library)

int main() {
  // To use printf (print formatted), you provide a format string
  // and a list of values.  The format string can contain literal
  // characters to print and format codes (with %) to indicate how
  // to print the values.
  //  
  // Use the %d format code to print an integer; use %f for a
  // floating- point or double precision number; use %c for a
  // single character.  An integer after the % specifies the width
  // of the output.
  // 
  int x = 255;
  double rootx = sqrt(x);
  char bang = '!';
  printf("The square root of %d = %f %c%c\n", x, rootx, bang, bang);

  // Problem 1: (10 = 2*5 pts)
  // Uncomment the printf below and recompile the program
  // (1) using gcc -Wall and (2) just gcc.
  // 1a: Do the two compiles behave differently under (1) and (2)?  If so, how?
  // 1b: Does the program execute differently under (1) and (2)?
  //
  // printf("The square root of %f = %f %c%c\n", x, rootx, '?', '?');

  // Some modified format codes:
  //
  // 1. %wd and %wf (where w is a positive integer) specifies the
  // minimum width w of the output.  E.g., %5d for a five-digit
  // integer (or minus sign and four digits).  If the value is too
  // large to print in w columns, then the natural number of columns
  // is used.
  //
  // 2. Put a 0 before the width (if any) to get leading zeros. 
  // E.g., %05d for a(n at least) five-digit integer with leading zeros.
  //
  // 3. To specify a specific number of decimal places for a
  // floating-point number, add a dot and the number of places.
  // E.g., %.3f to get 3 decimal places with an unspecified width
  // or %10.3f to get 6 places, a decimal point, and 3 places
  // (for an overall width of 10).
  //
  printf("The square root of %05d = %.3f = %10.4f\n", x, rootx, rootx);

  // Problem 2 (12 = 3*4 pts)
  // Uncomment the code below and recompile and rerun the program.
  // (a) What is the output of the printf?  (b) Change x = 1; to
  // x = 123456;  Now what is the output?  (c) What does
  // the 3 in the format %3d mean?
  // [Recomment the code before going on to the next problem.]
  //
  // x = 1;
  // rootx = sqrt(x);
  // printf("The square root of %3d = %.3f = %10.4f\n", x, rootx, rootx);
  
  // You can't print out an array of numbers all at once.
  //
  int xx[2] = {3, 5};	// Declare and initialize array xx
  printf("xx: %d, %d\n", xx[0], xx[1]);
  
  // Problem 3 (8 = 2*4 pts)
  // Uncomment the printf below and recompile and rerun using gcc
  // and gcc -Wall.  (a) Do you get the same error messages?
  // (b) Do you get the same (bizarre) output?
  // [Recomment the code before going on to the next problem.]
  // printf("xx: %d, %d\n", xx);

  // In C, a string is an array of characters with one extra
  // character '\0' (the null character), at the end of the
  // string. You can print a character array as a string using the
  // printf format code %s.  You can also print a string constant using %s.
  //
  char hello[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
  printf("hello: %s\n", hello);
  printf("%s again\n", "hello");

  char empty_string[1] = {'\0'};
  printf("empty string: [%s]\n", empty_string);
  printf("\n");
  
  // You can initialize a character array using a string constant,
  // which can make life easier.
  //
  char hello2[6] = "hello";
  char empty_string2[1] = "";  // empty string constant
  printf("hello2: %s\n", hello2);
  printf("Another empty string: <%s>\n\n", empty_string2);
  
  // Problem 4 (8 = 2*4 pts)
  // Uncomment the code below and recompile and rerun it.
  // (a) No explicit length for hello3 was given; what
  // happened instead?  (b) Change the declaration
  // of hello3 to have a length less than the length of the
  // string constant.  What happens if you recompile (and possibly rerun)?
  // [Recomment the code before going on to the next problem.]
  //
  // char hello3[] = "hello!!?  hi?";
  // printf("hello3: %s\n", hello3);

  // We can use scanf (formatted scan) to read from the keyboard. 
  // Like printf, scanf takes a format string. Unlike printf,
  // instead of providing a list of variables to read into, you
  // provide a list of **addresses** into which to read.  For
  // variables of basic types like int, double, or char, you
  // add an ampersand before the variable.  We'll study the
  // ampersand more later, but basically it says to get the
  // address of the thing following it.
  //
  int y;
  printf("Enter an fairly small integer >= 0 (and then return): ");
  scanf("%d", &y);
  printf("We read in %d\n\n", y);
  
  // sscanf is like scanf but instead of reading from the
  // keyboard, it reads from a string.  Unused input is ignored.
  //
  // Problem 5 (12 = 3*4 points)
  // (a) Explain why the outputs of the two sscanf statements
  // below give different values for h.  (b)  Change the
  // "Using sccanf %%f..." in the printf below to 
  // "Using sscanf %f..." and recompile.  What happens?  (c) What
  // does a %% in a printf format string indicate?
  //
  char buffer[] = "-25.70 junk";
  float h;
  sscanf(buffer, "%d%f", &y, &h);
  printf("Using sscanf %%d%%f, we read in %d and %f from %s\n",
  	y, h, buffer );
  sscanf(buffer, "%f", &h);
  printf("Using sscanf %%f, we read in %f from %s\n\n", h, buffer);
  return 0;
}
