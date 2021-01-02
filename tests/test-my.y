%{
#include <stdio.h>
using namespace std;
%}


%token ID INT REAL STRING IF ELSE FN OP
%token LOOP BREAK RETURN CONTINUE THEN
%token IMPL STRUCT
%start prog
%%
prog
  :
  | prog stm ';'
  | prog estm
  | prog def
  ;

pexp
  : prog
  | prog THEN exp
  ;

stm
  : ID '=' exp
  | BREAK | BREAK exp
  | RETURN | RETURN exp
  | CONTINUE
  ;

exp
  : exp1
  | exp OP exp1
  ;

exps
  : exp
  | exps ',' exp
  ;

exp1
  : '(' exp ')'
  | INT | REAL | STRING
  | ID  | exp1 '.' ID
  | exp1 '[' exp ']'
  | call
  | estm
  ;

call
  : exp1 '(' ')'
  | exp1 '(' exps ')'
  ;

estm
  : loop
  | if
  | fn
  ;
  
loop
  : LOOP '{' prog '}'
  ;

if
  : ifelif
  | ifelif ELSE '{' pexp '}'
  ;
ifelif
  : IF exp '{' pexp '}'
  | ifelif ELSE IF exp '{' pexp '}'
  ;

fn
  : FN ID '(' ')' '{' prog '}'
  | FN ID '(' exps ')' '{' prog '}'
  ;

def
  : struct
  | impl
  ;

struct
  : STRUCT ID '{' fields '}'
  ;
fields
  :
  | fields ID ID ';'
  ;

impl
  : IMPL ID '{' fns '}'
  ; 
fns
  :
  | fns fn
  ;
%%

int main() {
  return 0;
}

