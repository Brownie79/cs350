#include <stdio.h>
#include <stdlib.h>       
//Devanshu Bharel CS350

// CPU Declarations

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

//Main program and initialization stuff.
    int main(int argc, char *argv[]);
    void initialize_CPU(CPU *cpu);
    void initialize_memory(int argc, char *argv[], CPU *cpu);
    FILE *get_datafile(int argc, char *argv[]);

//Dumps 
    void dump_CPU(CPU *cpu);
    void dump_memory(CPU *cpu);
    void dump_registers(CPU *cpu);

//Commands and related methods. 
    int read_execute_command(CPU *cpu);
    int execute_command(char cmd_char, CPU *cpu);
    void help_message(void);
    void many_instruction_cycles(int nbr_cycles, CPU *cpu);
    void one_instruction_cycle(CPU *cpu);

//SDC Instructions 
    void LD(CPU *cpu);
    void ST(CPU *cpu);
    void ADD(CPU *cpu);
    void NEG(CPU *cpu);
    void LDM(CPU *cpu);
    void ADDM(CPU *cpu);
    void BR(CPU *cpu);
    void BRC(CPU *cpu);
    void GETC(CPU *cpu);
    void OUT(CPU *cpu);
    void PUTS(CPU *cpu);
    void HALT(CPU *cpu);

int main(int argc, char *argv[]) {
    printf("SDC Simulator pt 2: CS 350 Lab 7\n");
    printf("Devanshu Bharel\n");
    
    CPU cpu_value, *cpu = &cpu_value;
    initialize_CPU(cpu);
    initialize_memory(argc, argv, cpu);

    char *prompt = ">";
    printf("\nBeginning execution; Hit [ENTER] to continue\n%s", prompt);
    
    int done = read_execute_command(cpu);
    while (!done) {
        done = read_execute_command(cpu);
	printf("%s", prompt);
    }
    return 0;
}


// Initialize the CPU

void initialize_CPU(CPU *cpu) {
  (*cpu).ir=0;
  (*cpu).pc=0;
  (*cpu).instr_sign=1;
  (*cpu).running=1;
    for (int i=0;i<NREG;i++){
      (*cpu).reg[i]=0;
    }
  (*cpu).reg_R=0;
  (*cpu).addr_MM=0;
    
  printf("\nInitial CPU:\n");
  dump_CPU(cpu);
  printf("\n");
}

// Read and dump initial values for memory
void initialize_memory(int argc, char *argv[], CPU *cpu) {
    FILE *datafile = get_datafile(argc, argv);
    
    #define BUFFER_LEN 80
    char buffer[BUFFER_LEN];

    int value_read, words_read, loc = 0, done = 0;

    char *read_success;    // NULL if reading in a line fails.
    int max=MEMLEN;
    
    read_success = fgets(buffer, BUFFER_LEN, datafile);
    while (read_success != NULL && !done) {
        words_read = sscanf(buffer, "%d", &value_read);
	if (words_read==1){
	    if(loc>max){
	      printf("\nERROR: Memory out of Range!.");
	      done = 1;
	    }
	    else if (value_read < -9999 || value_read > 9999){
	      printf("\nSentinel %d at %d\n",value_read,loc);
	      done = 1;
	    }
	    else{
	      (*cpu).mem[loc]=value_read;
	      loc++;
	    }
	}
	read_success = fgets(buffer, BUFFER_LEN, datafile);
    }
    
    // Initialize rest of memory with zeroes.
    while (loc < MEMLEN) {
        cpu -> mem[loc++] = 0;
    }
    dump_memory(cpu);
}


FILE *get_datafile(int argc, char *argv[]) {
    char *default_datafile_name = "default.sdc";
    char *datafile_name = NULL;

    if(argv[1]){
      datafile_name=argv[1];
    } else {
        datafile_name = default_datafile_name; 
    }
    
    FILE *datafile = fopen(datafile_name, "r");

    if(!datafile){
      printf("Could not get in the file");
      exit(EXIT_FAILURE);
    }

    return datafile;
}


void dump_CPU(CPU *cpu) {
  printf("PC: %02d  IR: %04d  Running: %d\n", (*cpu).pc, (*cpu).ir, (*cpu).running);
  dump_registers(cpu);
}


void dump_memory(CPU *cpu) {
  for (int i=0;i<MEMLEN;i++){
    if((*cpu).mem[i]!=0){
      printf("@ %2d  Value: %d \n ", i, (*cpu).mem[i]);
    }
  }
}


void dump_registers(CPU *cpu) {
    for(int i=0;i<5;i++){
      printf(" R%d: %4d ",i,(*cpu).reg[i]);
    }
    printf("\n");
    for (int i=5;i<NREG;i++){
      printf(" R%d: %4d ",i,(*cpu).reg[i]);
    }
    printf("\n");
}

