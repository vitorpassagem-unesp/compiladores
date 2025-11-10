#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>
/**********************************************************************************************************
 ********************** Compilador de expressões Pascal para expressões pós-fixas *************************
 **********************************************************************************************************/

int lookahead; // O olho do parser, é por este operador que o parser enxerga antecipadamente os tokens
	       // provenientes do lexer

/*
 * E -> [ ominus ] T { oplus T }
 *  input: "a * b"
 *  token: ID '*' ID
 */
void E(void) 
{ 
	/**/int oplus_flg = 0;/**/
	/**/int otimes_flg = 0;/**/
	/**/int ominus_flg = 0;/**/
	// [ ominus ]; ominus = '+'|'-'
	if (lookahead == '+' || lookahead == '-') {
		/**/if (lookahead == '-') {
			ominus_flg = lookahead;
		}/**/
		match(lookahead);
	}
_Tbegin:
_Fbegin:
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
			// cheque operador atribuição, ":=" => ASGN
			if (lookahead == ASGN) {
				// variable := express
				match(ASGN);
				E();
			}
	}
	/**/if (otimes_flg) {
		printf(" %c ", otimes_flg);/**/
		otimes_flg = 0;// turnoff the otimes flag
	}/**/
	// { otimes T }; otimes = '*'|'/'
	if (lookahead == '*' || lookahead == '/') {
		/**/otimes_flg = lookahead;/**/
		match(lookahead);
		goto _Fbegin;
	}
	/**/if (ominus_flg) {
		printf(" %c ", ominus_flg);
		ominus_flg = 0;// turnoff ominus flag
	}/**/
	/**/if (oplus_flg) {
		printf(" %c ", oplus_flg);/**/
		oplus_flg = 0;// turnoff the otimes flag
	}/**/
	if (lookahead == '+' || lookahead == '-') {
		/**/oplus_flg = lookahead;/**/
		match(lookahead);
		goto _Tbegin;
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
