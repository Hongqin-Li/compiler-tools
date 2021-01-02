%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token SEMI COMMA LBRKT RBRKT LBRCS RBRCS LPAREN RPAREN
%token ID
%token IF ELSE WHILE ASSIGN
%token EQ NEQ LT GT LE AND OR PLUS MINUS TIMES DIV EXCLM
%token NUM STRING


%token TYPEDEF EXTERN STATIC AUTO REGISTER INLINE RESTRICT
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%start CompUnit
%%
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
%%

int main() {
  return 0;
}
