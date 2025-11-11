//mybc_grp02
/*
Grupo 2: 
Gustavo Varjão de Frias
Vitor Ferreira
Vitor Alves Chuquer Zanetti Passagem
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>

// Buffer global para armazenar o lexema reconhecido
char lexeme[MAXIDLEN + 1];

// Contadores de posição no arquivo fonte
int lineno = 1;      // Linha atual sendo processada
int colno = 1;       // Coluna atual no buffer de leitura
int last_colno = 1;  // Última coluna válida antes de newline
int newline_flag = 0; // Flag indicando que '\n' foi encontrado

//last_colno: guarda a última coluna válida antes de '\n', usada nas mensagens de erro
// para evitar que o erro seja identificado na primeira coluna da proxima linha após o ENTER.

//newline_flag: indica que um '\n' foi lido; o parser usa essa flag para atualizar
// o número da linha (lineno) no momento certo, evitando incrementos duplos.


/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
	// Testa se o primeiro caractere é uma letra
	if (isalpha(lexeme[0] = getc(tape))) {
		int i = 1;
		/**/colno++; last_colno = colno;/**/ // Atualiza posição após ler primeiro caractere

		// Consome sequência alfanumérica
		while (isalnum(lexeme[i] = getc(tape))) {
			/**/i++; colno++; last_colno = colno;/**/ // Avança índice e posição
		}
		ungetc(lexeme[i], tape); // Devolve caractere que não faz parte do ID
		lexeme[i] = 0;

		// Verifica se é palavra reservada
		if (strcmp(lexeme, "exit") == 0) return EXIT;
		if (strcmp(lexeme, "quit") == 0) return QUIT;
		
		/**/return ID;/**/ // Identificador válido reconhecido
	}

	// Primeiro caractere não é letra, devolve e retorna falha
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

/*
 * DEC = [1-9][0-9]* | '0'
 */

/*
 * DEC = [1-9][0-9]* | '0'
 *                           ------------------------------------------
 *                          |                      digit               |
 *                          |                    --------              |
 *                          |                   |        |             |
 *               digit      |     not zero      V        |  epsilon    V
 * -->(is DEC)--------->(is ZERO)---------->(isdigit)-------------->((DEC))
 *       |
 *       | epsilon
 *       |
 *       V
 *     ((0))
 */

int isDEC(FILE *tape)
{
	// Verifica se primeiro caractere é dígito
	if (isdigit(lexeme[0] = getc(tape))) {
		int i = 1;
		/**/colno++; last_colno = colno;/**/ // Atualiza posição

		// Zero isolado é válido como decimal
		if (lexeme[0] == '0') return DEC;

		// Consome demais dígitos (números de 1-9 seguidos de 0-9)
		while (isdigit(lexeme[i] = getc(tape))) {
			/**/i++; colno++; last_colno = colno;/**/ // Avança no lexema e na coluna
		}
		ungetc(lexeme[i], tape); // Devolve caractere não numérico
		lexeme[i] = 0;
		/**/return DEC;/**/ // Token decimal reconhecido
	}

	// Não é dígito, devolve caractere
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// fpoint = DEC\.[0-9]* | \.[0-9][0-9]*
// flt = fpoint EE? | DEC EE
// EE = [eE]['+''-']?[0-9][0-9]*
// test input: 3e+
//             012

int isEE(FILE *tape)
{
	int i = strlen(lexeme);

	// Verifica se há expoente 'E' ou 'e'
	if ( toupper(lexeme[i] = getc(tape)) == 'E' ) {
		i++;
		
		// check optional signal
		int hassign;
		// Aceita sinal opcional após 'E'
		if ( (lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-' ) {
			/**/hassign = i++;/**/ // Marca presença de sinal
		} else {
			hassign = 0;
			ungetc(lexeme[i], tape); // Devolve se não for sinal
		}
		
		// check required digit following
		// Expoente exige pelo menos um dígito após E ou E+/-
		if ( isdigit(lexeme[i] = getc(tape)) ) {
			i++;
			// Consome demais dígitos do expoente
			while( isdigit(lexeme[i] = getc(tape)) ) i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			/**/return FLT;/**/ // Notação científica válida
		}
		// Expoente sem dígito: desfaz leitura
		ungetc(lexeme[i], tape);
		i--;
		if (hassign) {
			ungetc(lexeme[i], tape); // Devolve sinal também
			i--;
		}
	}

	// Não há expoente válido
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;	
	return 0;
}

int isNUM(FILE *tape)
{
	int token = isDEC(tape);
	// Caso 1: número começa com dígito (DEC reconhecido)
	if (token == DEC) {
		int i = strlen(lexeme);
		// Verifica se há ponto decimal
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			i++;
			// Consome dígitos da parte fracionária (opcional)
			while ( isdigit( lexeme[i] = getc(tape) ) ) i++;
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			/**/token = FLT;/**/ // Presença de ponto torna o número float
		} else {
			ungetc(lexeme[i], tape); // Não há ponto, mantém como DEC
			lexeme[i] = 0;
		}
	} else {
		// Caso 2: número começa com ponto (formato .123)
		if ( (lexeme[0] = getc(tape)) == '.' ) {
			// Ponto deve ser seguido por dígito para ser número válido
			if ( isdigit( lexeme[1] = getc(tape) ) ) {
				/**/token = FLT;/**/ // Formato .N é float
				int i = 2;
				// Consome demais dígitos
				while ( isdigit( lexeme[i] = getc(tape) ) ) i++;
			} else {
				// Ponto isolado não é número
				ungetc(lexeme[1], tape);
				ungetc(lexeme[0], tape);
				lexeme[0] = 0;
				return 0; // not a number
			}
		} else {
			// Não começa com dígito nem ponto
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			return 0; // not a number
		}
	}
	
	// Verifica se há notação científica (E ou e)
	if (isEE(tape)) {
		/**/token = FLT;/**/ // Expoente força classificação como float
	}

	return token;
}

/*
 * OCT = '0'[0-7]+
 */
int isOCT(FILE *tape)
{
	// Tenta reconhecer octal: deve começar com '0'
	if ( (lexeme[0] = getc(tape)) == '0') {
		int i = 1;
		// Segundo caractere deve ser dígito octal (0-7)
		if ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] <= '7') {
			i = 2;
			// Consome demais dígitos octais
			while ((lexeme[i] = getc(tape)) >= '0' && lexeme[i] <= '7') i++;
			ungetc(lexeme[i], tape); // Devolve primeiro não-octal
			lexeme[i] = 0;
			/**/return OCT;/**/ // Número octal válido
		}
		// Não é octal válido, desfaz leitura
		ungetc(lexeme[1], tape);
		ungetc(lexeme[0], tape);
	} else {
		// Não começa com '0'
		ungetc(lexeme[0], tape);
	}
	return 0;
}

