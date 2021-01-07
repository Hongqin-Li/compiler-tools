// ECMAScript 1 Grammar http://www.ecma-international.org/publications/files/ECMA-ST-ARCH/ECMA-262,%201st%20edition,%20June%201997.pdf
%{
%}
%token FUNCTION VAR NEW DELETE TYPEOF
%token ID VOID STRING FLOAT INT
%token IF ELSE WHILE FOR IN CONTINUE BREAK RETURN WITH
%token TRUE FALSE NULL THIS
%token OR AND NE EQ GT LT LE GE
%token SLL SRL SRA
%token INC DEC
%token ASSIGN_ADD ASSIGN_MUL ASSIGN_DIV ASSIGN_MOD ASSIGN_SLL ASSIGN_SRL ASSIGN_SRA ASSIGN_AND ASSIGN_XOR ASSIGN_OR

%start Program
%%
Program
  : SourceElements
  ;

SourceElements
  : SourceElement
  | SourceElements SourceElement
  ;

SourceElement
  : Statement
  | FunctionDeclaration
  ;

Statement
  : Block
  | VariableStatement
  | EmptyStatement
  | ExpressionStatement
  | IfStatement
  | IterationStatement
  | ContinueStatement
  | BreakStatement
  | ReturnStatement
  | WithStatement
  ;

Block
  : '{' '}'
  | '{' StatementList '}'
  ;

StatementList
  : Statement
  | StatementList Statement
  ;

VariableStatement
  : VAR VariableDeclarationList ';'
  ;

VariableDeclarationList
  : VariableDeclaration
  | VariableDeclarationList ',' VariableDeclaration
  ;

VariableDeclaration
  : ID
  | ID Initializer
  ;

Initializer
  : '=' AssignmentExpression
  ;

AssignmentExpression
  : ConditionalExpression
  | LeftHandSideExpression AssignmentOperator AssignmentExpression
  ;

AssignmentOperator
  : '='
  | ASSIGN_MUL
  | ASSIGN_DIV
  | ASSIGN_MOD
  | ASSIGN_ADD
  | ASSIGN_SLL
  | ASSIGN_SRL
  | ASSIGN_SRA
  | ASSIGN_AND
  | ASSIGN_XOR
  | ASSIGN_OR
  ;

ConditionalExpression
  : LogicalORExpression
  | LogicalORExpression '?' AssignmentExpression ':' AssignmentExpression
  ;

LogicalORExpression
  : LogicalANDExpression
  | LogicalORExpression OR LogicalANDExpression
  ;

LogicalANDExpression
  : BitwiseORExpression
  | LogicalANDExpression AND BitwiseORExpression
  ;

BitwiseORExpression
  : BitwiseXORExpression
  | BitwiseORExpression '|' BitwiseXORExpression
  ;

BitwiseXORExpression
  : BitwiseANDExpression
  | BitwiseXORExpression '^' BitwiseANDExpression
  ;

BitwiseANDExpression
  : EqualityExpression
  | BitwiseANDExpression '&' EqualityExpression
  ;

EqualityExpression
  : RelationalExpression
  | EqualityExpression EQ RelationalExpression
  | EqualityExpression NE RelationalExpression
  ;

RelationalExpression
  : ShiftExpression
  | RelationalExpression LT ShiftExpression
  | RelationalExpression GT ShiftExpression
  | RelationalExpression LE ShiftExpression
  | RelationalExpression GE ShiftExpression
  ;

ShiftExpression
  : AdditiveExpression
  | ShiftExpression SLL AdditiveExpression
  | ShiftExpression SRL AdditiveExpression
  | ShiftExpression SRA AdditiveExpression
  ;

AdditiveExpression
  : MultiplicativeExpression
  | AdditiveExpression '+' MultiplicativeExpression
  | AdditiveExpression '-' MultiplicativeExpression
  ;

MultiplicativeExpression
  : UnaryExpression
  | MultiplicativeExpression '*' UnaryExpression
  | MultiplicativeExpression '/' UnaryExpression
  | MultiplicativeExpression '%' UnaryExpression
  ;

UnaryExpression
  : PostfixExpression
  | DELETE UnaryExpression
  | VOID UnaryExpression
  | TYPEOF UnaryExpression
  | INC UnaryExpression
  | DEC UnaryExpression
  | '+' UnaryExpression
  | '-' UnaryExpression
  | '~' UnaryExpression
  | '!' UnaryExpression
  ;

PostfixExpression
  : LeftHandSideExpression
  | LeftHandSideExpression INC
  | LeftHandSideExpression DEC
  ;

LeftHandSideExpression
  : NewExpression
  | CallExpression
  ;

NewExpression
  : MemberExpression
  | NEW NewExpression
  ;

MemberExpression
  : PrimaryExpression
  | MemberExpression '[' Expression ']'
  | MemberExpression '.' ID
  | NEW MemberExpression Arguments
  ;

PrimaryExpression
  : THIS
  | ID
  | STRING
  | '(' Expression ')'
  ;

Expression
  : AssignmentExpression
  | Expression ',' AssignmentExpression
  ;

ExpressionOpt
  :
  | Expression
  ;

Arguments
  : '(' ')'
  | '(' ArgumentList ')'
  ;

ArgumentList
  : AssignmentExpression
  | ArgumentList ',' AssignmentExpression
  ;

CallExpression
  : MemberExpression Arguments
  | CallExpression Arguments
  | CallExpression '[' Expression ']'
  | CallExpression '.' ID
  ;

EmptyStatement
  : ';'
  ;

ExpressionStatement
  : Expression ';'
  ;

IfStatement
  : IF '(' Expression ')' Statement ELSE Statement
  | IF '(' Expression ')' Statement
  ;

IterationStatement
  : WHILE '(' Expression ')' Statement
  | FOR '(' ExpressionOpt ';' ExpressionOpt ';' ExpressionOpt ')' Statement
  | FOR '(' VAR VariableDeclarationList ';' ExpressionOpt ';' Expression ')' Statement
  | FOR '(' VAR VariableDeclarationList ';' ExpressionOpt ';' ')' Statement
  | FOR '(' LeftHandSideExpression IN Expression ')' Statement
  | FOR '(' VAR ID Initializer IN Expression ')' Statement
  | FOR '(' VAR ID IN Expression ')' Statement
  ;

ContinueStatement
  : CONTINUE ';'
  ;

BreakStatement
  : BREAK ';'
  ;

ReturnStatement
  : RETURN ';'
  | RETURN Expression ';'
  ;

WithStatement
  : WITH '(' Expression ')' Statement
  ;

FunctionDeclaration
  : FUNCTION ID '(' ')' Block
  | FUNCTION ID '(' FormalParameterList ')' Block
  ;

FormalParameterList
  : ID
  | FormalParameterList ',' ID
  ;

%%

int main() {
  return 0;
}
