#include <stdio.h>
using namespace std;

---
CompUnit : CompUnit Decl
         | CompUnit FuncDef
         | Decl
         | FuncDef
         ;

Decl : BType VarDefs SEMI ;
BType : INT
      | CHAR
      | VOID
      ;
VarDefs : VarDefs COMMA VarDef
        | VarDef
        ;
VarDef : ID
       | ID ASSIGN InitVal
       | ID LBRKT ConstExp RBRKT
       | ID LBRKT ConstExp RBRKT ASSIGN InitValBlock
       | ID LBRKT ConstExp RBRKT ASSIGN STRING
       ;
InitValBlock : LBRCS RBRCS
             | LBRCS InitVals RBRCS
             ;
InitVals : InitVal
         | InitVal COMMA InitVal
         ;
InitVal : Exp ;
FuncDef : BType ID LPAREN RPAREN FuncBlock
        | BType ID LPAREN FuncFParams RPAREN FuncBlock
        ;
FuncFParams : FuncFParam
            | FuncFParams COMMA FuncFParam
            ;
FuncFParam : BType ID
           | BType ID LBRKT RBRKT
           ;
FuncBlock : LBRCS FuncBlockItems RBRCS
          | LBRCS RBRCS
          ;
FuncBlockItems : FuncBlockItem
               | FuncBlockItems FuncBlockItem
               ;
FuncBlockItem : Decl
              | Stmt
              ;
Stmt : LVal ASSIGN Exp SEMI
     | Exp SEMI
     | FuncBlock
     | IF LPAREN Cond RPAREN Stmt
     | IF LPAREN Cond RPAREN Stmt ELSE Stmt
     | WHILE LPAREN Cond RPAREN Stmt
     | BREAK SEMI
     | CONTINUE SEMI
     | RETURN Exp SEMI
     | RETURN SEMI
     ;
LVal : ID
     | ID LBRKT Exp RBRKT
     ;
Cond : LOrExp ;
Exp : AddExp ;
AddExp : MulExp
       | AddExp PLUS MulExp
       | AddExp MINUS MulExp
       ;
MulExp : UnaryExp
       | MulExp TIMES UnaryExp
       | MulExp DIV UnaryExp
       ;
UnaryExp : PrimaryExp
         | ID LPAREN FuncRParams RPAREN
         | ID LPAREN RPAREN
         | UnaryOp UnaryExp
         ;
PrimaryExp : LPAREN Exp RPAREN
           | LVal
           | NUM
           ;
FuncRParams : Exp
            | FuncRParams COMMA Exp
            ;
UnaryOp : PLUS
        | MINUS
        | EXCLM
        ;
LOrExp : LAndExp
       | LOrExp OR LAndExp
       ;
LAndExp : EqExp
        | LAndExp AND EqExp
        ;
EqExp : RelExp
      | EqExp EQ RelExp
      | EqExp NEQ RelExp
      ;
RelExp : AddExp
       | RelExp LT AddExp
       | RelExp GT AddExp
       | RelExp LE AddExp
       ;
ConstExp : AddExp ;
---

int main() {

  return 0;
}