int read_execute_command(CPU *cpu) {
// Buffer to read next command line into
#define CMD_LINE_LEN 80
    char cmd_line[CMD_LINE_LEN];
    char *read_success;     // NULL if reading in a line fails.
    int nbr_cycles;     // Number of instruction cycles to execute
    char cmd_char;      // Command 'q', 'h', '?', 'd', or '\n'
    int done = 0;       // Should simulator stop?
    int words_read;

    read_success = fgets(cmd_line, CMD_LINE_LEN, stdin);

    while (read_success != NULL && !done) {
        words_read = sscanf(cmd_line, "%d", &nbr_cycles);
	if (words_read==1){
	  many_instruction_cycles(nbr_cycles, cpu);
	} else {
	  words_read = sscanf(cmd_line,"%c",&cmd_char);
	  execute_command(cmd_char, cpu);
	}
    read_success = fgets(cmd_line, CMD_LINE_LEN, stdin);    
    }

    return done;
}

int execute_command(char cmd_char, CPU *cpu) {
  if (cmd_char == '?' || cmd_char == 'h') {
    help_message();
  } else if (cmd_char == 'd') {
    printf("Dumping CPU and MEM:\n");
    dump_CPU(cpu);
    dump_memory(cpu);
  } else if (cmd_char == 'q') {
    printf("Quitting. \n");
    exit(0);
  } else if (cmd_char == '\n' || cmd_char == ' ') {
    one_instruction_cycle(cpu);
  } else {
    printf("Unkown Command: %c \n",cmd_char);
    return 1;
  }
  return 0;
}

// Print standard message for simulator help command ('h' or '?')
//
void help_message(void) {
   char *help = "Commands:"
   "\nEnter h or ? for help)"
   "\nEnter d to dump the cpu"
   "\n\nSDC Instructions:"
   "\n Q: Quit";
   printf("%s",help);
}

// Execute a number of instruction cycles.  Exceptions: If the
// number of cycles is <= 0, complain and return; if the CPU is
// not running, say so and return; if the number of cycles is
// insanely large, complain and substitute a saner limit.
//
// If, as we execute the many cycles, the CPU stops running,
// stop and return.
//
void many_instruction_cycles(int nbr_cycles, CPU *cpu) {
    int MAX_NBR_CYCLES = 100;

    if (nbr_cycles <=0){
      printf("ERROR: The number of cycles is less than 1..\n");
      return;
    }
    else if (!(*cpu).running){
      printf("ERROR: The CPU is not running.\n");
      return;
    }
    else if (nbr_cycles > MAX_NBR_CYCLES) {
      printf("%d is too many cycles. The limit is 100. \n" , nbr_cycles);
      nbr_cycles = 100;
    }

    int i = 1;
    while (i <= nbr_cycles && (*cpu).running){
      one_instruction_cycle(cpu);
      i++;
    }
}

// Execute one instruction cycle
//
void one_instruction_cycle(CPU *cpu) {
    // If the CPU isn't running, say so and return.
    // If the pc is out of range, complain and stop running the CPU.
    //
  if ((*cpu).running==0){
    printf("The CPU is not running. \n");
    return;
  }

    // Get instruction and increment pc
    //
  int instr_loc = (*cpu).pc;  // Instruction's location (pc before increment)
  (*cpu).ir = (*cpu).mem[(*cpu).pc];
  (*cpu).pc++;



  // Making sure the instruction is 
  if ((*cpu).reg_R<0 || (*cpu).reg_R>NREG || (*cpu).addr_MM<0 || (*cpu).addr_MM>100 ) {
    printf("Error. The register or memory address given in the instruction is out of bounds.\n");
    return;
  }

  //Decode opcode into opcode, reg_r, addr_MM, and instruction sign
  (*cpu).opcode = abs((*cpu).mem[instr_loc] / 1000);
  (*cpu).reg_R = abs((*cpu).mem[instr_loc] / 100) % 10;
  (*cpu).addr_MM = abs((*cpu).mem[instr_loc] % 100);
  
  // In case first instruction stored in memory is a zero, we need to give it a sign of 0.     
  if((*cpu).mem[instr_loc] != 0) {
    (*cpu).instr_sign = (*cpu).mem[instr_loc] / abs((*cpu).mem[instr_loc]);
  } else{
    (*cpu).instr_sign=0;
  }


 
  // Echo instruction
  printf("At %02d instr %d %d %02d: ", instr_loc, (*cpu).opcode, (*cpu).reg_R, (*cpu).addr_MM);

    switch ((*cpu).opcode) {
	case 0: HALT(cpu); break;
	case 1: LD(cpu); break;
	case 2: ST(cpu); break;
	case 3: ADD(cpu); break;
	case 4: NEG(cpu); break;
	case 5: LDM(cpu); break;
	case 6: ADDM(cpu); break;
	case 7: BR(cpu); break;
	case 8: BRC(cpu); break;
	case 9:
	  switch ((*cpu).reg_R) {
	      case 0: GETC(cpu); break;
	      case 1: OUT(cpu); break;
	      case 2: PUTS(cpu); break;
	      case 3: dump_CPU(cpu); break;
	      case 4: dump_memory(cpu); break;
	      case 5: printf("Ignored"); break;
	      case 6: printf("Ignored"); break;
	      case 7: printf("Ignored"); break;
	      case 8: printf("Ignored"); break;
	      case 9: printf("Ignored"); break;
	  }
	  break;
        default: printf("Bad opcode!? %d\n", (*cpu).opcode);
    }


}

