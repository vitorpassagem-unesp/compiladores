//mybc_grp02
/*
Grupo 2: 
Gustavo Varjão de Frias
Vitor Ferreira
Vitor Alves Chuquer Zanetti Passagem
*/

extern char lexeme[]; // definido no lexer.c
extern int lookahead; // definido no parser.c
extern int gettoken(FILE *); // definido no lexer.c
extern void match(int expected); // definido no parser.c

extern FILE *source;

extern int lineno;
extern int colno;
extern int last_colno;
extern int newline_flag;

extern void E(void);
extern void cmd(void);

#define ERRTOKEN -0x1000000
