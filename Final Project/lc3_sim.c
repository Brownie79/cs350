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
//phase2
	Word get_nBit(Word val, int nthBit);
	void printBinary(Word val);
	Word get_bits(Word val, Word leftBit, Word rightBit);
	void asm_printer(Word val);
	Word getOffset9(Word val);
	Word getImmediate5(Word val);

int main(int argc, char *argv[]){
	printf("LC3 Simulator Final Project pt 2\n");
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

Word get_nBit(Word val, int nthBit){
	Word temp = val >> nthBit;
	temp = temp & 0x0001;
	return temp;
}

Word get_bits(Word val, Word leftBit, Word rightBit){
	//printf("\nWord: %x, LeftBit: %d, RightBit: %d\n", val,leftBit,rightBit);
	Word leftPad = 15 - leftBit;
	Word rightPad = leftPad + rightBit;
	//printf("LeftPad: %d RightPad: %d\n", leftPad,rightPad);
	//printf("Val   : "); printBinary(val);  printf("\n");
	Word temp = val << leftPad;
	//printf("Temp L: "); printBinary(temp); printf("\n");	
	temp = temp >> rightPad;
	//printf("Temp R: "); printBinary(temp); printf("\n");	
	if(temp < 0){
		temp = temp*-1; //fix for when right shift fills with 1's instead of 0s
	}
	return temp; //no need for mask cause the entire value is only the bits we want	
}

void printBinary(Word val){
	for(int i = 15; i >= 0; i--){
		printf("%x", get_nBit(val,i));
	}
}

void asm_printer(Word val){
	int op_code = get_bits(val,15,12);
	int srcReg = 0;
	int dstReg = 0;
	int offset = 0;
	int n,z,p = 0;
	
	switch(op_code){
		case 0x0: //BR or NOP
			n = get_nBit(val,11);
			z = get_nBit(val,10);
			p = get_nBit(val,9);	
			if(n+z+p == 0){
				//printf("NOP "); //never branch, don't print
				break;
			}
			printf("BR");	
			if(n == 1)
				printf("N");
			if(p == 1)
				printf("Z");
			if(z == 1)
				printf("P");
			printf(" "); //BRNZ_ (space where _ is)
			
			offset = getOffset9(val);
			printf("%d", offset);									
			break;
		case 0x1: //ADD or ADD
			printf("ADD ");
			dstReg = get_bits(val,11,9);
			srcReg = get_bits(val,8,6);
			printf("R%d, R%d, ", dstReg, srcReg);
			if(get_nBit(val,5) == 1){
				Word immediateVal = getImmediate5(val);
				printf("%d", immediateVal);
			} else {
				printf("R%d", get_bits(val,2,0));
			}
			
			break;
		case 0x2: //LD
			printf("LD ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);
			break;
		case 0x3: //ST
			printf("ST ");
			srcReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", srcReg, offset);			
			break;
		case 0x4: //JSR or JSRR
			break;
		case 0x5: //AND or AND
			printf("AND ");
			break;		
		case 0x6: //LDR
			printf("LDR ");
			break;
		case 0x7: //STR
			printf("STR ");
			break;
		case 0x8: //RTI
			printf("RTI ");
			break;
		case 0x9: //NOT
			printf("NOT ");
			break;
		case 0xA: //LDI
			printf("LDI ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);			
			break;
		case 0xB: //STI
			printf("STI ");
			srcReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", srcReg, offset);			
			break;
		case 0xC: //JMP
			printf("JMP ");
			break;
		case 0xD: //err
			printf("ERROR CODE NOT IN USE");
			break;
		case 0xE: //LEA
			printf("LEA ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);
			break;
		case 0xF: //TRAP
			printf("TRAP ");
			break;
		default:
			printf("Not A Valid Fuction ");
	}
}

Word getOffset9(Word val){
	Word num = val & 0x01FF; //bitstring of 8 bit number //returned as is if positive
	if(get_nBit(val,8) == 1) { //check if 8th bit (9th digit) is 1, if so, num is negative
		//negate, add 1
		num = ~num + 1;
		num = num & 0x01FF;
		num = num * -1;
	}
	return num;	
}

Word getImmediate5(Word val){
	Word num = val & 0x001F;
	//val will have 5 digits, 5th one being the negative identified
	if(get_nBit(val,4) == 1) { //check if 8th bit (9th digit) is 1, if so, num is negative
		//negate, add 1
		num = ~num + 1;
		num = num & 0x000F;
		num = num * -1;
	}
	return num;	
}

void dump_memory(CPU *cpu){
	for(int i=0; i<=MEMLEN; i++){
		if((*cpu).mem[i] != 0){
			Word val = (*cpu).mem[i];
			//printBinary(val); //works!
			//printf("%x \n", get_bits(val,15,12)); // should be the leftmost 4 bits to give op_code
			
			printf("@ %2d (%x) Value: %d\t%hx ASM: ", i, i, val,val);
			asm_printer(val);
			printf("\n");
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


