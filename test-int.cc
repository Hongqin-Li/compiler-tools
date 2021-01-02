#include <stdio.h>
using namespace std;

union Data {
  int a;
  float b;
};

// The first rule represents the whole program.
---
exps
  :
  | exps ';' exp
  ;

exp_list
  :
  | exp_list ',' exp
  ;

exp
  : ID
  | call      { return $1; }
  | loop      { return $1; }
  | ifelse
  | assign
  ;

assign
  : ID '=' exp  { return $2; }
  ;

call
  : ID '(' exp_list ')'
  ;

ifelse
  : IF '(' exp ')' '{' exps '}' ELSE '{' exps '}'
  ;

loop
  : LOOP '{' exp_list '}'
  ;

---

int main() {
  return 0;
}
