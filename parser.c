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
#include <setjmp.h> /*Biblioteca com funções de jump para restaurar o controle do interpretador em caso de erro*/
#include <tokens.h>
#include <parser.h>
#include <signal.h> //Biblioteca para tratar sinais do sistema operacional (Ex: Ctrl + C)

/**********************************************************************************************************
 ************************************ Interpretador de Expressões MyBC ************************************
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

int lookahead; // Token corrente visualizado pelo parser (lookahead de 1 símbolo)

double acc; // Acumulador para armazenar resultado intermediário das operações

#define STACK_SIZE 256

// Pilha para salvar operandos durante avaliação de expressões
double stack[STACK_SIZE];

//stack pointer
int sp = -1; // Ponteiro de pilha: -1 representa pilha vazia

#define VMEM_SIZE 4096
double vmem[VMEM_SIZE];           // Memória virtual para armazenar valores de variáveis
char symboltable[VMEM_SIZE];      // Tabela de símbolos (nomes de variáveis)
int symboltable_next_query = 0;  // Próxima posição livre na tabela
double recall(char * symbol);    // Recupera valor de variável
int store(char *symbol);         // Armazena valor de variável

//Variável que irá indicar o ponto a partir do qual o código é executado quando a função longjmp() é chamada
jmp_buf erro;

//Interpretador de comadno vai ser um laço infinito (enquanto dure), que fica sempre buscando um ';' ou '\n', ele é interrompido se avistar um EOF(ctrl + d) ou comando quit ou exit

/*
mybc -> cmd{ CMDSEP cmd} EOF

cmd -> E | quit | exit | <epsilon>

CMDSEP -> [;\n] == ';' | '\n' // definido no lexer

*/

//Função para traduzir o token esperado em erro legível
const char *tokenname(int token){
	switch(token){
		case EXIT:
			return "EXIT";
		case QUIT:
			return "QUIT";
		case ID:
		case DEC:
		case FLT:
		case HEX:
		case OCT:
			return "número";
		case ASGN:
			return "ASGN";
		case EOF:
			return "EOF";
		case '+':
			return "'+'";
		case '-':
			return "'-'";
		case '*':
			return "'*'";
		case '/':
			return "'/'";
		case '(':
			return "'('";
		case ')':
			return "')'";
		case ';':
			return "';'";
		case '\n':
			return "'\\n'";
		default: {
			// Tokens não mapeados: imprime caractere ou código numérico
			static char buf[16];
			if (isprint(token))
				sprintf(buf, "'%c'", token);
			else
				sprintf(buf, "token %d", token);
			return buf;
		}
	}
}

// Handler para captura de sinais do sistema (ex: Ctrl+C)
void  handle_signal(int signal){
	(void)signal; // Suprime warning de parâmetro não usado
	printf("\n");
	/**/fflush(stdout);/**/ // Força impressão imediata da quebra de linha
}

void mybc(){
        // Ponto de retorno em caso de erro: devolve controle ao interpretador
        int controle = setjmp(erro);

		signal(SIGINT, handle_signal); // Registra tratamento do Ctrl+C

		if(controle != 0){
				// Em caso de erro, restaura estado inicial
				/**/sp = -1;/**/  // Esvazia a pilha
				/**/acc = 0;/**/  // Zera o acumulador
		}
        
	cmd(); // Processa primeiro comando
	// Loop principal: consome comandos separados por ';' ou '\n'
	while(lookahead == ';' || lookahead == '\n'){
		match(lookahead); // Consome separador
		cmd();            // Processa próximo comando
		if(lookahead ==  EOF){
			break; // Sai se encontrar fim de arquivo
		}
	}
	match(EOF); // Valida fim de entrada
}

void cmd(void) {
	// Decide ação com base no token atual
	switch(lookahead){
		case EXIT:
		case QUIT:
			exit(0); // Encerra interpretador
		
		// FIRST(E): símbolos que podem iniciar uma expressão
		case'+':
		case'-':
		case'(':
		case HEX:
		case OCT:
		case FLT:
		case DEC:
		case ID:
			E(); // Avalia expressão
			/**/printf("%lg\n", acc);/**/ // Imprime resultado no acumulador
			break;
		default:
				; // Comando vazio (transição epsilon)
	}
}


/*
 * E -> [ ominus ] T { oplus T }
 * Avalia expressões aritméticas com precedência de operadores
 * input: "a * b"
 * token: ID '*' ID
 */

