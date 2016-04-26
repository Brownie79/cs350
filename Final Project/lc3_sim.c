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
		Address PC; 		//points to mem addr of next instr
		Word IR;			//holds current instruction
		Word running;		//running=1 iff CPU executing instr
		char cond_code;		//p if last instr was positive, n if it was negative, z if it was 0
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
	void asm_printer(Word val, Address memAddr);
	Word getOffset9(Word val);
	Word getImmediate5(Word val);
	Word getOffset11(Word val);
	Word getOffset6(Word val);
	int read_command(CPU *cpu);
	void execute_instructions(int nbr_cycles, CPU *cpu);
	void parse_exec(Word opcode, CPU *cpu);
//Commands
	void BR(CPU *cpu);
	void ADD(CPU *cpu);
	void LD(CPU *cpu);
	void ST(CPU *cpu);
	void JSR(CPU *cpu);
	void AND(CPU *cpu);
	void LDR(CPU *cpu);
	void STR(CPU *cpu);
	void NOT(CPU *cpu);
	void LDI(CPU *cpu);
	void STI(CPU *cpu);
	void JMP(CPU *cpu);
	void LEA(CPU *cpu);
	void TRAP(CPU *cpu);
	
int main(int argc, char *argv[]){
	printf("LC3 Simulator Final Project pt 2\n");
	printf("Devanshu Bharel\n");
	CPU cpu_value, *cpu = &cpu_value;
	initalize_CPU(cpu);
	initalize_memory(argc,argv, cpu);
	
	char *prompt = ">";
    printf("\nBeginning execution; Hit [ENTER] to continue\n%s", prompt);
    
    int done = read_command(cpu);
    while (!done) { //doesn't do anything since read_command has it's own loop
        done = read_command(cpu);
	  	printf("%s", prompt);
    }
	
	return 0;
}

int read_command(CPU *cpu) {
	// Buffer to read next command line into
	#define CMD_LINE_LEN 80
	char cmd_line[CMD_LINE_LEN];
	char *read_success;     // NULL if reading in a line fails.
	int nbr_cycles;     	// Number of instruction cycles to execute
	char cmd_char;      	// Command 'q', 'h', '?', 'd', or '\n'
	int done = 0;       	// Should simulator stop?
	int words_read;
	char *prompt = ">";

	read_success = fgets(cmd_line, CMD_LINE_LEN, stdin);

	while (read_success != NULL && !done) {
		words_read = sscanf(cmd_line, "%d", &nbr_cycles);
		if (words_read==1){
			if(nbr_cycles > 100)
				nbr_cycles = 100; 
			printf("Executing %d cycles!\n", nbr_cycles);
			execute_instructions(nbr_cycles, cpu);		
		} else {
			words_read = sscanf(cmd_line,"%c",&cmd_char);
			if(cmd_char == '\n'){
				nbr_cycles = 1;
				printf("Executing 1 cycle!\n");
				execute_instructions(nbr_cycles, cpu);	
			} else {
				printf("Command char '%c' read\n",cmd_char);
				switch(cmd_char){
					case '?':
					case 'h':
						printf("h for help\nd for cpu and memory dump\nq to quit\n");
						break;
					case 'd':
						printf("Dumping CPU and Memory!\n");
						dump_cpu(cpu);
						dump_memory(cpu);
						break;
					case 'q':
						printf("Hit 'ENTER' to exit program\n");
						done = 1;
						break;
					default:
						printf("Type h for help~\n");
				}	
			}			
		}
		printf("%s",prompt);	read_success = fgets(cmd_line, CMD_LINE_LEN, stdin);    
    }

    return done;
}

void execute_instructions(int nbr_cycles, CPU *cpu){
	//Word orig = (*cpu).PC //init memory sets PC to orig of code
	Word opcode;
	printf("CPU PC: %x", (*cpu).PC);
	for(int i = 0; i<nbr_cycles; i++){
		if((*cpu).running == 0 ){
			printf("CPU NOT RUNNING!");
			break;
		}
		(*cpu).IR = (*cpu).mem[(*cpu).PC];
		(*cpu).PC = (*cpu).PC + 1; //should wrap around since unsigned int
		opcode = get_bits((*cpu).IR, 15,12);
		printf("\nCycle: %d, Instr: %x \n",i,(*cpu).IR);
		parse_exec(opcode, cpu);
	}
}