/*
 * HEX = '0'[Xx][0-9A-Fa-f]+
 *
 * isxdigit == [0-9A-Fa-f]
 */
int isHEX(FILE *tape)
{
	// Para ter um hexa, é necessário que venha o prefixo "0[xX]" seguido de um hexa digito
	if ( (lexeme[0] = getc(tape)) == '0' ) {
		// Verifica se segundo caractere é 'X' ou 'x'
		if ( toupper(lexeme[1] = getc(tape)) == 'X' ) {
			// Terceiro caractere deve ser dígito hexadecimal
			if ( isxdigit(lexeme[2] = getc(tape)) ) {
				int i = 3;
				// Consome demais dígitos hexadecimais
				while ( isxdigit(lexeme[i] = getc(tape)) ) i++;
				ungetc(lexeme[i], tape); // Devolve primeiro não-hex
				lexeme[i] = 0;
				/**/return HEX;/**/ // Hexadecimal válido reconhecido
			}
			// Não há dígito hex após 0x, desfaz leitura
			ungetc(lexeme[2], tape);
			ungetc(lexeme[1], tape);
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			return 0;
		}
		// Não é 'X' após '0', desfaz
		ungetc(lexeme[1], tape);
		ungetc(lexeme[0], tape);
		lexeme[0] = 0;
		return 0;
	}
	// Não começa com '0'
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

int isASGN(FILE *tape)
{
	lexeme[0] = getc(tape);
	// Verifica se é início do operador ':='
	if (lexeme[0] == ':') {
		lexeme[1] = getc(tape);
		// Confirma segundo caractere '='
		if (lexeme[1] == '=') {
			lexeme[2] = 0;
			/**/colno += 2; last_colno = colno;/**/ // Atualiza coluna para operador de 2 chars
			/**/return ASGN;/**/ // Token de atribuição reconhecido
		}
		ungetc(lexeme[1], tape); // Não é ':=', devolve segundo char
	}
	ungetc(lexeme[0], tape); // Devolve primeiro char
	return lexeme[0] = 0;
}

void skipspaces(FILE *tape)
{
	int head;
	// Consome caracteres de espaço em branco
	while (isspace(head = getc(tape))) {
		// Newline requer tratamento especial
		if (head == '\n') {
			/**/newline_flag = 1;/**/ // Sinaliza detecção de nova linha
			/**/last_colno = colno;/**/ // Salva posição antes da quebra
			break; // Para para devolver '\n' como token
		}
		else if (head != EOF) {
			/**/colno++;/**/ // Avança coluna para espaços normais
			/**/last_colno = colno;/**/
		}
	}
	ungetc(head, tape); // Devolve caractere não-espaço ou '\n'
}

int gettoken(FILE *source)
{
	int token;

	skipspaces(source); // Ignora espaços em branco

	// Tenta reconhecer tokens compostos (ordem de prioridade importante)
	if ((token = isID(source)))  return token;  // Identificadores e palavras reservadas
	if ((token = isHEX(source))) return token;  // Hexadecimais (0x...)
	if ((token = isOCT(source))) return token;  // Octais (0...)
	if ((token = isNUM(source))) return token;  // Números (inteiros e floats) - DEVE vir antes de isDEC
	if ((token = isASGN(source))) return token; // Operador de atribuição :=

	// Lê caractere ASCII simples (operadores, delimitadores)
	lexeme[0] = token = getc(source);
	lexeme[1] = 0;

	// Tokens ASCII simples - atualiza posição e retorna
	switch (token) {
		case '+': 
		case '-': 
		case '*': 
		case '/':
		case '(': 
		case ')': 
		case ';': 
		case '.':
			/**/colno++; last_colno = colno;/**/ // Avança coluna
			return token;

		case '\n':
			/**/newline_flag = 1;/**/ // Marca flag de nova linha
			/**/last_colno = colno;/**/ // Salva posição
			return '\n';

		case EOF:
			return EOF; // Fim de arquivo

		default:
			// Caractere inválido - reporta erro léxico
			fprintf(stderr, "Erro léxico na linha %d e coluna %d: charactere inválido '%c'\n", lineno, last_colno, token);
			break;
	}
}
