#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>

int lookahead; // O olho do parser, é por este operador que o parser enxerga antecipadamente os tokens
	       // provenientes do lexer

/*
 * E -> [ ominus ] T { oplus T }
 *  input: "a * b"
 *  token: ID '*' ID
 */
void E(void) 
{ 
	// [ ominus ]; ominus = '+'|'-'
	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
	}
_Tbegin:
	T(); 
	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
		goto _Tbegin;
	}
}

/*
 * T -> F { otimes F }
 *  input: "a * b"
 */
void T(void) 
{ 
_Fbegin:
	F(); 
	// { otimes T }; otimes = '*'|'/'
	if (lookahead == '*' || lookahead == '/') {
		match(lookahead);
		goto _Fbegin;
	}
}

/*
 * F -> '(' E ')' | HEX | OCT | DEC | FLT | ID
 */
void F(void)
{
	switch(lookahead) {
		case '(':
			match('('); E(); match(')');
			break;
		case HEX:
			match(HEX); break;
		case OCT:
			match(OCT); break;
		case DEC:
			match(DEC); break;
		case FLT:
			match(FLT); break;
		default:
			match(ID);
	}
}

/*** A principal função (procedimento) interface do parser é match:
-> match vai consumir um tolen da cadeia de entrada, se ele corresponder com a sintaxe
 */
void match(int expected_token)
{
	if (lookahead == expected_token) {
		lookahead = gettoken(source);
	} else {
		fprintf(stderr, "token mismatch at line %d\n", lineno);
		exit(ERRTOKEN);
	}
}
