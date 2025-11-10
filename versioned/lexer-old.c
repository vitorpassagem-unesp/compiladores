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
int isID(FILE *tape)
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
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			token = FLT;
			i++;
			while (isdigit(lexeme[i] = getc(tape))) i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
	} else if ( (lexeme[0] = getc(tape)) == '.' ) {
		if ( isdigit(lexeme[1] = getc(tape)) ) {
			int i = 2;
			while (isdigit(lexeme[i] = getc(tape))) i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			token = FLT;
		} else {
			ungetc(lexeme[1], tape);
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			return 0;
		}
	}

	// if (isEE(tape)) {
	// 	blah blah blah
	// }
	
	return token;

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

	token = getc(source);

	return token;
}

