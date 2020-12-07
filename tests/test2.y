%token X

// %start translation_unit
%%
S
  : V '=' E
  | E
  ;
E
  : V
  ;
V
  : X
  | '*' E
  ;
%%
#include <stdio.h>

extern char yytext[];
extern int column;

void yyerror(char const *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