void parse_exec(Word opcode, CPU *cpu){
	switch(opcode){
		case 0x0: //BR or NOP 
			BR(cpu);
			break;	
		case 0x1: //ADD or ADD
			ADD(cpu);
			break;
		case 0x2: //LD
			LD(cpu);
			break;
		case 0x3: //ST
			ST(cpu);
			break;
		case 0x4: //JSR or JSRR
			JSR(cpu);
			break;
		case 0x5: //AND or AND
			AND(cpu);
			break;		
		case 0x6: //LDR 
			LDR(cpu);
			break;
		case 0x7: //STR
			STR(cpu);
			break;
		case 0x8: //RTI
			printf("Not Implemented\n");
			break;
		case 0x9: //NOT
			NOT(cpu);
			break;
		case 0xA: //LDI
			LDI(cpu);
			break;
		case 0xB: //STI
			STI(cpu);
			break;
		case 0xC: //JMP
			JMP(cpu);
			break;
		case 0xD: //err
			printf("Unused Opcode\n");
			break;
		case 0xE: //LEA
			LEA(cpu);
			break;
		case 0xF: //TRAP
			TRAP(cpu); 
			break;
		}	
}

void initalize_CPU(CPU *cpu){
	(*cpu).IR = 0;
	(*cpu).PC = 0;
	(*cpu).running=1;
	for(int i=0; i<NREG; i++){
		(*cpu).reg[i]=0;
	}
	(*cpu).cond_code = 'z';
	
	//printf("\n Inital CPU:\n");
	//dump_cpu(cpu);
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
				orig = value_read;
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
		
	//dump_memory(cpu);
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

void asm_printer(Word val, Address memAddr){
	Word op_code = get_bits(val,15,12);
	Word n,z,p = 0;
	//weird bug that won't let me redefine vars in cases
	Word dstReg,srcReg,offset,immediateVal, src1, src2,immediate5,baseR,vector;
	
	switch(op_code){
		case 0x0: //BR or NOP //done
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
		case 0x1: //ADD or ADD //done
			printf("ADD ");
			dstReg = get_bits(val,11,9);
			srcReg = get_bits(val,8,6);
			printf("R%d, R%d, ", dstReg, srcReg);
			if(get_nBit(val,5) == 1){
				immediateVal = getImmediate5(val);
				printf("%d", immediateVal);
			} else {
				printf("R%d", get_bits(val,2,0));
			}
			
			break;
		case 0x2: //LD //done
			printf("LD ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);
			break;
		case 0x3: //ST //done
			printf("ST ");
			srcReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", srcReg, offset);			
			break;
		case 0x4: //JSR or JSRR //done...
			if(get_nBit(val,11) == 1){ //JSR 1 PCoffset11
				Word offset = getOffset11(val);
				Address newAddr = memAddr + offset + 1; //should wrap around if exceeds 65535? //memAddr + 1 to simulate PC since print doesn't have PC values
				printf("JSR M[0x%x]",newAddr);
			} else { //JSRR 000 REG 000000
				dstReg = get_bits(val,8,6);
				printf("JSRR R%d",dstReg);
			}			
			break;
		case 0x5: //AND or AND
			printf("AND ");
			dstReg = get_bits(val, 11,9);
			src1 = get_bits(val, 8,6);
			if(get_nBit(val,5) == 0){
				src2 = get_bits(val, 2, 0);
				printf("R%d, R%d, R%d ", dstReg, src1, src2);
			} else {
				immediate5 = getImmediate5(val);
				printf("R%d, R%d, 0x%hx ", dstReg, src1, immediate5);
			}
			break;		
		case 0x6: //LDR //maybe show new memaddr //done
			printf("LDR ");
			dstReg = get_bits(val,11,9);
			baseR = get_bits(val,8,6);
			offset = getOffset6(val);
			printf("R%d, R%d, %d",dstReg,baseR,offset);
			break;
		case 0x7: //STR //done
			printf("STR ");
			srcReg = get_bits(val,11,9);
			baseR = get_bits(val,8,6);
			offset = getOffset6(val);
			printf("R%d, R%d, %d",srcReg, baseR, offset);
			break;
		case 0x8: //RTI //done
			printf("RTI not to be implemented. ");
			//return from interrupt //don't have to implement
			break;
		case 0x9: //NOT //done
			printf("NOT ");
			dstReg = get_bits(val,11,9);
			srcReg = get_bits(val,8,6);
			printf("R%d, R%d", dstReg, srcReg);
			break;
		case 0xA: //LDI //done
			printf("LDI ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);			
			break;
		case 0xB: //STI //done
			printf("STI ");
			srcReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", srcReg, offset);			
			break;
		case 0xC: //JMP //doen
			dstReg = get_bits(val,8,6);
			printf("JMP R%d",dstReg); //R7 is just RET
			break;
		case 0xD: //err //done
			printf("ERROR CODE NOT IN USE");
			break;
		case 0xE: //LEA //done
			printf("LEA ");
			dstReg = get_bits(val,11,9);
			offset = getOffset9(val);
			printf("R%d, %d", dstReg, offset);
			break;
		case 0xF: //TRAP 
			printf("TRAP ");
			vector = get_bits(val,7,0);
			switch(vector){
				case 0x20:
					printf("x20 - GETC - Read character from keyboard into R0");
					break;
				case 0x21:
					printf("x21 - OUT Print character in R0");
					break;
				case 0x22:
					printf("x22 - PUTS Print string of ASCII in chars starting at location pointed to by R0");
					break;
				case 0x23:
					printf("x23 - IN Read character, but print prompt first");
					break;
				case 0x25:
					printf("x25 - HALT Halts execution");
					break;
				default:
					printf("Trap Vector not found");
			}
			break;
		default:
			printf("Not A Valid Fuction ");
	}
}

Word getOffset11(Word val){
	Word mask = 0x7FF;
	Word num = val & mask;
	if(get_nBit(val,10) == 1) { 
		//negate, add 1
		num = ~num + 1;
		num = num & mask;
		num = num * -1;
	}
	return num;
}

Word getOffset9(Word val){
	Word mask = 0x1FF;
	Word num = val & mask; 
	if(get_nBit(val,8) == 1) { 
		//negate, add 1
		num = ~num + 1;
		num = num & mask;
		num = num * -1;
	}
	return num;	
}

Word getOffset6(Word val){
	Word mask = 0x3F;
	Word num = val & mask; 
	if(get_nBit(val,5) == 1) { 
		//negate, add 1
		num = ~num + 1;
		num = num & mask;
		num = num * -1;
	}
	return num;	
}

Word getImmediate5(Word val){
	Word mask = 0x1F;
	Word num = val & mask;
	//val will have 5 digits, 5th one being the negative identified
	if(get_nBit(val,4) == 1) { //check if 8th bit (9th digit) is 1, if so, num is negative
		//negate, add 1
		num = ~num + 1;
		num = num & mask;
		num = num * -1;
	}
	return num;	
}

void dump_memory(CPU *cpu){
	for(Address i=0; i<=MEMLEN; i++){
		if((*cpu).mem[i] != 0){
			Word val = (*cpu).mem[i];
			printf("@ %2d (%x) Value: %d\t%hx ASM: ", i, i, val,val);
			asm_printer(val, i);
			printf("\n");
		}
		if(i == MEMLEN){ break; }
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

//commands
void BR(CPU *cpu){
	Word instr = (*cpu).IR;
	Word n = get_nBit(instr,11);
	Word z = get_nBit(instr,10);
	Word p = get_nBit(instr,9);
	Word offset = 0;
	
	//print trace	
	if(n+z+p == 0){
		//printf("NOP "); //never branch, don't print
		return;
	}
	printf("BR");	
	if(n == 1)
		printf("N");
	if(p == 1)
		printf("Z");
	if(z == 1)
		printf("P");
	printf(" "); //BRNZ_ (space where _ is)
	
	
	char c = (*cpu).cond_code; 
	if(c == 'n' && n == 1){
		offset = getOffset9(instr);		
	} else if(c == 'z' && z == 1) {
		offset = getOffset9(instr);
	} else if(c == 'p' && p == 1){
		offset = getOffset9(instr);
	} 
	
	Address pcoffset = (*cpu).PC + offset;
	printf("%d", pcoffset); //branch to PC if offset = 0 (cond codes not met)
	
	 
	(*cpu).PC = pcoffset;
}

void ADD(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("ADD ");
	Word dstReg = get_bits(instr,11,9);
	Word srcReg = get_bits(instr,8,6);
	printf("R%d, R%d (%d), ", dstReg, srcReg,(*cpu).reg[srcReg]);
	if(get_nBit(instr,5) == 1){
		Word immediateVal = getImmediate5(instr);
		printf("%d", immediateVal);
		(*cpu).reg[dstReg] = (*cpu).reg[srcReg] + immediateVal;
	} else {
		Word src2 = get_bits(instr,2,0);
		printf("R%d (%d)", src2,(*cpu).reg[src2]);
		(*cpu).reg[dstReg] = (*cpu).reg[srcReg] + (*cpu).reg[src2];
	}
	printf("\n");
	
	//set cond code after doing the add
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}
}

void LD(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("LD ");
	Word dstReg = get_bits(instr,11,9);
	Address offset = (*cpu).PC + getOffset9(instr);
	
	(*cpu).reg[dstReg] = (*cpu).mem[offset];	
	printf("R%d, M[%d](%hx = %d)\n", dstReg, offset, (*cpu).mem[offset], (*cpu).mem[offset]); 
	
	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}	
}

void ST(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("ST ");
	Word srcReg = get_bits(instr,11,9);
	Word offset = getOffset9(instr);
	Address pcoffset = (*cpu).PC+offset;
	(*cpu).mem[pcoffset] = (*cpu).reg[srcReg]; 
	printf("M[%x + %hx], R%d (%x)", (*cpu).PC, offset,srcReg, (*cpu).reg[srcReg]);	
}

void JSR(CPU *cpu){
	Word instr = (*cpu).IR;
	(*cpu).reg[7] = (*cpu).PC; //save the current PC in R7
	
	if(get_nBit(instr,11) == 1){ //JSR 1 PCoffset11
		Word offset = getOffset11(instr);
		Address newAddr = (*cpu).PC + offset;
		printf("JSR M[%x + %hx]\n",(*cpu).PC, offset);
		(*cpu).PC = newAddr;
	} else { //JSRR 000 REG 000000
		Word baseR = get_bits(instr,8,6);
		printf( "JSRR R%d (M[%x])\n",baseR, (*cpu).reg[baseR] );
		(*cpu).PC = (*cpu).PC + (*cpu).reg[baseR];
	}
}

void AND(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("AND ");
	Word dstReg = get_bits(instr, 11,9);
	Word src1 = get_bits(instr, 8,6);
	if(get_nBit(instr,5) == 0){
		Word src2 = get_bits(instr, 2, 0);
		printf("R%d, R%d(%x), R%d(%x) ", dstReg, src1,(*cpu).reg[src1],src2,(*cpu).reg[src2]);
		(*cpu).reg[dstReg] = (*cpu).reg[src1] & (*cpu).reg[src2];
	} else {
		Word immediate5 = getImmediate5(instr);
		printf("R%d, R%d (%x), 0x%hx ", dstReg, src1,(*cpu).reg[src1],immediate5);
		(*cpu).reg[dstReg] = (*cpu).reg[src1] & immediate5;
	}
	printf("\n");
	
	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}	
}

void LDR(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("LDR ");
	Word dstReg = get_bits(instr,11,9);
	Word baseR = get_bits(instr,8,6);
	Word offset = getOffset6(instr);
	printf("R%d, M[%x + %hx]\n",dstReg,(*cpu).reg[baseR],offset);
	(*cpu).reg[dstReg] = (*cpu).mem[(*cpu).reg[baseR] + offset];
	
	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}	
}

void STR(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("STR ");
	
	Word srcReg = get_bits(instr,11,9);
	Word baseR = get_bits(instr,8,6);
	Word offset = getOffset6(instr);
	
	printf("M[%x + %hx], R%d\n", (*cpu).reg[baseR], offset,srcReg);
	
	(*cpu).mem[(*cpu).reg[baseR] + offset] = (*cpu).reg[srcReg];	
}

void NOT(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("NOT ");
	Word dstReg = get_bits(instr,11,9);
	Word srcReg = get_bits(instr,8,6);
	printf("R%d, R%d (%x)\n", dstReg, srcReg, (*cpu).reg[srcReg]);	
	(*cpu).reg[dstReg] = ~(*cpu).reg[srcReg];
		
	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}	
}

