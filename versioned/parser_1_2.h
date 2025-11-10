extern char lexeme[]; // definido no lexer.c
extern int lookahead; // definido no parser.c
extern int gettoken(FILE *); // definido no lexer.c
extern void match(int expected); // definido no parser.c

extern FILE *source;

extern int lineno;

#define ERRTOKEN -0x1000000
