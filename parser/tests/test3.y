
%{
#include <stdio.h>
#include <stdlib.h>

extern FILE *fp;

%}

%token INT
%token IF
%token ID

%right '='
%left EQ
%%
start:	Function 
	| Declaration
	;

/* Declaration block */
Declaration: Type Assignment ';' 
	| Assignment ';'  	
	| FunctionCall ';' 	
	;

/* Assignment block */
Assignment: ID '=' Assignment
	| ID '=' FunctionCall
	| ID ',' Assignment
	| ID '+' Assignment
	| '(' Assignment ')'
	| '-' '(' Assignment ')'
	| '-' ID
	|   ID
	;

/* Function Call Block */
FunctionCall : ID '(' ')'
	| ID '(' Assignment ')'
	;

/* Function block */
Function: Type ID '(' ArgListOpt ')' CompoundStmt 
	;
ArgListOpt: ArgList
	|
	;
ArgList:  ArgList ',' Arg
	| Arg
	;
Arg:	Type ID
	;
CompoundStmt:	'{' StmtList '}'
	;
StmtList:	StmtList Stmt
	|
	;
Stmt: Declaration
	| IfStmt
	| ';'
	;

/* Type Identifier block */
Type:	INT 
	;

/* IfStmt Block */
IfStmt : IF '(' Expr ')' Stmt 
	;

/*Expression Block*/
Expr:	
	| Expr EQ Expr
	| Assignment
	;
%%
#include"lex.yy.c"
#include<ctype.h>
int count=0;

int main(int argc, char *argv[])
{
	yyin = fopen(argv[1], "r");
	
   if(!yyparse())
		printf("\nParsing complete\n");
	else
		printf("\nParsing failed\n");
	
	fclose(yyin);
    return 0;
}
         
yyerror(char *s) {
	printf("%d : %s %s\n", yylineno, s, yytext );
}         
