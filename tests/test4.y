%{
#include <stdio.h>
using namespace std;
%}


%token ID ASSIGN IF ELSE THEN
%start stmlist
%%
stmlist
  : stm
  | stmlist stm
  ;

stm
  : ID ASSIGN ID
  | IF ID THEN stm
  | IF ID THEN stm ELSE stm
  ;
%%

int main() {
  return 0;
}
