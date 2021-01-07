// https://github.com/FANMixco/minipascal-simplified
#include <cstdio>
using namespace std;

---
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
---

int main() {
  return 0;
}