// Load 
void LD(CPU *cpu) {
  printf("LD R%d <- M[%02d] = %d\n",
  (*cpu).reg_R,
  (*cpu).addr_MM,
  (*cpu).mem[(*cpu).addr_MM]);

  (*cpu).reg[(*cpu).reg_R] = (*cpu).mem[(*cpu).addr_MM];
}

// Store
void ST(CPU *cpu) {
    printf("ST M[%02d] <- R%d = %d\n",
    (*cpu).addr_MM,
    (*cpu).reg_R,
    (*cpu).reg[(*cpu).reg_R]);

    (*cpu).mem[(*cpu).addr_MM] = (*cpu).reg[(*cpu).reg_R];
} 

// Add contents of MM to Reg[R]
void ADD(CPU *cpu) {
    printf("ADD R%d <- R%d + M[%02d] = %d + %d = %d\n",
    (*cpu).reg_R, (*cpu).reg_R,
    (*cpu).addr_MM,
    (*cpu).reg[(*cpu).reg_R],
    (*cpu).mem[(*cpu).addr_MM],
    (*cpu).reg[(*cpu).reg_R] + (*cpu).mem[(*cpu).addr_MM]);
 
    (*cpu).reg[(*cpu).reg_R] += (*cpu).mem[(*cpu).addr_MM];
}

// Set Reg[R] to opposite sign. 
void NEG(CPU *cpu) {
  printf("NEG R%d <- -R%d = -%d\n",
  (*cpu).reg_R,
  (*cpu).reg_R,
  (*cpu).reg[(*cpu).reg_R]);
  
  (*cpu).reg[(*cpu).reg_R] *= -1;
}

// Load Reg[R] with MM
void LDM(CPU *cpu) {
    printf("LDM R%d <- %d\n",
    (*cpu).reg_R,
    (*cpu).instr_sign*(*cpu).addr_MM);
  
    (*cpu).reg[(*cpu).reg_R] = (*cpu).instr_sign * (*cpu).addr_MM;
} 

// Add MM to Reg[R]
void ADDM(CPU *cpu) {
    printf("ADDM R%d <- R%d + %d = %d + %d = %d\n",
    (*cpu).reg_R,
    (*cpu).reg_R,
    (*cpu).instr_sign*(*cpu).addr_MM,
    (*cpu).reg[(*cpu).reg_R],
    (*cpu).instr_sign*(*cpu).addr_MM,
    (*cpu).reg[(*cpu).reg_R] + ((*cpu).instr_sign*(*cpu).addr_MM));
    
    (*cpu).reg[(*cpu).reg_R] += (*cpu).instr_sign * (*cpu).addr_MM;
}

// Branch Unconditional
void BR(CPU *cpu) {
    printf("BR %02d\n", (*cpu).addr_MM);
    (*cpu).pc = (*cpu).addr_MM;	
}

// Branch conditional
void BRC(CPU *cpu) {
  char *result;
  int sign = abs((*cpu).reg[(*cpu).reg_R]) / (*cpu).reg[(*cpu).reg_R];
  if(sign == (*cpu).instr_sign){
    (*cpu).pc = (*cpu).addr_MM;
  }
}

// Reads a character, gets it ASCII value, and stores it in R0
void GETC(CPU *cpu) {
  printf("Enter one character. When done, press [ENTER].");
  (*cpu).reg[0] = getchar();
  printf("R0 <- %d\n",(*cpu).reg[0]);
}

// Prints the ASCII value at R0
void OUT(CPU *cpu) {
    printf("Print char in R0 (=%d): %c\n",(*cpu).reg[0],(*cpu).reg[0]);
}

// Prints a string one character at a time
void PUTS(CPU *cpu) {
    printf("Print string: aA0\n");
    int c = (*cpu).reg[(*cpu).addr_MM];

    while (c != 0) {
        printf("%c\n",c);
        c = (*cpu).mem[++(*cpu).addr_MM];
    }

}

// Halts the CPU
void HALT(CPU *cpu) {
    printf("Halting\n");
    (*cpu).running = 0;
}
