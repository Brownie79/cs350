// CS 350, Spring 2016
// Lab 6: SDC Simulator, part 1
//
// Illinois Institute of Technology, (c) 2016, James Sasaki
// 2016-02-21: fixed indentation

#include <stdio.h>
#include <stdlib.h>       // For error exit()

// CPU Declarations -- a CPU is a structure with fields for the
// different parts of the CPU.
//
    typedef short int Word;          // type that represents a word of SDC memory
    typedef unsigned char Address;   // type that represents an SDC address

    #define MEMLEN 100
    #define NREG 10

    typedef struct {
        Word mem[MEMLEN];
        Word reg[NREG];      // Note: "register" is a reserved word
        Address pc;          // Program Counter
        int running;         // running = 1 iff CPU is executing instructions
        Word ir;             // Instruction Register
        int instr_sign;      //   sign of instruction
        int opcode;          //   opcode field
        int reg_R;           //   register field
        int addr_MM;         //   memory field
    } CPU;


// Prototypes [note the functions are also declared in this order]
//
    int main(int argc, char *argv[]);
    void initialize_CPU(CPU *cpu);
    void initialize_memory(int argc, char *argv[], CPU *cpu);
    FILE *get_datafile(int argc, char *argv[]);

    void dump_CPU(CPU *cpu);
    void dump_memory(CPU *cpu);
    void dump_registers(CPU *cpu);

// Main program: Initialize the cpu, and read the initial memory values
//
int main(int argc, char *argv[]) {
    printf("SDC Simulator pt 1 Devanshu Bharel: CS 350 Lab 6\n");
    CPU cpu_value, *cpu = &cpu_value;
    printf("Initalizing CPU:\n");
    initialize_CPU(cpu);
    printf("Initalizing MEM:\n");
    initialize_memory(argc, argv, cpu);

    dump_CPU(cpu);
    dump_memory(cpu);
    dump_registers(cpu);
    // That's it for Lab 6
    //
    return 0;
}


// Initialize the CPU (pc, ir, instruction sign, running flag,
// and the general-purpose registers).
//
void initialize_CPU(CPU *cpu) {
    
    // *** STUB ***
    (*cpu).pc = 0;
    (*cpu).ir = 0;
    (*cpu).running = 1;
    for(int i = 0; i<NREG;i++){
        (*cpu).reg[i] =0;
    }
        
    printf("\nInitial CPU:\n");
    dump_CPU(cpu);
    printf("\n");
}

// Read and dump initial values for memory
//
void initialize_memory(int argc, char *argv[], CPU *cpu) {
    FILE *datafile = get_datafile(argc, argv);
    printf("\nBefore While");
    // Buffer to read next line of text into
    #define BUFFER_LEN 80
    char buffer[BUFFER_LEN];
    printf("\nBefore While");
    // Will read the next line (words_read = 1 if it started
    // with a memory value). Will set memory location loc to
    // value_read
    //
    int value_read, words_read, loc = 0, done = 0;

    char *read_success;    // NULL if reading in a line fails.
    read_success = fgets(buffer, BUFFER_LEN, datafile);
    int range = MEMLEN -1;
    
    while (read_success != NULL && !done) {
        // If the line of input begins with an integer, treat
        // it as the memory value to read in.  Ignore junk
        // after the number and ignore blank lines and lines
        // that don't begin with a number.
        //

        words_read = sscanf(buffer, "%d", &value_read);
        
        //printf("\nWords Read: %d",words_read);
        // *** STUB *** set memory value at current location to
        // value_read and increment location.  Exceptions: If
        // loc is out of range, complain and quit the loop. If
        // value_read is outside -9999...9999, then it's a
        // sentinel and we should say so and quit the loop.
        if(words_read == 1){
            if(loc>range){
                printf("\nERROR: Location %d is outside range",value_read);
                done = 1;
            } else if(value_read < -9999 || value_read > 9999) {
                printf("\nSentinel %d at location %d",value_read,loc);
                done = 1;
            } else {
                (*cpu).mem[loc] = value_read;
                loc++;
            }
        }
        
        // Get next line and continue the loop
        //
        // *** STUB ***
        words_read = sscanf(buffer,"%d",&value_read);
    }//end while
    
    // Initialize rest of memory
    //
    while (loc < MEMLEN) {
        cpu -> mem[loc++] = 0;
    }
    dump_memory(cpu);
}

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.sdc.  If file opening fails, complain
// and terminate program execution with an error.
// See linux command man 3 exit for details.
//
FILE *get_datafile(int argc, char *argv[]) {
    char *default_datafile_name = "default.sdc";
    char *datafile_name = NULL;
    //printf("datafile method 2\n");
    // *** STUB *** set datafile name to argv[1] or default
    if(argv[1]){
        datafile_name = argv[1];
    } else {
        datafile_name = default_datafile_name;
    }
    FILE *datafile = fopen(datafile_name, "r");
        
    // *** STUB *** if the open failed, complain and call
    // exit(EXIT_FAILURE); to quit the entire program
    if(!datafile){
        printf("\n ERROR: File %s could not be opened, Quitting! \n",datafile_name);
        exit(1);
    }
    
    printf("Initialize memory from %s\n", datafile_name);
    return datafile;
}

// dump_CPU(CPU *cpu): Print out the control unit
// and general-purpose registers
// 
void dump_CPU(CPU *cpu) {
    // *** STUB ****
    printf("    PC: %02d IR:: %04d Running: %d\n",(*cpu).pc,(*cpu).ir,(*cpu).running);
    dump_registers(cpu);
}

// dump_memory(CPU *cpu): For each memory address that
// contains a non-zero value, print out a line with the
// address, the value as an integer, and the value
// interpreted as an instruction.
//
void dump_memory(CPU *cpu) {
    // *** STUB ****
    int loc = 0;
    for(int r=0; r<10;r++){
        printf("\n");
        for(int c=0;c<11;c++){
            if(c==0){
                printf(" %d0:",r);
            } else {
                printf("%6d",(*cpu).mem[loc]);
                if(c==5){printf("   ");}
                loc++;
            }
        }
    }
    printf("\n");
}


// dump_registers(CPU *cpu): Print register values in two rows of
// five.
//
void dump_registers(CPU *cpu) {
    // *** STUB ****
    for(int j=0;j<NREG;j++){
        if(j==5){
            printf("\n");
        }
        printf(" R%d: %4d   ",j,(*cpu).reg[j]);   
    }
    printf("\n");
}