void E() 
{ 
	/**/int oplus_flg = 0;/**/   // Flag para operadores + ou -
	/**/int otimes_flg = 0;/**/  // Flag para operadores * ou /
	/**/int ominus_flg = 0;/**/  // Flag para sinal negativo unário
	
	// [ ominus ]; ominus = '+'|'-'
	// Trata sinal unário opcional no início
	if (lookahead == '+' || lookahead == '-') {
		/**/if (lookahead == '-') {
			ominus_flg = lookahead; // Marca negativo para aplicar depois
		}/**/
		match(lookahead);
	}
_Tbegin: // Label para processar termos (precedência de + e -)
_Fbegin: // Label para processar fatores (precedência de * e /)
	switch(lookahead) {
		// Expressão entre parênteses
		case '(':
			match('('); 
			E();        // Recursão para sub-expressão
			match(')');
			break;
		
		// Constantes numéricas hexadecimais
		case HEX:
			match(HEX); break;
		case OCT:
			match(OCT); break;
		case DEC:
			/**/acc = atoi(lexeme);/**/ // Converte string para inteiro e armazena
			match(DEC);

			// Detecta erro: dois números consecutivos sem operador
			if (lookahead == DEC || lookahead == FLT || lookahead == HEX || lookahead == OCT || lookahead == ID) {
				fprintf(stderr,
					"Erro de sintaxe na linha %d, coluna %d: é esperado um operador antes de %s ('%s')\n",
					lineno, colno,
					tokenname(lookahead),
					lexeme
				);
				//exit(ERRTOKEN);
			}

			break;

		case FLT:
			/**/acc = atof(lexeme);/**/ // Converte string para float e armazena
			match(FLT); break;
		default:
			// Identificador (variável)
			match(ID);
			// Verifica se é atribuição
			if (lookahead == ASGN) {
				match(ASGN);
				E(); // Avalia lado direito da atribuição
			}
	}
	
	// Aplica operação multiplicativa pendente (* ou /)
	/**/if (otimes_flg) {
		if(otimes_flg == '*'){
			acc = stack[sp] * acc; // Multiplica operando salvo com resultado atual
		}
		else{
			acc = stack[sp] / acc; // Divide operando salvo por resultado atual
		}
		sp--;              // Remove operando da pilha (pop)
		otimes_flg = 0;    // Limpa flag
	}/**/
	
	// { otimes T }; otimes = '*'|'/'
	// Processa operadores multiplicativos (maior precedência)
	if (lookahead == '*' || lookahead == '/') {
		/**/otimes_flg = lookahead;/**/ // Salva qual operador
		/**/
		sp++;             // Empilha resultado atual (push)
		stack[sp] = acc;
		/**/
		match(lookahead);
		goto _Fbegin;     // Volta para processar próximo fator
	}
	
	// Aplica sinal negativo unário
	/**/if (ominus_flg) {
		acc = -acc;
		ominus_flg = 0;
	}/**/
	
	// Aplica operação aditiva pendente (+ ou -)
	/**/if (oplus_flg) {
		if(oplus_flg == '+'){
			acc = stack[sp] + acc; // Soma operando salvo com resultado atual
		}
		else{
			acc = stack[sp] - acc; // Subtrai resultado atual do operando salvo
		}
		sp--;             // Remove operando da pilha (pop)
		oplus_flg = 0;    // Limpa flag
	}/**/
	
	// Processa operadores aditivos (menor precedência)
	if (lookahead == '+' || lookahead == '-') {
		/**/oplus_flg = lookahead;/**/ // Salva qual operador
		/**/
		sp++;             // Empilha resultado atual (push)
		stack[sp] = acc;
		/**/
		match(lookahead);
		goto _Tbegin;     // Volta para processar próximo termo
	}
}


/*** A principal função (procedimento) interface do parser é a match
-> match vai consumir um token da cadeia de entrada, se ele corresponder com a sintaxe
 */
void match(int expected_token)
{
	if (lookahead == expected_token) {

		// Atualiza contador de linha quando newline é consumido
		if (lookahead == '\n' && newline_flag) {
            /**/lineno++;/**/       // Incrementa linha
			/**/colno = 1;/**/      // Reseta coluna
            /**/newline_flag = 0;/**/ // Limpa flag
        }
		
		/**/lookahead = gettoken(source);/**/ // Avança para próximo token

	} else {
		// Erro de sintaxe: token encontrado não corresponde ao esperado
		fprintf(stderr, "Erro de sintaxe na linha %d e coluna %d.\n", lineno, last_colno);
		printf("Token Esperado: %s --- Token no lookahead: %s\n", tokenname(expected_token), tokenname(lookahead));
		//exit(ERRTOKEN);
		
                // Restaura estado do interpretador
                /**/sp = -1;/**/   // Esvazia pilha
                /**/acc = 0;/**/   // Zera acumulador

                // Descarta tokens até próximo separador de comando
                while (lookahead != ';' && lookahead != '\n' && lookahead != EOF) {
                /**/lookahead = gettoken(source);/**/
            }
          // Retorna controle ao interpretador principal
          longjmp(erro, 1);
	}
}
