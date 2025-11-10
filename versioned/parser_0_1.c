#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>

int lookahead; // O olho do parser, é por este operador que o parser enxerga antecipadamente os tokens
	       // provenientes do lexer

/*

	E-> ominus T R | T R
	ominus = ['+''-']

 	Q -> otimes F Q | epsilon
 	otimes = ['*''/']

	R -> OPLUS T R | epsilon
 	OPLUS = ['+''-']

*/


/*
 * E -> T R
 *  input: "a * b"
 *  token: ID '*' ID
 */

void E(void) {

	/**/int ominus = 0;

	if(lookahead == '+' || lookahead == '-'){
		/**/(lookahead == '-') && (ominus = lookahead);/**/	
		match(lookahead);
	}

	T();

	if(ominus){
		printf(" negate ");
		ominus = 0;
	}
	
	R(); 
}

/*
 * T -> F Q
 */
void T(void) {
	F(); 
	Q();
}

/*
 * F -> '(' E ')' | HEX | OCT | DEC | FLT | ID
 */

void F(void){

	switch(lookahead) {
		case '(':
			match('('); E(); match(')');
			break;
		case HEX:
			/**/printf(" %s ", lexeme);/**/
			match(HEX); break;
		case OCT:
			/**/printf(" %s ", lexeme);/**/
			match(OCT); break;
		case DEC:
			/**/printf(" %s ", lexeme);/**/
			match(DEC); break;
		case FLT:
			/**/printf(" %s ", lexeme);/**/
			match(FLT); break;
		default:
			/**/printf(" %s ", lexeme);/**/
			match(ID);
	}	

}

/*Q -> otimes F Q | epsilon*/

void Q(void){

	/**/int otimes = 0;/**/

	if (lookahead == '*' || lookahead == '/') {
		/**/otimes = lookahead;
		match(lookahead);
		F();
		
		/**/printf(" %c ", otimes);/**/
		otimes = 0;

		Q(); // Q is a tail-recursion
	
	} else {
		;
	}
}

/*R -> OPLUS T R | epsilon*/ 

void R(void){

	/**/int oplus = 0;/**/

	if (lookahead == '+' || lookahead == '-') {
		/**/oplus = lookahead;
		match(lookahead);

		T(); 
		
		/**/printf(" %c ", oplus);/**/
		oplus = 0;

		R();

	} else {
		;
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
