#include <stdio.h>
#include <stdlib.h>       // For error exit()

// Prototypes [note the functions are also declared in this order]
//
	int main(int argc, char *argv[]);
	void initialize_control_unit(int reg[], int nreg);
	void initialize_memory(int argc, char *argv[], int mem[], int memlen);
	FILE *get_datafile(int argc, char *argv[]);

	void dump_control_unit(int pc, int ir, int running, int reg[], int nreg);
	void dump_memory(int mem[], int memlen);
	void dump_registers(int reg[], int nreg);

	int read_execute_command(int reg[], int nreg, int mem[], int memlen);
	int execute_command(char cmd_char, int reg[], int nreg, int mem[], int memlen);
	void help_message(void);
	void many_instruction_cycles(int nbr_cycles, int reg[], int nreg, int mem[], int memlen);
	void one_instruction_cycle(int reg[], int nreg, int mem[], int memlen);
	
	void exec_HLT(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen);
	void exec_LD(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen);
	void exec_ST(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen);
	void exec_ADD(int reg_R,int addr_MM, int reg[], int nreg, int mem[], int memlen);
	void exec_NEG(int reg_R, int reg[], int nreg); 
	void exec_LDM(int reg_R,int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen); 
	void exec_ADDM(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen); 
	void exec_BR(int addr_MM, int mem[], int memlen); 
	void exec_BRC(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen); 
	void exec_GETC(int reg[], int nreg); 
	void exec_OUT(int reg[], int nreg); 
	void exec_PUTS(int addr_MM, int mem[], int memlen); 
	void exec_DMP(int addr_MM, int reg[], int nreg, int mem[], int memlen); 
	void exec_MEM(int addr_MM, int reg[], int nreg, int mem[], int memlen); 

// CPU declarations
//
#define NREG 10
#define MEMLEN 100
// Note horrible use of global variables
	int pc;               // Program counter
	int ir;               // Instruction register
    int running;          // true iff instruction cycle is active
	int reg[NREG];        // general-purpose registers
	int mem[MEMLEN];      // main memory


// Main program: Initialize the cpu, read initial memory values,
// and execute the read-in program starting at location 00.
//
int main(int argc, char *argv[]) {
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
	printf("=-=-=-=-=-=-= SDC SIM lab06 CS350 Devanshu Bharel =-=-=-=-=-=-=\n");
	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
	initialize_control_unit(reg, NREG);
	initialize_memory(argc, argv, mem, MEMLEN);

	char *prompt = "> ";
	printf("\nBeginning execution; type h for help\n%s", prompt);

	int done = read_execute_command(reg, NREG, mem, MEMLEN);
	while (!done) {
		printf("%s", prompt);
		done = read_execute_command(reg, NREG, mem, MEMLEN);
	}
	printf("At termination\n");
	dump_control_unit(pc, ir, running, reg, NREG);
	dump_memory(mem, MEMLEN);
	return 0;
}


// Initialize the control registers (pc, ir, running flag) and
// the general-purpose registers
//
void initialize_control_unit(int reg[], int nreg) {
	pc = 0;
	ir = 0;
	running = 1;
	int i;
	for(i=0;i<nreg;i++){
		reg[i]=0;
	}
	printf("\nInitial control unit:\n");
	dump_control_unit(pc, ir, running, reg, nreg);
	printf("\n");
}

