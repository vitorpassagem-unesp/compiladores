//mybc_grp02
/*
Grupo 2: 
Gustavo Varjão de Frias
Vitor Ferreira
Vitor Alves Chuquer Zanetti Passagem
*/

//parser_1_2

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>
/**********************************************************************************************************
 ********************** Compilador de expressões Pascal para expressões pós-fixas *************************
 **********************************************************************************************************/


/* LR(1)-grammar for simple expressions - initial symbol: E
 *
 * E -> E + T | E - T | T
 * T -> T * F | T / F | F
 * F -> ID | DEC | ( E )
 *
 * LL(1)-grammar for simple expressions - initial symbol: E
 * E -> T R
 * R -> + T R | - T R | <epsilon>
 * T -> F Q
 * Q -> * F Q | / F Q | <epsilon>
 * F -> ID | DEC | ( E )
 */

int lookahead; // O olho do parser, é por este operador que o parser enxerga antecipadamente os tokens
	       // provenientes do lexer

double acc; // acumulador para fazer a operação que vier na expressão, se acumulador acc já estiver cheio utiliza-se stk = acc -> para exp: + -> acc = acc + stk

#define STACK_SIZE 256

// como é a seção bss, o array será inicializado com zero
double stack[STACK_SIZE];

//stack pointer
int sp = -1; //-1 representa pilha vazia

#define VMEM_SIZE 4096
double vmem[VMEM_SIZE];
char symboltable[VMEM_SIZE];
int symboltable_next_query = 0;//se tiver vazia, a proxima posição livre é 0
double recall(char * symbol);
int store(char *symbol);


//Interpretador de comadno vai ser um laço infinito (enquanto dure), que fica sempre buscando um ';' ou '\n', ele é interrompido se avistar um EOF(ctrl + d) ou comando quit ou exit

/*
mybc -> cmd{ CMDSEP cmd} EOF

cmd -> E | quit | exit | <epsilon>

CMDSEP -> [;\n] == ';' | '\n' // definido no lexer

*/

void mybc(){
	cmd();
	while(lookahead == ';' || lookahead == '\n'){
		match(lookahead);
		cmd();
		if(lookahead ==  EOF){
			break;
		}
	}
	match(EOF);
}

void cmd(void) {
	switch(lookahead){
		case EXIT:
		case QUIT:
			exit(0);
		//Aqui é o FIRST de E -> melhor fazer a função para pegar
		case'+':
		case'-':
		case'(':
		case HEX:
		case OCT:
		case FLT:
		case DEC:
		case ID:
			E();
			/**/printf("%lg\n", acc);/**/
			break;
		default:
				; // transição epsilon
	}
}


/*
 * E -> [ ominus ] T { oplus T }
 * input: "a * b"
 * token: ID '*' ID
 */


void E() 
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
		//expre entre parênteses 
		case '(':
			match('('); E(); match(')');
			break;
		//sessão de constantes -> c no diagrama sintático
		case HEX:
			//=/**/printf(" %s ", lexeme);/**/
			match(HEX); break;
		case OCT:
			//=/**/printf(" %s ", lexeme);/**/
			match(OCT); break;
		case DEC:
			//=/**/printf(" %s ", lexeme);/**/
			/**/acc = atoi(lexeme); /**/ //-> variavel que vai representar o registrador eax para fazer a conversão de string p/ numero
			// faço direto, pq sei que vai vir um novo operador, ai preciso usar stk
			match(DEC); break;
		case FLT:
			//=/**/printf(" %s ", lexeme);/**/
			/**/acc = atof(lexeme); /**/
			match(FLT); break;
		default:
			//envolve variaveis
			//=/**/printf(" %s ", lexeme);/**/
			match(ID);
			// cheque operador atribuição, ":=" => ASGN
			if (lookahead == ASGN) {
				// variable := express
				match(ASGN);
				E();
			}
	}
	//Se chegou operador multiplicativo, é pq já houve uma entrada de número no acc (acumulador) como 1° operando
	/**/if (otimes_flg) {
		if(otimes_flg == '*'){
			acc = stack[sp] * acc;
		}
		else{
			acc = stack[sp] / acc;
		}
		sp--; // decrementou quem está na pilha(pop)
		otimes_flg = 0;// turnoff the otimes flag
	}/**/
	// { otimes T }; otimes = '*'|'/'
	if (lookahead == '*' || lookahead == '/') {
		/**/otimes_flg = lookahead;/**/
		/**/
		// salva na pilha porque há de vir um próximo número e precisamos de acc, então colocamos na pilha
		//push(acc)
		sp++;
		stack[sp] = acc;
		/**/
		match(lookahead);
		goto _Fbegin;
	}
	/**/if (ominus_flg) {
		acc = -acc;
		ominus_flg = 0;// turnoff ominus flag
	}/**/
	/**/if (oplus_flg) {
		if(oplus_flg == '+'){
			acc = stack[sp] + acc;
		}
		else{
			acc = stack[sp] - acc;
		}
		sp--; // decrementou quem está na pilha(pop)
		oplus_flg = 0;// turnoff the oplus flag
	}/**/
	if (lookahead == '+' || lookahead == '-') {
		/**/oplus_flg = lookahead;/**/
		// salva na pilha porque há de vir um próximo número e precisamos de acc, então colocamos na pilha
		//push(acc)
		/**/ // um meta -> não faz parte da linguagem
		sp++;
		stack[sp] = acc;
		/**/
		match(lookahead);
		goto _Tbegin;
	}
}

/*** A principal função (procedimento) interface do parser é a match
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
