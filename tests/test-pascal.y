// https://github.com/FANMixco/minipascal-simplified
%{
%}

%left T_OR
%left T_AND
%left T_CEQ T_CNE
%left T_CLT T_CGT T_CLE T_CGE
%left T_PLUS T_MINUS
%left T_MUL T_DIV T_MOD
%left T_NOT

//This collection of tokens are related to the keywords reserved by Pascal such as begin, end, etc. More information pascal.l file
%token T_PROGRAM T_VAR T_PROCEDURE T_FUNCTION T_BEGIN T_END
%token T_IF T_THEN T_ELSE T_WHILE T_FOR T_TO T_DO
%token T_ASSIGNOP T_OBRACKET T_CBRACKET T_SEMICOLON T_COLON T_COMMA T_DOT
%token T_WRITE_INT T_WRITE_CHAR T_WRITE_BOOL T_WRITE_LN

//This tokens has special types because they are related to real information to use and they are not constants like IF or OR, they mean Regular Expressions
%token T_STANDARD_TYPE
%token T_IDENTIFIER
%token T_INT
%token T_BOOL
%token T_CHAR
%token T_REAL

//This are the keywords that we're gonna use accross the grammar
%type program program_decl declarations multi_var_decl var_decl list_id multi_id id
%type subprogram_declarations multi_subprogram_decl subprogram_decl procedure_decl function_decl arg arg_list multi_arg
%type compound_statement statements optional_statements multi_statement statement matched_statement unmatched_statement
%type if matched_if while for
%type write_statement write_char write_int write_bool write_ln
%type expression simple_expression not_element element term
%type call call_arg_list call_arg call_multi_arg
%type assignop identifier standard_type
%type add_op mul_op rel_op not_op

//This expression indicates to Bison where to start the translation
%start program

%%
program
  : program_decl declarations subprogram_declarations compound_statement
  ;

program_decl
  : T_PROGRAM identifier T_SEMICOLON
  ;

declarations
  : multi_var_decl
  ;

multi_var_decl
  : var_decl multi_var_decl
  |
  ;

var_decl
  : T_VAR list_id T_COLON T_STANDARD_TYPE T_SEMICOLON
  ;

list_id
  : id multi_id
  ;

multi_id
  : T_COMMA id multi_id
  |
  ;

id
  : identifier
  ;


subprogram_declarations
  : subprogram_decl multi_subprogram_decl
  |
  ;

multi_subprogram_decl
  : subprogram_decl multi_subprogram_decl
  |
  ;

subprogram_decl
  : procedure_decl
  | function_decl
  ;

procedure_decl
  : T_PROCEDURE identifier T_OBRACKET arg_list T_CBRACKET
    T_SEMICOLON declarations T_BEGIN optional_statements T_END T_SEMICOLON
  ;

function_decl
  : T_FUNCTION identifier T_OBRACKET arg_list T_CBRACKET T_COLON T_STANDARD_TYPE
    T_SEMICOLON declarations T_BEGIN optional_statements T_END T_SEMICOLON
  ;

arg_list
  : arg multi_arg
  |
  ;

multi_arg
  : T_COMMA arg multi_arg
  |
  ;

arg
  : identifier T_COLON T_STANDARD_TYPE
  ;

compound_statement
  : T_BEGIN optional_statements T_END T_DOT
  |
  ;

statements
  : statement
  | T_BEGIN optional_statements T_END
  ;

optional_statements
  : statement multi_statement
  |
  ;

multi_statement
  : T_SEMICOLON statement multi_statement
  |
  ;

statement
  : matched_statement
  | unmatched_statement
  ;

matched_statement
  : assignop
  | matched_if
  | while
  | for
  | call
  | write_statement
  ;

unmatched_statement
  : if
  | T_IF expression T_THEN matched_statement T_ELSE unmatched_statement
  ;

if
  : T_IF expression T_THEN statements
  ;

matched_if
  : T_IF expression T_THEN matched_statement T_ELSE matched_statement
  ;

write_statement
  : write_int
  | write_char
  | write_bool
  | write_ln
  ;

write_int
  : T_WRITE_INT T_OBRACKET expression T_CBRACKET
  ;

write_char
  : T_WRITE_CHAR T_OBRACKET expression T_CBRACKET
  ;

write_bool
  : T_WRITE_BOOL T_OBRACKET expression T_CBRACKET
  ;

write_ln
  : T_WRITE_LN T_OBRACKET T_CBRACKET
  ;

assignop
  : identifier T_ASSIGNOP expression
  ;

while
  : T_WHILE expression T_DO statements
  ;

for
  : T_FOR assignop T_TO expression T_DO statements
  ;

expression
  : simple_expression
  | simple_expression rel_op simple_expression
  ;

simple_expression
  : term
  | simple_expression add_op term
  ;

term
  : not_element
  | term mul_op not_element
  ;

not_element
  : element
  | not_op element
  ;

element
  : identifier
  | standard_type
  | call
  | T_OBRACKET expression T_CBRACKET
  ;

call
  : identifier T_OBRACKET call_arg_list T_CBRACKET
  ;

call_arg_list
  :
  | call_arg call_multi_arg
  ;

call_multi_arg
  :
  | T_COMMA call_arg call_multi_arg
  ;

call_arg
  : expression
  ;

add_op
  : T_PLUS
  | T_MINUS
  | T_OR
  ;

mul_op
  : T_MUL
  | T_DIV
  | T_AND
  ;

rel_op
  : T_CLT
  | T_CLE
  | T_CGT
  | T_CGE
  | T_CEQ
  | T_CNE
  ;

not_op
  : T_NOT
  ;

identifier
  : T_IDENTIFIER
  ;

standard_type
  : T_INT
  | T_CHAR
  ;
%%

int main() {
  return 0;
}
