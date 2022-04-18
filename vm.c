/*
	You can use these two print statements for the errors:
		printf("Virtual Machine Error: Stack Overflow Error\n");
		printf("Virtual Machine Error: Out of Bounds Access Error\n");
*/

/*Authors : Eli Ferguson, Jason Co, Drake Scott*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define REG_FILE_SIZE 10
#define MAX_STACK_LENGTH 100

/*
To Run in Eustis
use commands:

make
./a.out basicish.txt

*/

int BP = MAX_STACK_LENGTH - 1; // base pointer
int SP = MAX_STACK_LENGTH; // stack pointer
int PC = 0; // program counter
int halt = 0; // Halt Flag
int pF; //Print Flag

int stack[MAX_STACK_LENGTH], RF[REG_FILE_SIZE];

instruction currentCode; //Current Code Instruction

instruction * IR; // instruction register

char * opname;

void print_execution(int line, char * opname, instruction IR, int PC, int BP, int SP, int *stack, int *RF)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t\t", line, opname, IR.r, IR.l, IR.m, PC, SP, BP);
	
	// print register file
	for (i = 0; i < REG_FILE_SIZE; i++)
		printf("%d ", RF[i]);
	printf("\n");
	
	// print stack
	printf("stack:\t");
	for (i = MAX_STACK_LENGTH - 1; i >= SP; i--)
		printf("%d ", stack[i]);
	printf("\n");
}

int base(int L, int BP, int *stack)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = stack[rtn];
		ctr--;
	}
	return rtn;
}

void fetchCycle();
void executeCycle();
void writeToFile();

void execute_program(instruction *code, int printFlag)
{
	pF = printFlag;
	
	// keep this
	if (pF)
	{
		printf("\t\t\t\t\tPC\tSP\tBP\n");
		printf("Initial values:\t\t\t\t%d\t%d\t%d\n", PC, SP, BP);
	}

	// line :: op name :: r :: l :: m :: pc :: sp :: bp :: stack
	// stack : x x x x x x x x

	for (int i = 0; !halt; i++ ) {
		// Fetch
		fetchCycle(code);
		// Execute
		int tmpPC = PC;
		executeCycle();
		// Print
		if ( pF ) { print_execution(tmpPC-1, opname, code[tmpPC-1], PC, BP, SP, stack, RF); }
	}
}

void fetchCycle(instruction * code)
{
	//Increment the Current Instruction Register
	IR = &code[PC];
	PC++;

	return;
}

void executeCycle()
{	
	switch (IR->opcode) {
		case 1: // LIT R 0 M
			RF[IR->r] = IR->m;
			opname = "LIT";
			break;

		case 2: // RET 0 0 0
			SP = BP + 1;
			PC = stack[SP - 3];
			BP = stack[SP - 2];
			opname = "RET";
			break;

		case 3: // LOD R L M
			if (base(IR->l, BP, stack) - RF[IR->m] < 0 || base(IR->l, BP, stack) - RF[IR->m] >= MAX_STACK_LENGTH)
			{
				printf("Virtual Machine Error: Out of Bounds Access Error\n");
				halt = 1;
				break;
			}
			opname = "LOD";
			RF[IR->r] = stack[base(IR->l, BP, stack) - RF[IR->m]];
			break;

		case 4: // STO R L M
			if (base(IR->l, BP, stack) - RF[IR->m] < 0 || base(IR->l, BP, stack) - RF[IR->m] >= MAX_STACK_LENGTH)
			{
				printf("Virtual Machine Error: Out of Bounds Access Error\n");
				halt = 1;
				break;
			}
			opname = "STO";
			stack[base(IR->l, BP, stack) - RF[IR->m]] = RF[IR->r];
			break;

		case 5: // CAL 0 L M
			opname = "CAL";
			stack[SP - 1] = base(IR->l, BP, stack);
			stack[SP - 2] = BP;
			stack[SP - 3] = PC;
			BP = SP - 1;
			PC = IR->m;
			break;

		case 6: // INC 0 0 M
            SP -= IR->m;
            if(SP < 0){
                printf("Virtual Machine Error: Stack Overflow Error\n");
                halt = 1;
                break;
            }
			opname = "INC";
			break;

		case 7: // JMP 0 0 M
			opname = "JMP";
            PC = IR->m;
			break;

		case 8: // JPC R 0 M
			opname = "JPC";
            if(RF[IR->r] == 0){
                PC = IR->m;
            }
			break;

		case 9: // WRT R 0 0
			opname = "WRT";
            printf("Write Value: %d\n", RF[IR->r]);
			break;

		case 10: // RED R 0 0
            opname = "RED";
            
			if (pF) { printf("Please Enter a Value:\n"); }

			scanf("%d", &RF[IR->r]);

			break;

		case 11: // HAL 0 0 0
            opname = "HLT";
            halt=1;
			break;

		case 12: // NEG R 0 M
           	opname = "NEG";
			RF[IR->r] = -1 * RF[IR->r];
			break;
			
		case 13: // ADD R L M
			opname = "ADD";
			RF[IR->r] = RF[IR->l] + RF[IR->m];
			break;

		case 14: // SUB R L M
			opname = "SUB";
			RF[IR->r] = RF[IR->l] - RF[IR->m];
			break;

		case 15: // MUL R L M
			opname = "MUL";
			RF[IR->r] = RF[IR->l] * RF[IR->m];
			break;

		case 16: // DIV R L M
			opname = "DIV";
			RF[IR->r] = RF[IR->l] / RF[IR->m];
			break;

		case 17: // MOD R L M
			opname = "MOD";
			RF[IR->r] = RF[IR->l] % RF[IR->m];
			break;

		case 18: // EQL R L M
			opname = "EQL";
			RF[IR->r] = 1 ? RF[IR->l] == RF[IR->m] : 0;
			break;

		case 19: // NEQ R L M
			opname = "NEQ";
			RF[IR->r] = 1 ? RF[IR->l] != RF[IR->m] : 0;
			break;

		case 20: // LSS R L M
			opname = "LSS";
			RF[IR->r] = 1 ? RF[IR->l] < RF[IR->m] : 0;
			break;

		case 21: // LEQ R L M
			opname = "LEQ";
			RF[IR->r] = 1 ? RF[IR->l] <= RF[IR->m] : 0;
			break;

		case 22: // GTR R L M
			opname = "GTR";
			RF[IR->r] = 1 ? RF[IR->l] > RF[IR->m] : 0;
			break;

		case 23: // GEQ R L M
			opname = "GEQ";
			RF[IR->r] = 1 ? RF[IR->l] >= RF[IR->m] : 0;
			break;

		default:
			break;
	}

	return;
}