/*********************************************
				Eli Ferguson
				Drake Scott
				Jason Co
*********************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme * list;
int lex_index;

int alphatoken();
int numbertoken();
int symboltoken();
int comment();
int reservedcheck(char *buffer);
int symbolCheck( char buffer );
int whileDigit(char * input, int i);

void printlexerror(int type);
void printtokens();

void printOurs( lexeme in, int t )
{
	if( t == 0 )
		printf("%d\t%d\n", in.type, in.value);
	if(t == 1)
		printf("%d\t%s\n", in.type, in.name);
	if( t == 2 )
		printf("%d\t%d\n", in.type, in.type);
}

lexeme * lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;

	int listPos = 0;

	for ( int i = 0; input[i] != '\0'; i++)
	{
		
		// If Comment Skip
		if ( ( input[i] == '/' ) && ( input[i+1] == '/' ) )
		{
			while( input[i] != '\n' && input[i] != '\0' )
			{
				i++;
			}
		}
		
		// If Digit found convert to int
		if ( isdigit( input[i] ) )
		{
			int endPos = whileDigit(input, i);

			int numLen = endPos - i;

			char number[6];

			for (int idx = 0; idx < numLen; idx++)
			{
				number[ idx ] = input[ i ];
				i++;
			}

			int value = atoi( number );

			list[listPos].type = numbersym;
			list[listPos].value = value;

			//printOurs(list[listPos], 0);
			listPos++;
		}
		
		// If char found convert to identifier or reservered word
		if ( isalpha( input[i] ) )
		{
			int j = i;

			while( isalpha( input[j] ) || isdigit( input[j] ) )
			{
				j++;
			}

			int len = j - i;

			if( len > 12 )
			{
				printlexerror(3);
				exit(0);
			}

			for (int idx = 0; idx < len; idx++)
			{
				list[listPos].name[idx] = input[i];
				i++;
			}

			int rc = reservedcheck( list[listPos].name );

			if( rc != -1 ) // Reserved Words
			{
				list[listPos].type = rc;
			}
			else // Not a Reserved Word
			{
				list[listPos].type = identsym;
			}

			//printOurs(list[listPos], 1);
			listPos++;
		}
		
		// If valid symbol
		if ( symbolCheck( input[i] ) >= 0 )
		{
			int sym = -1;

			switch ( input[i] ) {						

				case '<' :
					if( input[i+1] == '='){
						sym = leqsym;
						i++;
					}
					else if(input[i+1] == '>') { 
						sym = neqsym; 
						i++; 
					}
					else { sym = lsssym; }
					break;

				case '>' :
					if( input[i+1] == '=') { sym = geqsym; i++; }
					else { sym = gtrsym; }
					break;

				case ':' :
					if( input[i+1] == '=') { sym = assignsym; i++; }
					else { sym = colonsym; }
					break;

				case '=' :
					if( input[i+1] == '=') { sym = eqlsym; i++; }
					break;
				
				default :
					sym = symbolCheck( input[i] );
			};
			list[ listPos ].type = sym;

			//printOurs(list[listPos], 2);
			listPos++;
		}

		// If Period found return list
		if ( list[listPos - 1].type == periodsym)
		{				
			//if (printFlag)
			if (1)
			{
				lex_index = listPos;
				//printtokens();
			}

			return list;
		}

		// if not anthying else -> check for invalid symbol
		if ( !isspace( input[i] ) && !iscntrl( input[i] )  && !isalnum( input[i] ) )
		{
			if ( symbolCheck( input[i] ) == -1 )
			{
				printlexerror( 4 );
				exit(0);
			}
		}

	}

	// File ends without a period
	// printf( "Failed To Find Period Token\n" );
	return list;
}

// Add chars to string while the char is a digit
// Error out if alpha found
int whileDigit(char * input, int i)
{

	for(int j = 0; j <= 6; j++)
	{
		if(j == 6)
		{
			printlexerror(2);
			exit(0);
		}
		else if( isdigit( input[i] ) )
		{
			i++;
		}
		else if( isalpha( input[i] ) )
		{
			printlexerror(1);
			exit(0);
		}
		else
		{
			break;
		}
	}

	return i;
}

// Check if word is a reserved word
int reservedcheck(char * buffer)
{
	char* reservedList[10] = {"var", "procedure", "call", "begin", "end", "if", "do", "while", "read", "write" };
	int reservedType[10] = {varsym, procsym, callsym, beginsym, endsym, ifsym, dosym, whilesym, readsym, writesym};

	for(int i = 0; i < 10; i++)
	{
		if( strcmp( buffer, reservedList[i] ) == 0 )
		{
			return reservedType[i];
		}
	}

	return -1;
}

// Check if character is a valid symbol
int symbolCheck( char buffer )
{
	char symbolList[17] = {'.', '=', '[', ']', ',', ';', ':', '?', '(', ')', '<', '>', '+', '-', '*', '/', '%'};
	int symbolType[17] = { periodsym, eqlsym, lbracketsym, rbracketsym, commasym, semicolonsym, colonsym, questionsym, lparenthesissym, rparenthesissym, lsssym, gtrsym, addsym, subsym, multsym, divsym, modsym };

	for(int i = 0; i < 17; i++)
	{
		if( buffer == symbolList[i] )
		{
			return symbolType[i];
		}
	}

	return -1;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case lbracketsym:
				printf("%11s\t%d", "[", lbracketsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case rbracketsym:
				printf("%11s\t%d", "]", rbracketsym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case questionsym:
				printf("%11s\t%d", "?", questionsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case colonsym:
				printf("%11s\t%d", ":", colonsym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case lparenthesissym:
				printf("%11s\t%d", "(", lparenthesissym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case rparenthesissym:
				printf("%11s\t%d", ")", rparenthesissym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			default:
				printf("%11s\t%s", "err", "err");
				break;
		}
		printf("\n");
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}