#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 200
#define MAX_SYMBOL_COUNT 50
#define MAX_REG_COUNT 10

// generated code
instruction *code;
int cIndex;

// symbol table
symbol *table;
int tIndex;

void emit(int opname, int reg, int level, int mvalue);
void addToSymbolTable(int k, char n[], int s, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

//* Global Variables
int registerCounter = -1;
int code, codeIdx, table, tableIdx, listIdx, level, registerCounter;

lexeme * list;

void block();
int var_declaration();
void procedure_declaration(), statement(), condition(), expression(), term(), factor();


instruction *parse(lexeme *list, int printTable, int printCode)
{
	list = list;

	// set up program variables
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	cIndex = 0;
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0;

	//* Our Code Start

	//*Jump to start
	emit(7, 0, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;
	block();

	//! Check For Period at end, must end in period

	//*add a hlt to the program
	emit(11, 0, 0, 0);

	code[0].m = table[0].addr;

	for( int i = 0; i < codeIdx; i++ )
	{
		if( code[i].opcode == 5 )
		{
			code[i].m = table[ code[i].m ].addr;
		}
	}

	//* Our Code End


	// print off table and code
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();

	// mark the end of the code
	code[cIndex].opcode = -1;
	return code;
}

void block()
{
	level++;
	int procedureIdx = tableIdx - 1;
	int x = var_declaration();
	table[ procedureIdx ].addr = codeIdx;
	emit( 6, 0, 0, x );
	statement();
	mark();
	level--;
}

int var_declaration()
{
	int memorysize = 3;
	int symbolname;
	int arraysize;

	if ( list[listIdx].type == varsym )
	{
		do
		{
			listIdx++;

			if ( list[listIdx].type != identsym )
			{
				//! error 2
				printparseerror(2);
			}

			if ( multipledeclarationcheck(list[listIdx].type) != -1 )
			{
				//! error 3
				printparseerror(3);
			}

			symbolname = list[listIdx].name;
			listIdx++;

			if ( list[listIdx].type == lbracketsym )
			{
				listIdx++;

				if ( list[listIdx].type != numbersym || list[listIdx].value == 0 )
				{
					//! error 4
					printparseerror(4);
				}

				arraysize = list[listIdx].value;
				listIdx++;

				if ( list[listIdx].type == multsym, divsym, modsym, addsym, subsym )
				{
					//! error 4
					printparseerror(4);
				}
				else if ( list[listIdx].type != rbracketsym )
				{
					//! error 5
					printparseerror(5);
				}

				listIdx++;
				addtosymboltable(2, symbolname, arraysize, level, memorysize, 0);
				memorysize += arraysize;
			}
			else
			{
				addtosymboltable(1, symbolname, 0, level, memorysize, 0);
				memorysize++;
			}
		} while ( list[listIdx].type == commasym );

		if ( list[listIdx].type == identsym )
		{
			//! error 6
			printparseerror(6);
		}
		else if ( list[listIdx].type != semicolonsym )
		{
			//! error 7
			printparseerror(7);
		}

		listIdx++;

		return memorysize;
	}
	else
	{
		return memorysize;
	}
}

void emit(int opname, int reg, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].r = reg;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void procedure_declaration()
{
	char * symbolName = malloc(sizeof(char) * 12);

	while (list[listIdx].type == procsym)
	{
		listIdx++;
		if (list[listIdx].type != identsym)
        {
			//! error 2
            printparseerror(2);
        }
		else if (multipledeclarationcheck(list[listIdx].name) != -1)
        {
			//! error 3
			printparseerror(3);
        }
		strcpy(symbolName, list[listIdx].name);
		listIdx++;
		if (list[listIdx].type != semicolonsym)
        {
			//! error 8
            printparseerror(8);
        }
		listIdx++;
		addtosymboltable(3, symbolName, 0, level, 0, 0);
		block();
		if (list[listIdx].type != semicolonsym)
        {
			//! error 7
            printparseerror(7);
        }
		listIdx++;
		emit(2, 0, 0, 0); // RET
	}
}

void condition()
{
	expression();
	if ( list[listIdx].type == eqlsym)
    {
		listIdx++;
		expression();
        //* emit EQL
		emit( 18, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
		registerCounter--;
    }
	else if ( list[listIdx].type == neqsym )
    {
		listIdx++;
		expression();
        //* emit NEQ
		emit( 19, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter);
		registerCounter--;
    }
	else if ( list[listIdx].type == lsssym )
    {
		listIdx++;
		expression();
        //* emit LSS
		emit( 20, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
		registerCounter--;
    }
	else if ( list[listIdx].type == leqsym )
    {
		listIdx++;
		expression();
        //* emit LEQ
		emit( 21, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
		registerCounter--;
    }
	else if ( list[listIdx].type == gtrsym )
    {
		listIdx++;
		expression();
        //* GTR
		emit( 22, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter);
		registerCounter--;
    }
	else if ( list[listIdx].type == geqsym )
    {
		listIdx++;
		expression();
        //* emit GEQ
		emit( 23, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
		registerCounter--;
    }
	else
		//! error 21
        printparseerror(21);
}

void expression()
{
    if (list[listIdx].type == subsym)
    {
        listIdx++;
		term();
		emit(12, registerCounter, 0, registerCounter); // NEG
		while (list[listIdx].type == addsym || list[listIdx].type == subsym)
        {
            if (list[listIdx].type == addsym)
            {
                listIdx++;
				term();
				emit(13, registerCounter - 1, registerCounter - 1, registerCounter); // ADD
				registerCounter--;
            }
			else
            {
                listIdx++;
				term();
				emit(14, registerCounter - 1, registerCounter - 1, registerCounter); // SUB
				registerCounter--;
            }
        }
    }
    else
    {
        term();
		while (list[listIdx].type == addsym || list[listIdx].type == subsym)
        {
            if (list[listIdx].type == addsym)
            {
                listIdx++;
				term();
				emit(13, registerCounter - 1, registerCounter - 1, registerCounter); // ADD
				registerCounter--;
            }
			else
            {
                listIdx++;
				term();
				emit(14, registerCounter - 1, registerCounter - 1, registerCounter); // SUB
				registerCounter--;
            }
        }
    }
	if (list[listIdx].type == lparenthesissym || list[listIdx].type == identsym || list[listIdx].type == numbersym)
    {
		//! error 22
		printparseerror(22);
    }
}

void term()
{
	factor();
	while ( list[listIdx].type == multsym, divsym, modsym )
	{
		if ( list[listIdx].type == multsym )
		{
			listIdx++;
			factor();
			//* emit MUL
			emit( 15, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
			registerCounter--;
		}
		else if ( list[listIdx].type == divsym )
		{
			listIdx++;
			factor();
			//* emit DIV
			emit( 16, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
			registerCounter--;
		}
		else
		{
			listIdx++;
			factor();
			//* emit MOD
			emit( 17, ( registerCounter - 1 ), ( registerCounter - 1 ), registerCounter );
			registerCounter--;
		}
	}
}

void factor()
{
	//char symbolName[12];

	char * symbolName = malloc(sizeof(char) * 12);
	int symIdx;
	int arrayIdxReg;
	int varLocReg;

	if ( list[listIdx].type == identsym )
	{
		//* symbolName = list[listIdx].name;

		listIdx++;

		if ( list[listIdx].type == lbracketsym )
		{
			listIdx++;
			symIdx = findsymbol(symbolName, 2);
			if ( symIdx == -1 )
			{
				if ( findsymbol(symbolName, 1) != -1 )
				{
					//! error 11
					printparseerror(11);
				}
				else if ( findsymbol(symbolName, 3) != -1 )
				{
					//! error 9
					printparseerror(9);
				}
				else
				{
					//! error 10
					printparseerror(10);
				}
			}
			expression();
			arrayIdxReg = registerCounter;
			if  ( list[listIdx].type != rbracketsym )
			{
				//! error 5
				printparseerror(5);
			}
			listIdx++;
			registerCounter++;
			if ( registerCounter >= 10 )
			{
				//! error 14
				printparseerror(14);
			}
			//* emit LIT
			emit( 1, registerCounter, 0, table[symIdx].addr );

			//* emit ADD
			emit( 13, arrayIdxReg, arrayIdxReg, registerCounter);
			registerCounter--;

			//* emit LOD
			emit( 3, registerCounter, ( level - table[symIdx].level ), arrayIdxReg );
		}
		else
		{
			symIdx = findsymbol(symbolName, 1);

			if ( symIdx == -1 )
			{
				if ( findsymbol(symbolName, 2) != -1 )
				{
					//! error 12
					printparseerror(12);
				}
				else if ( findsymbol(symbolName, 3) != -1 )
				{
					//! error 9
					printparseerror(9);
				}
				else
				{
					//! error 10
					printparseerror(10);
				}
			}
			registerCounter++;

			if ( registerCounter >= 10 )
			{
				//! error 14
				printparseerror(14);
			}

			//* emit LIT
			emit( 1, registerCounter, 0, table[symIdx].addr );
			varLocReg = registerCounter;

			//* emit LOD
			emit( 3, registerCounter, ( level - table[symIdx].level ), varLocReg );
		}
	}
	else if ( list[listIdx].type == numbersym )
	{
		registerCounter++;
		if ( registerCounter >= 10 )
		{
			//! error 14
			printparseerror(14);
		}
		//* emit LIT
		emit( 1, registerCounter, 0, list[listIdx].value );
		listIdx++;
	}
	else if ( list[listIdx].type == lparenthesissym )
	{
		listIdx++;
		expression();
		if ( list[listIdx].type != rparenthesissym )
		{
			//! error 23
			printparseerror(23);
		}
		listIdx++;
	}
	else
	{
		//! error 24
		printparseerror(24);
	}
}

void addToSymbolTable(int k, char n[], int s, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].size = s;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Symbol names must be identifiers\n");
			break;
		case 3:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 4:
			printf("Parser Error: Array sizes must be given as a single, nonzero number\n");
			break;
		case 5:
			printf("Parser Error: [ must be followed by ]\n");
			break;
		case 6:
			printf("Parser Error: Multiple symbols in var declaration must be separated by commas\n");
			break;
		case 7:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 8:
			printf("Parser Error: Procedure declarations should contain a semicolon before the body of the procedure begins\n");
			break;
		case 9:
			printf("Parser Error: Procedures may not be assigned to, read, or used in arithmetic\n");
			break;
		case 10:
			printf("Parser Error: Undeclared identifier\n");
			break;
		case 11:
			printf("Parser Error: Variables cannot be indexed\n");
			break;
		case 12:
			printf("Parserr Error: Arrays must be indexed\n");
			break;
		case 13:
			printf("Parser Error: Assignment operator missing\n");
			break;
		case 14:
			printf("Parser Error: Register Overflow Error\n");
			break;
		case 15:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 16:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 17:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 18:
			printf("Parser Error: if must be followed by ?\n");
			break;
		case 19:
			printf("Parser Error: do must be followed by while\n");
			break;
		case 20:
			printf("Parser Error: read must be followed by a var or array identifier\n");
			break;
		case 21:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 22:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 23:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 24:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, and variables\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);

	//! Add exit();
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Size | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %4d | %5d | %5d\n", table[i].kind, table[i].name, table[i].size, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tR\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				printf("RET\t");
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("WRT\t");
				break;
			case 10:
				printf("RED\t");
				break;
			case 11:
				printf("HLT\t");
				break;
			case 12:
				printf("NEG\t");
				break;
			case 13:
				printf("ADD\t");
				break;
			case 14:
				printf("SUB\t");
				break;
			case 15:
				printf("MUL\t");
				break;
			case 16:
				printf("DIV\t");
				break;
			case 17:
				printf("MOD\t");
				break;
			case 18:
				printf("EQL\t");
				break;
			case 19:
				printf("NEQ\t");
				break;
			case 20:
				printf("LSS\t");
				break;
			case 21:
				printf("LEQ\t");
				break;
			case 22:
				printf("GTR\t");
				break;
			case 23:
				printf("GEQ\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\t%d\n", code[i].r, code[i].l, code[i].m);
	}

	if (table != NULL)
		free(table);
}