// Read and dump initial values for memory
//
void initialize_memory(int argc, char *argv[], int mem[], int memlen) {
	FILE *datafile = get_datafile(argc, argv);

	// Will read the next line (words_read = 1 if it started
	// with a memory value). Will set memory location loc to
	// value_read
	//
	int value_read, words_read, loc = 0, done = 0;

	// Each getline automatically reallocates buffer and resets
	// buffer_len so that we can read in the whole line of
	// input. bytes_read is 0 at end-of-file.  Note we must
	// free the buffer once we're done with this file.
	//
	// See linux command man 3 getline for details.
	//
	char *buffer = NULL;
	size_t buffer_len = 0, bytes_read = 0;

	// Read in first and succeeding memory values. Stop when we
	// hit a sentinel value, fill up memory, or hit end-of-file.
	//
	bytes_read = getline(&buffer, &buffer_len, datafile);
	int range = MEMLEN-1;
	while (bytes_read != -1 && !done) {
		// If the line of input begins with an integer, treat
		// it as the memory value to read in.  Ignore junk
		// after the number and ignore blank lines and lines
		// that don't begin with a number.
		//
		words_read = sscanf(buffer, "%d", &value_read);
//		printf("\n%d %d",words_read,value_read);
		if (words_read == 1){
			// set memory value at current location to
			// value_read and increment location.  Exceptions: If
			// loc is out of range, complain and quit the loop. If
			// value_read is outside -9999...9999, then it's a
			// sentinel and we should say so and quit the loop.
			//
			if(loc>range) {
				printf("\nerror: location %d is outside range",value_read);
				done = 1;
			} else if (value_read < -9999 || value_read > 9999){
				printf("\nSentinel %d at location %d",value_read,loc);
				done = 1;
			} else {
				mem[loc]=value_read;
				loc++;
			}
		} 
		// Get next line and continue the loop
		//
		bytes_read = getline(&buffer, &buffer_len, datafile);
	}
	free(buffer);  // return buffer to OS
	
	// Initialize rest of memory
	//
	while (loc < memlen) {
		mem[loc++] = 0;
	}
	dump_memory(mem, memlen);
}

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.sdc.  If file opening fails, complain
// and terminate program execution with an error.
// See linux command man 3 exit for details.
//
FILE *get_datafile(int argc, char *argv[]) {
	char *default_datafile_name = "default.sdc";
	char *datafile_name;
	
	if(argc > 1) {
		datafile_name=argv[1];
	} else {
		datafile_name=default_datafile_name;
	}
	printf("Initialize memory from %s", datafile_name);

	// if the open failed, complain and call
	// exit(EXIT_FAILURE); to quit the entire program
	FILE *datafile = fopen(datafile_name, "r");
	if(!datafile){
		printf("\n error: file %s could not be opened. quitting.\n", datafile_name);
		exit(1);
	}

	return datafile;
}

// dump_memory(mem, memlen): Print memory values in a table,
// ten per row, with a space between each group of five columns
// and with a header column of memory locations.
//
void dump_memory(int mem[], int memlen) {
	int loc = 0;
	int row, col;
	for(row=0;row<10;row++) {	
		printf("\n");
		for(col=0;col<11;col++) {
			if(col == 0) {
				printf("  %d0:",row);
			} else {
				printf("%6d",mem[loc]);
				if(col == 5) {
					printf("  ");
				}
				loc++;
			}
		}
	}
	printf("\n");
}

// dump_control_unit(pc, ir, running, reg): Print out the
// control and general-purpose registers
// 
void dump_control_unit(int pc, int ir, int running, int reg[], int nreg) {
	printf("  PC:   %02d  IR: %04d  RUNNING: %d\n",pc,ir,running);
	dump_registers(reg, nreg);
}

// dump_registers(reg, nreg): Print register values in two rows of five.
//
void dump_registers(int reg[], int nreg) {
	int j;
	for(j=0;j<nreg;j++){
		if(j == 5) {
			printf("\n");
		}
		printf("  R%d: %4d  ",j,reg[j]);
	}
	printf("\n");
}

// Read a simulator command from the keyboard ("h", "?", "d", number,
// or empty line) and execute it.  Return true if we hit end-of-input
// or execute_command told us to quit.  Otherwise return false.
//
int read_execute_command(int reg[], int nreg, int mem[], int memlen) {
	// Buffer for the command line from the keyboard, plus its size
	//
	char *cmd_buffer = NULL;
	size_t cmd_buffer_len = 0, bytes_read = 0;

	// Values read using sscanf of command line
	//
	int nbr_cycles;
	char cmd_char;
	size_t words_read;	// number of items read by sscanf call

	int done = 0;	// Should simulator stop?

	bytes_read = getline(&cmd_buffer, &cmd_buffer_len, stdin);
	if (bytes_read == -1) {
		done = 1;   // Hit end of file
	}
	
	words_read = sscanf(cmd_buffer, "%d", &nbr_cycles);
	// If we found a number, do that many instruction cycles.  
	// Otherwise sscanf for a character and call execute_command with it.
	// (Note the character might be '\n'.)
	//
//	printf("%d %d",words_read,nbr_cycles);
	if (words_read == 1) {
		many_instruction_cycles(nbr_cycles,reg,nreg,mem,memlen);
	} else {
		words_read = sscanf(cmd_buffer,"%c",&cmd_char);
		execute_command(cmd_char,reg,nreg,mem,memlen);
	}

	free(cmd_buffer);
	return done;
}

