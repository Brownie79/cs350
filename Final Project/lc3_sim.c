#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Devanshu Bharel CS350

//CPU Declarations
	typedef short int Word;
	typedef unsigned short int Address;
	
	#define MEMLEN 65535 //size of memory
	#define NREG 8		 //number of registers, 0-7
	
	typedef struct {
		Word mem[MEMLEN];	//Memory Array
		Word reg[NREG];		//registers array
		Address PC; 		//program counter
		Word IR;			//Instruction registers
		int running;		//running=1 iff CPU executing instr
		int instr_sign;		//sign of the instruction
		int opcode;			
		int reg_R;
		int addr_MM;
		char cond_code;
	} CPU;
	
//Main Program	
	int main(int argc, char *argv[]);
	void initalize_CPU(CPU *cpu);
	void initalize_memory(int argc, char *argv[], CPU *cpu);
	FILE *get_datafile(int argc, char *argv[]);
//Dumps
	void dump_cpu(CPU *cpu);
	void dump_memory(CPU *cpu);
	void dump_registers(CPU *cpu);
	void get_asm(Word val);
	const char *byte_to_binary(int x);

int main(int argc, char *argv[]){
	printf("LC3 Simulator Final Project pt 1\n");
	printf("Devanshu Bharel\n");
	
	CPU cpu_value, *cpu = &cpu_value;
	initalize_CPU(cpu);
	
	initalize_memory(argc,argv, cpu);
	
	return 0;
}

void initalize_CPU(CPU *cpu){
	(*cpu).IR = 0;
	(*cpu).PC = 0;
	(*cpu).instr_sign=1; //1 = positive, -1 = negative
	(*cpu).running=1;
	for(int i=0; i<NREG; i++){
		(*cpu).reg[i]=0;
	}
	(*cpu).reg_R=0;
	(*cpu).addr_MM=0;
	(*cpu).cond_code = 'Z';
	
	printf("\n Inital CPU:\n");
	dump_cpu(cpu);
	printf("\n");
}

void initalize_memory(int argc, char *argv[], CPU *cpu){
	//initalize memory with 0s first
	for(int i=0; i<=MEMLEN; i++){
		(*cpu).mem[i] = 0;
	}

	FILE *datafile = get_datafile(argc, argv);
	
	#define BUFFER_LEN 80
	char buffer[BUFFER_LEN];
	int value_read, words_read, loc=-1, done=0, orig=0;
			
	char *read_success; 
	read_success = fgets(buffer, BUFFER_LEN, datafile);
	while(read_success != NULL && !done){
		words_read = sscanf(buffer, "%x", &value_read);
		if(words_read==1){
			if(loc == -1){
				loc = value_read; //first value determines where to start placing code
			} else if(loc == 65535) { //wrap around if you hit xFFFF
				loc = 0;
				(*cpu).mem[loc]=value_read;
				loc++;		
			} else {
				(*cpu).mem[loc]=value_read;
				loc++;
			}
		}
		read_success = fgets(buffer, BUFFER_LEN, datafile);
	}//end while
	(*cpu).PC = orig;
		
	dump_memory(cpu);
}

FILE *get_datafile(int argc,char *argv[]){
	char *default_datafile_name = "default.hex";
	char *datafile_name = NULL;
	
	if(argv[1]){ datafile_name = argv[1]; }
	else{ datafile_name = default_datafile_name;}
	
	FILE *datafile = fopen(datafile_name, "r");
	
	if(!datafile){
		printf("File could not be opened!");
		exit(EXIT_FAILURE);
	}
	
	return datafile;
}

void dump_memory(CPU *cpu){
	for(int i=0; i<=MEMLEN; i++){
		if((*cpu).mem[i] != 0){
			Word val = (*cpu).mem[i];
			get_asm(val);
			printf("@ %2d (%x) Value: %d\t%x ASM: %s \n", i, i, val,val,"\tASM_Under_Construction");
		}
	}
}

void dump_cpu(CPU *cpu){
	printf("PC: %02d  IR: %04d  Condition Code: %c  Running: %d\n",(*cpu).PC, (*cpu).IR, (*cpu).cond_code, (*cpu).running);	
	dump_registers(cpu);
}

void dump_registers(CPU *cpu){
	for(int i=0;i<5;i++){
      printf(" R%d: %4d ",i,(*cpu).reg[i]);
    }
    printf("\n");
    for (int i=5;i<NREG;i++){
      printf(" R%d: %4d ",i,(*cpu).reg[i]);
    }
    printf("\n");
}


void get_asm(Word val){
	//char *p; //pointer to string that holds asm instruction
	char a[] = "%X",val;
	printf("asm: %s\n", a);
}

const char *byte_to_binary(int x){
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1){
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
