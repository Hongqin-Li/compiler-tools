#include <stdio.h>
using namespace std;

---
prog
  :
  | prog stm ';'
  | prog estm
  | prog def
  ;

eprog
  : prog
  | prog THEN exp
  ;

stm
  : ID '=' exp
  | BREAK | BREAK exp
  | RETURN | RETURN exp
  | CONTINUE
  | call
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
  | ifelif ELSE '{' eprog '}'
  ;
ifelif
  : IF exp '{' eprog '}'
  | ifelif ELSE IF exp '{' eprog '}'
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

---

int main() {
  return 0;
}