// Execute a nonnumeric command; complain if it's not 'h', '?', 'd', 'q' or '\n'
// Return true for the q command, false otherwise
//
int execute_command(char cmd_char, int reg[], int nreg, int mem[], int memlen) {
	if (cmd_char == '?' || cmd_char == 'h') {
		help_message();
	} else if (cmd_char == 'd') {
		printf("Dumping control and memory:\n");
		dump_control_unit(pc, ir, running, reg, NREG);
		dump_memory(mem, MEMLEN);
	} else if (cmd_char == 'q') {
		printf("Quitting\n");
		exit(0);
	} else if (cmd_char == '\n' || cmd_char == ' ') {
		one_instruction_cycle(reg,nreg,mem,memlen);
	} else {
		printf("Unkown command: %c; Ignoring it.\n",cmd_char);
		return 1;
	}
	return 0;
}

// Print standard message for simulator help command ('h' or '?')
//
void help_message(void) {
	char *help_message = "Simulator commands:"
		"\nh or ? for help (prints this message)"
		"\nd to dump the control unit"
		"\nAn integer > 0 to execute that many instruction cycles"
		"\nOr just return, which executes one instruction cycle"
		"\n\nSDC Instruction set:"
		"\n 0xxx: HALT"
		"\n 1RMM: Load Reg[R] <- M[MM]"
		"\n 2RMM: Store M[MM] <- Reg[R]"
		"\n 3RMM: Add M[MM] to Reg[R]"
		"\n 4Rxx: Negate Reg[R]"
		"\n 5RMM: Load Immediate Reg[R] <- MM"
		"\n 6RMM: Add Immediate Reg[R] <- Reg[R] +/- MM"
		"\n 7xMM: Branch to MM"
		"\n 8RMM: Branch Conditional to MM"
		"\n 90xx: Read char into R0"
		"\n 91xx: Print char in R0"
		"\n 92MM: Print string starting at MM"
		"\n 93MM: Dump control unit"
		"\n 94MM: Dump memory\n\n";
	printf("%s",help_message);
}

// Execute a number of instruction cycles.  Exceptions: If the
// number of cycles is <= 0, complain and return; if the CPU is
// not running, say so and return; if the number of cycles is
// insanely large, complain and substitute a saner limit.
//
// If, as we execute the many cycles, the CPU stops running,
// then return.
//
void many_instruction_cycles(int nbr_cycles, int reg[], int nreg, int mem[], int memlen) {
	if (nbr_cycles <= 0){
		printf("Error: number of cycles <= 0.\n");
		return;
	} else if (!running){
		printf("Error: CPU is not running.\n");
		return;
	} else if (nbr_cycles > 100){
		printf("%d seems like an inSANE number of cycles. Let's do 100 instead.\n",nbr_cycles);
		nbr_cycles = 100;
	}
	int cycle = 0;
	while (cycle < nbr_cycles && running){
		one_instruction_cycle(reg,nreg,mem,memlen);
		cycle++;
	}
}

// Execute one instruction cycle
//
void one_instruction_cycle(int reg[], int nreg, int mem[], int memlen) {
	// If the CPU isn't running, say so and return.
	// If the pc is out of range, complain and stop running the CPU.
	//
	if (!running){
		printf("Error: CPU not running.\n");
		return;
	}

	// Get instruction and increment pc
	//
	int instr_loc = pc;  // Instruction's location (pc before increment)
	ir = mem[pc++];
	// Decode instruction into opcode, reg_R, addr_MM, and instruction sign
	//
	int opcode = abs(mem[instr_loc] / 1000);
	int	reg_R = abs((mem[instr_loc] / 100) % 10);
	int addr_MM = abs(mem[instr_loc] % 100);
	int instr_sign = 0;
	// double check we're not trying to divide by 0
	if (mem[instr_loc] != 0) {
		instr_sign = mem[instr_loc] / abs(mem[instr_loc]);
	} 
	 
	// Echo instruction
	//
	printf("At %02d instr %d %d %02d: ", instr_loc, opcode, reg_R, addr_MM);

	// basic bounds checks. bail if we're out of range
	if (reg_R < -1 || nreg < reg_R || addr_MM < -1 || 100 < addr_MM) {
		printf("Error: bad instruction. reg_R or addr_MM is out of bounds.\n");
		return;
	}

	switch (opcode) {
	case 0: exec_HLT(reg_R, addr_MM, reg, nreg, mem, memlen); break;
	case 1: exec_LD(reg_R, addr_MM, reg, nreg, mem, memlen); break;
	case 2: exec_ST(reg_R, addr_MM, reg, nreg, mem, memlen); break;
	case 3: exec_ADD(reg_R, addr_MM, reg, nreg, mem, memlen); break;
	case 4: exec_NEG(reg_R, reg, nreg); break;
	case 5: exec_LDM(reg_R, addr_MM, instr_sign, reg, nreg, mem, memlen); break;
	case 6: exec_ADDM(reg_R, addr_MM, instr_sign, reg, nreg, mem, memlen); break;
	case 7: exec_BR(addr_MM, mem, memlen); break;
	case 8: exec_BRC(reg_R, addr_MM, instr_sign, reg, nreg, mem, memlen); break;
	case 9:
		switch (reg_R) {
			case 0: exec_GETC(reg, nreg); break;
			case 1: exec_OUT(reg, nreg); break;
			case 2: exec_PUTS(addr_MM, mem, memlen); break;
			case 3: exec_DMP(addr_MM, reg, nreg, mem, memlen); break;
			case 4: exec_MEM(addr_MM, reg, nreg, mem, memlen); break;
			case 5: 
			case 6:
		   	case 7: 
			case 8:
		   	case 9: printf("Unknown I/O; skipped\n"); break;
		}
		break;
	default: printf("Bad opcode!? %d\n", opcode);
	}
}

