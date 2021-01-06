#include <bits/stdc++.h>
using namespace std;

---
start
  :	Function 
	| Declaration
	;

Declaration
  : Assignment ';'  	
	| FunctionCall ';' 	
	;

Assignment
  : ID '=' Assignment
	| ID '=' FunctionCall
	| ID ',' Assignment
	| '(' Assignment ')'
	;

FunctionCall
  : ID '(' ')'
	| ID '(' Assignment ')'
	;

Function
  : Type ID '(' ArgListOpt ')' CompoundStmt 
	;
ArgListOpt
  : ArgList
	|
	;
ArgList
  : ArgList ',' Arg
	| Arg
	;
Arg
  : Type ID
	;
CompoundStmt
  :	'{' StmtList '}'
	;
StmtList
  :	StmtList Stmt
	|
	;
Stmt
  : Declaration
	| IfStmt
	| ';'
	;

Type
  :	INT 
	;

IfStmt
  : IF '(' Expr ')' CompoundStmt 
	;

Expr
  :	Expr EQ Expr
	| Assignment
	;
---


int main() {
  return 0;
}
