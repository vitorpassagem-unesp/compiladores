#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>

char lexeme[MAXIDLEN + 1];

/*
 * Esboce um AFN semideterminístico para o analisador léxico abaixo:
 *
 *               ----------------
 *               |              |
 *               v              |
 * -->(isalpha)---->(isalnum)------->((isID))
 *       |                   epsilon
 *       |
 *       -------->((0))
 *        epsilon
 */
int isID(FILE *tape) // OK
{

	if (isalpha(lexeme[0] = getc(tape))) {
		int i = 1;
		while(isalnum(lexeme[i] = getc(tape)))i++;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return ID;
	}

	// emulate epsilon-transition
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// Unix-like hexadecimal numbers
// hexa = 0[xX][0-9A-Fa-f]+
//
int isHEX(FILE *tape)
{
	lexeme[0] = getc(tape);
	lexeme[1] = getc(tape);
	lexeme[2] = getc(tape);
	if (lexeme[0] == '0' &&
	    toupper(lexeme[1]) == 'X' &&
	    isxdigit(lexeme[2])) {
		int i = 3;
		while(isxdigit(lexeme[i] = getc(tape))) i++;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return HEX;
	}
	ungetc(lexeme[2], tape);
	ungetc(lexeme[1], tape);
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}


// Unix-like octal numbers
// oct = 0[0-7]+
//
int isOCT(FILE *tape)
{
	lexeme[0] = getc(tape);
	if (lexeme[0] == '0') {
		lexeme[1] = getc(tape);
		if (lexeme[1] >= '0' && lexeme[1] <= '7') {
			int i = 2;
			while ( (lexeme[i] = getc(tape)) >= '0'
					&& lexeme[i] <= '7') i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			return OCT;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// Unix-like decimal numbers
// num = [1-9][0-9]* | 0
//
int isDEC(FILE *tape)
{
	if (isdigit(lexeme[0] = getc(tape))) {
		if (lexeme[0] == '0') {
			lexeme[1] = 0;
			return DEC;
		}
		int i = 1;
		while(isdigit(lexeme[i] = getc(tape)))i++;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return DEC;
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// fixp = DEC'.'[0-9]* | '.'[0-9]+
// float = fixp EE? | DEC EE
// EE = [eE]['+''-']?[0-9]+
//
int isNUM(FILE *tape)
{
	int token = isDEC(tape);

	if (token == DEC) {

		int i = strlen(lexeme);
		lexeme[i] = getc(tape);
		if (lexeme[i] == '.') {
			i++;
			token = FLT;
			while( isdigit(lexeme[i] = getc(tape)) ) i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
	} else {
		lexeme[0] = getc(tape);
		if ( lexeme[0] == '.' ) {
			if ( isdigit(lexeme[1] = getc(tape)) ) {
				token = FLT;
				int i = 2;
				while( isdigit(lexeme[i] = getc(tape)) ) i++;
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
			} else {
				// backtracking since this is not a number
				ungetc(lexeme[1], tape);
				ungetc(lexeme[0], tape);
				lexeme[0] = 0;
				return 0;// Not a Number
			}
		} else {
			ungetc(lexeme[0], tape);
			lexeme[0] = 0; // lexeme == epsilon
			// This is not a number!!!
			return 0;
		}
	}
	// This is a number!!! YES!

	// if (isEE(tape)) {
	// 	blah blah blah
	// }
	
	return token;
}

int isASGN(FILE *tape)
{
	lexeme[0] = getc(tape);
	if (lexeme[0] == ':') {
		lexeme[1] = getc(tape);
		if (lexeme[1] == '=') {
			lexeme[2] = 0;
			return ASGN;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

int lineno = 1;

void skipspaces(FILE *tape) // OK
{
	int head;
	while (isspace(head = getc(tape))) {
		if (head == '\n') {
			lineno++;
		}
	}
	ungetc(head, tape);
}

int gettoken(FILE *source)
{
	int token;

	skipspaces(source);

	if ( (token = isID(source)) ) return token;
	if ( (token = isOCT(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;
	if ( (token = isASGN(source)) ) return token;

	lexeme[0] = token = getc(source);
	lexeme[1] = 0;

	return token;
}