// Execute the halt instruction (make CPU stop running)
void exec_HLT(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("HALT\nHalting\n");
	running = 0;
}

// Load register[R] with value at memory[MM]
void exec_LD(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("LD   R%d <- M[%02d] = %d\n",reg_R,addr_MM,mem[addr_MM]);
	reg[reg_R] = mem[addr_MM];
}

// Copy register[R] to memory[MM]
void exec_ST(int reg_R, int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("ST   M[%02d] <- R%d = %d\n",addr_MM,reg_R,reg[reg_R]);
	mem[addr_MM] = reg[reg_R];
} 

// Add contents of MM to Reg[R]
void exec_ADD(int reg_R,int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("ADD  R%d <- R%d + M[%02d] = %d + %d = %d\n", reg_R, reg_R, addr_MM, reg[reg_R], reg[reg_R], 2*reg[reg_R]);
	reg[reg_R] += mem[addr_MM];
}

// Set Reg[R] to opposite
void exec_NEG(int reg_R, int reg[], int nreg) {
	printf("NEG  R%d <- -R%d = -%d\n",reg_R,reg_R,reg[reg_R]);
	reg[reg_R] *= -1;
}

// Load Reg[R] with MM
void exec_LDM(int reg_R,int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen) {
	printf("LDM  R%d <- %d\n", reg_R, instr_sign*addr_MM);
	reg[reg_R] = instr_sign * addr_MM;
} 

// Add MM to Reg[R]
void exec_ADDM(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen) {
	printf("ADDM R%d <- R%d + %d = %d + %d = %d\n", reg_R, reg_R, instr_sign*addr_MM, reg[reg_R], instr_sign*addr_MM, reg[reg_R] + (instr_sign*addr_MM));
	reg[reg_R] += instr_sign * addr_MM;
}

// GoTo MM
void exec_BR(int addr_MM, int mem[], int memlen) {
	printf("BR %02d\n", addr_MM);
	pc = addr_MM;	
}

// if sign of Reg[R] = sign, go to MM
void exec_BRC(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen) {
	char *result;
	int reg_sign = abs(reg[reg_R]) / reg[reg_R];
	if(reg_sign == instr_sign){
		pc = addr_MM;
		result = "Yes";
	} else {
		result = "No";
	}
	printf("BRC %02d if sign R%d(%d) = sign %d: %s\n", addr_MM, reg_R, reg[reg_R], instr_sign, result);
}

// read character and set its ASCII to R0
void exec_GETC(int reg[], int nreg) {
	printf("I/O 0: Read char \n");
	printf("Enter one character (and/or press enter): ");
	reg[0] = getchar();
	printf("R0 <- %d\n",reg[0]);
}

// print character from ASCII in R0
void exec_OUT(int reg[], int nreg) {
	printf("I/O 1: Print char in R0 (=%d): %c\n",reg[0],reg[0]);
}

// print each character of a string at MM, MM+1...until reach 0
void exec_PUTS(int addr_MM, int mem[], int memlen) {
	printf("I/O 2: Print string: aA0\n");
	int ch = reg[addr_MM];
	while (ch != 0) {
		printf("%c\n",ch);
		ch = mem[++addr_MM];
	}
}

// print out control unit 
void exec_DMP(int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("I/O 3: Dump control unit:\n");
	dump_control_unit(pc, ir, running, reg, NREG);
}

// print all mem 
void exec_MEM(int addr_MM, int reg[], int nreg, int mem[], int memlen) {
	printf("I/O 4: Dump memory:\n");
	dump_memory(mem, MEMLEN);
}