void LDI(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("LDI ");
	Word dstReg = get_bits(instr,11,9);
	Word offset = getOffset9(instr);
	(*cpu).reg[dstReg] = (*cpu).mem[(*cpu).mem[(*cpu).PC + offset]]; 
	printf("R%d, M[M[%x + %hx]]\n", dstReg, (*cpu).PC,offset);	

	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}	
}

void STI(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("STI ");
	Word srcReg = get_bits(instr,11,9);
	Word offset = getOffset9(instr);
	printf("M[M[%x %hx]], R%d\n", (*cpu).PC, offset, srcReg);
	(*cpu).mem[(*cpu).mem[(*cpu).PC + offset]] = (*cpu).reg[srcReg];		
}

void JMP(CPU *cpu){
	Word instr = (*cpu).IR;
	Word baseR = get_bits(instr,8,6);
	printf("JMP R%d",baseR);
	(*cpu).PC = (*cpu).reg[baseR];
}

void LEA(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("LEA ");
	Word dstReg = get_bits(instr,11,9);
	Word offset = getOffset9(instr);
	printf("R%d, (%x + %hx)\n", dstReg,(*cpu).PC, offset);
	(*cpu).reg[dstReg] = (*cpu).PC + offset;
	
	//set cond code
	if((*cpu).reg[dstReg] < 0){
		(*cpu).cond_code = 'n';
	} else if((*cpu).reg[dstReg] == 0){
		(*cpu).cond_code = 'z';
	} else if((*cpu).reg[dstReg] > 0){
		(*cpu).cond_code = 'p';
	}
}

