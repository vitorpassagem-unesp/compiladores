#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <main.h>

FILE *source;

extern double acc;

int main(void)
{
	lookahead = gettoken(source = stdin);

	/*
	if (isascii(token)) {
		printf("%c, ASCII(%u)\n", token, token);
	} else if (token == -1) {
		printf("EOF found\n");
	} else {
		printf("%u: %s\n", token, lexeme);
	}
	return 0;
	*/
	
	// procedimento void E(void) emula o símbolo inicial da gramática LL(1)
	// de expressões simplificadas
	mybc();

	printf("%lg\n", acc);

	return 0;
}