void TRAP(CPU *cpu){
	Word instr = (*cpu).IR;
	printf("TRAP ");
	Word vector = get_bits(instr,7,0);
	char c,out;
	Word read,addr;
	
	switch(vector){
		case 0x20:
			printf("x20 - GETC - Read character from keyboard into R0\n");
			scanf("  %c", &c);
			(*cpu).reg[0] = c;		
			break;
		case 0x21:
			printf("x21 - OUT Print character in R0\n");
			out = get_bits((*cpu).reg[0],7,0);
			printf("%c",out);
			break;
		case 0x22:
			printf("x22 - PUTS Print string of ASCII in chars starting at location pointed to by R0(%x)\n",(*cpu).reg[0]);
			addr = (*cpu).reg[0];
			read = (*cpu).mem[addr];
			while(read != 0){
				printf("%c",read);
				addr = addr + 1;
				read = (*cpu).mem[addr]; 	
			}			
			break;
		case 0x23:
			printf("x23 - IN Read character, but print prompt first\n");
			printf(">");
			scanf("  %c",&c);
			(*cpu).reg[0] = c;
			break;
		case 0x25:
			printf("x25 - HALT Halts execution");
			(*cpu).running = 0;
			break;
		default:
			printf("Trap Vector not found");
	}
	printf("/n");
}
