// https://github.com/bleibig/rust-grammar
#include <cstdio>
using namespace std;

---
crate
  : maybe_shebang inner_attrs maybe_mod_items
  | maybe_shebang maybe_mod_items
  ;

maybe_shebang
  : SHEBANG_LINE
  |
  ;

maybe_inner_attrs
  : inner_attrs
  |
  ;

inner_attrs
  : inner_attr
  | inner_attrs inner_attr
  ;

inner_attr
  : SHEBANG '[' meta_item ']'
  | INNER_DOC_COMMENT
  ;

maybe_outer_attrs
  : outer_attrs
  |
  ;

outer_attrs
  : outer_attr
  | outer_attrs outer_attr
  ;

outer_attr
  : '#' '[' meta_item ']'
  | OUTER_DOC_COMMENT
  ;

meta_item
  : ident
  | ident '=' lit
  | ident '(' meta_seq ')'
  | ident '(' meta_seq ',' ')'
  ;

meta_seq
  :
  | meta_item
  | meta_seq ',' meta_item
  ;

maybe_mod_items
  : mod_items
  |
  ;

mod_items
  : mod_item
  | mod_items mod_item
  ;

attrs_and_vis
  : maybe_outer_attrs visibility
  ;

mod_item
  : attrs_and_vis item
  ;

item
  : stmt_item
  | item_macro
  ;

stmt_item
  : item_static
  | item_const
  | item_type
  | block_item
  | view_item
  ;

item_static
  : STATIC ident ':' ty '=' expr ';'
  | STATIC MUT ident ':' ty '=' expr ';'
  ;

item_const
  : CONST ident ':' ty '=' expr ';'
  ;

item_macro
  : path_expr '!' maybe_ident parens_delimited_token_trees ';'
  | path_expr '!' maybe_ident braces_delimited_token_trees
  | path_expr '!' maybe_ident brackets_delimited_token_trees ';'
  ;

view_item
  : use_item
  | extern_fn_item
  | EXTERN CRATE ident ';'
  | EXTERN CRATE ident AS ident ';'
  ;

extern_fn_item
  : EXTERN maybe_abi item_fn
  ;

use_item
  : USE view_path ';'
  ;

view_path
  : path_no_types_allowed
  | path_no_types_allowed MOD_SEP '{'                '}'
  |                       MOD_SEP '{'                '}'
  | path_no_types_allowed MOD_SEP '{' idents_or_self '}'
  |                       MOD_SEP '{' idents_or_self '}'
  | path_no_types_allowed MOD_SEP '{' idents_or_self ',' '}'
  |                       MOD_SEP '{' idents_or_self ',' '}'
  | path_no_types_allowed MOD_SEP '*'
  |                       MOD_SEP '*'
  |                               '*'
  |                               '{'                '}'
  |                               '{' idents_or_self '}'
  |                               '{' idents_or_self ',' '}'
  | path_no_types_allowed AS ident
  ;

block_item
  : item_fn
  | item_unsafe_fn
  | item_mod
  | item_foreign_mod
  | item_struct
  | item_enum
  | item_union
  | item_trait
  | item_impl
  ;

maybe_ty_ascription
  : ':' ty_sum
  |
  ;

maybe_init_expr
  : '=' expr
  |
  ;

item_struct
  : STRUCT ident generic_params maybe_where_clause struct_decl_args
  | STRUCT ident generic_params struct_tuple_args maybe_where_clause ';'
  | STRUCT ident generic_params maybe_where_clause ';'
  ;

struct_decl_args
  : '{' struct_decl_fields '}'
  | '{' struct_decl_fields ',' '}'
  ;

struct_tuple_args
  : '(' struct_tuple_fields ')'
  | '(' struct_tuple_fields ',' ')'
  ;

struct_decl_fields
  : struct_decl_field
  | struct_decl_fields ',' struct_decl_field
  |
  ;

struct_decl_field
  : attrs_and_vis ident ':' ty_sum
  ;

struct_tuple_fields
  : struct_tuple_field
  | struct_tuple_fields ',' struct_tuple_field
  |
  ;

struct_tuple_field
  : attrs_and_vis ty_sum
  ;

item_enum
  : ENUM ident generic_params maybe_where_clause '{' enum_defs '}'
  | ENUM ident generic_params maybe_where_clause '{' enum_defs ',' '}'
  ;

enum_defs
  : enum_def
  | enum_defs ',' enum_def
  |
  ;

enum_def
  : attrs_and_vis ident enum_args
  ;

enum_args
  : '{' struct_decl_fields '}'
  | '{' struct_decl_fields ',' '}'
  | '(' maybe_ty_sums ')'
  | '=' expr
  |
  ;

item_union
  : UNION ident generic_params maybe_where_clause '{' struct_decl_fields '}'
  | UNION ident generic_params maybe_where_clause '{' struct_decl_fields ',' '}'
  ;

item_mod
  : MOD ident ';'
  | MOD ident '{' maybe_mod_items '}'
  | MOD ident '{' inner_attrs maybe_mod_items '}'
  ;

item_foreign_mod
  : EXTERN maybe_abi '{' maybe_foreign_items '}'
  | EXTERN maybe_abi '{' inner_attrs maybe_foreign_items '}'
  ;

maybe_abi
  : str
  |
  ;

maybe_foreign_items
  : foreign_items
  |
  ;

foreign_items
  : foreign_item
  | foreign_items foreign_item
  ;

foreign_item
  : attrs_and_vis STATIC item_foreign_static
  | attrs_and_vis item_foreign_fn
  | attrs_and_vis UNSAFE item_foreign_fn
  ;

item_foreign_static
  : maybe_mut ident ':' ty ';'
  ;

item_foreign_fn
  : FN ident generic_params fn_decl_allow_variadic maybe_where_clause ';'
  ;

fn_decl_allow_variadic
  : fn_params_allow_variadic ret_ty
  ;

fn_params_allow_variadic
  : '(' ')'
  | '(' params ')'
  | '(' params ',' ')'
  | '(' params ',' DOTDOTDOT ')'
  ;

visibility
  : PUB
  |
  ;

idents_or_self
  : ident_or_self
  | idents_or_self AS ident
  | idents_or_self ',' ident_or_self
  ;

ident_or_self
  : ident
  | SELF
  ;

item_type
  : TYPE ident generic_params maybe_where_clause '=' ty_sum ';'
  ;

for_sized
  : FOR '?' ident
  | FOR ident '?'
  |
  ;

item_trait
  : maybe_unsafe TRAIT ident generic_params for_sized maybe_ty_param_bounds maybe_where_clause '{' maybe_trait_items '}'
  ;

maybe_trait_items
  : trait_items
  |
  ;

trait_items
  : trait_item
  | trait_items trait_item
  ;

trait_item
  : trait_const
  | trait_type
  | trait_method
  | maybe_outer_attrs item_macro
  ;

trait_const
  : maybe_outer_attrs CONST ident maybe_ty_ascription maybe_const_default ';'
  ;

maybe_const_default
  : '=' expr
  |
  ;

trait_type
  : maybe_outer_attrs TYPE ty_param ';'
  ;

maybe_unsafe
  : UNSAFE
  |
  ;

maybe_default_maybe_unsafe
  : DEFAULT UNSAFE
  | DEFAULT
  |         UNSAFE
  |
  ;

trait_method
  : type_method
  | method
  ;

type_method
  : maybe_outer_attrs maybe_unsafe FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause ';'
  | maybe_outer_attrs CONST maybe_unsafe FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause ';'
  | maybe_outer_attrs maybe_unsafe EXTERN maybe_abi FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause ';'
  ;

method
  : maybe_outer_attrs maybe_unsafe FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause inner_attrs_and_block
  | maybe_outer_attrs CONST maybe_unsafe FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause inner_attrs_and_block
  | maybe_outer_attrs maybe_unsafe EXTERN maybe_abi FN ident generic_params fn_decl_with_self_allow_anon_params maybe_where_clause inner_attrs_and_block
  ;

impl_method
  : attrs_and_vis maybe_default maybe_unsafe FN ident generic_params fn_decl_with_self maybe_where_clause inner_attrs_and_block
  | attrs_and_vis maybe_default CONST maybe_unsafe FN ident generic_params fn_decl_with_self maybe_where_clause inner_attrs_and_block
  | attrs_and_vis maybe_default maybe_unsafe EXTERN maybe_abi FN ident generic_params fn_decl_with_self maybe_where_clause inner_attrs_and_block
  ;

item_impl
  : maybe_default_maybe_unsafe IMPL generic_params ty_prim_sum maybe_where_clause '{' maybe_inner_attrs maybe_impl_items '}'
  | maybe_default_maybe_unsafe IMPL generic_params '(' ty ')' maybe_where_clause '{' maybe_inner_attrs maybe_impl_items '}'
  | maybe_default_maybe_unsafe IMPL generic_params trait_ref FOR ty_sum maybe_where_clause '{' maybe_inner_attrs maybe_impl_items '}'
  | maybe_default_maybe_unsafe IMPL generic_params '!' trait_ref FOR ty_sum maybe_where_clause '{' maybe_inner_attrs maybe_impl_items '}'
  | maybe_default_maybe_unsafe IMPL generic_params trait_ref FOR DOTDOT '{' '}'
  | maybe_default_maybe_unsafe IMPL generic_params '!' trait_ref FOR DOTDOT '{' '}'
  ;

maybe_impl_items
  : impl_items
  |
  ;

impl_items
  : impl_item
  | impl_item impl_items
  ;

impl_item
  : impl_method
  | attrs_and_vis item_macro
  | impl_const
  | impl_type
  ;

maybe_default
  : DEFAULT
  |
  ;

impl_const
  : attrs_and_vis maybe_default item_const
  ;

impl_type
  : attrs_and_vis maybe_default TYPE ident generic_params '=' ty_sum ';'
  ;

item_fn
  : FN ident generic_params fn_decl maybe_where_clause inner_attrs_and_block
  | CONST FN ident generic_params fn_decl maybe_where_clause inner_attrs_and_block
  ;

item_unsafe_fn
  : UNSAFE FN ident generic_params fn_decl maybe_where_clause inner_attrs_and_block
  | CONST UNSAFE FN ident generic_params fn_decl maybe_where_clause inner_attrs_and_block
  | UNSAFE EXTERN maybe_abi FN ident generic_params fn_decl maybe_where_clause inner_attrs_and_block
  ;

fn_decl
  : fn_params ret_ty
  ;

fn_decl_with_self
  : fn_params_with_self ret_ty
  ;

fn_decl_with_self_allow_anon_params
  : fn_anon_params_with_self ret_ty
  ;

fn_params
  : '(' maybe_params ')'
  ;

fn_anon_params
  : '(' anon_param anon_params_allow_variadic_tail ')'
  | '(' ')'
  ;

fn_params_with_self
  : '(' maybe_mut SELF maybe_ty_ascription maybe_comma_params ')'
  | '(' '&' maybe_mut SELF maybe_ty_ascription maybe_comma_params ')'
  | '(' '&' lifetime maybe_mut SELF maybe_ty_ascription maybe_comma_params ')'
  | '(' maybe_params ')'
  ;

fn_anon_params_with_self
  : '(' maybe_mut SELF maybe_ty_ascription maybe_comma_anon_params ')'
  | '(' '&' maybe_mut SELF maybe_ty_ascription maybe_comma_anon_params ')'
  | '(' '&' lifetime maybe_mut SELF maybe_ty_ascription maybe_comma_anon_params ')'
  | '(' maybe_anon_params ')'
  ;

maybe_params
  : params
  | params ','
  |
  ;

params
  : param
  | params ',' param
  ;

param
  : pat ':' ty_sum
  ;

inferrable_params
  : inferrable_param
  | inferrable_params ',' inferrable_param
  ;

inferrable_param
  : pat maybe_ty_ascription
  ;

maybe_comma_params
  : ','
  | ',' params
  | ',' params ','
  |
  ;

maybe_comma_anon_params
  : ','
  | ',' anon_params
  | ',' anon_params ','
  |
  ;

maybe_anon_params
  : anon_params
  | anon_params ','
  |
  ;

anon_params
  : anon_param
  | anon_params ',' anon_param
  ;

anon_param
  : named_arg ':' ty
  | ty
  ;

anon_params_allow_variadic_tail
  : ',' DOTDOTDOT
  | ',' anon_param anon_params_allow_variadic_tail
  |
  ;

named_arg
  : ident
  | UNDERSCORE
  | '&' ident
  | '&' UNDERSCORE
  | ANDAND ident
  | ANDAND UNDERSCORE
  | MUT ident
  ;

ret_ty
  : RARROW '!'
  | RARROW ty
  |
  ;

generic_params
  : '<' '>'
  | '<' lifetimes '>'
  | '<' lifetimes ',' '>'
  | '<' lifetimes SHR
  | '<' lifetimes ',' SHR
  | '<' lifetimes ',' ty_params '>'
  | '<' lifetimes ',' ty_params ',' '>'
  | '<' lifetimes ',' ty_params SHR
  | '<' lifetimes ',' ty_params ',' SHR
  | '<' ty_params '>'
  | '<' ty_params ',' '>'
  | '<' ty_params SHR
  | '<' ty_params ',' SHR
  |
  ;

maybe_where_clause
  :
  | where_clause
  ;

where_clause
  : WHERE where_predicates
  | WHERE where_predicates ','
  ;

where_predicates
  : where_predicate
  | where_predicates ',' where_predicate
  ;

where_predicate
  : maybe_for_lifetimes lifetime ':' bounds
  | maybe_for_lifetimes ty ':' ty_param_bounds
  ;

maybe_for_lifetimes
  : FOR '<' lifetimes '>'
  |
  ;

ty_params
  : ty_param
  | ty_params ',' ty_param
  ;

path_no_types_allowed
  : ident
  | MOD_SEP ident
  | SELF
  | MOD_SEP SELF
  | SUPER
  | MOD_SEP SUPER
  | path_no_types_allowed MOD_SEP ident
  ;

path_generic_args_without_colons
  : ident
  | ident generic_args
  | ident '(' maybe_ty_sums ')' ret_ty
  | path_generic_args_without_colons MOD_SEP ident
  | path_generic_args_without_colons MOD_SEP ident generic_args
  | path_generic_args_without_colons MOD_SEP ident '(' maybe_ty_sums ')' ret_ty
  ;

generic_args
  : '<' generic_values '>'
  | '<' generic_values SHR
  | '<' generic_values GE
  | '<' generic_values SHREQ
  | SHL ty_qualified_path_and_generic_values '>'
  | SHL ty_qualified_path_and_generic_values SHR
  | SHL ty_qualified_path_and_generic_values GE
  | SHL ty_qualified_path_and_generic_values SHREQ
  ;

generic_values
  : maybe_ty_sums_and_or_bindings
  ;

maybe_ty_sums_and_or_bindings
  : ty_sums
  | ty_sums ','
  | ty_sums ',' bindings
  | bindings
  | bindings ','
  |
  ;

maybe_bindings
  : ',' bindings
  |
  ;


pat
  : UNDERSCORE
  | '&' pat
  | '&' MUT pat
  | ANDAND pat
  | '(' ')'
  | '(' pat_tup ')'
  | '[' pat_vec ']'
  | lit_or_path
  | lit_or_path DOTDOTDOT lit_or_path
  | path_expr '{' pat_struct '}'
  | path_expr '(' ')'
  | path_expr '(' pat_tup ')'
  | path_expr '!' maybe_ident delimited_token_trees
  | binding_mode ident
  |              ident '@' pat
  | binding_mode ident '@' pat
  | BOX pat
  | '<' ty_sum maybe_as_trait_ref '>' MOD_SEP ident
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident maybe_as_trait_ref '>' MOD_SEP ident
  ;

pats_or
  : pat
  | pats_or '|' pat
  ;

binding_mode
  : REF
  | REF MUT
  | MUT
  ;

lit_or_path
  : path_expr
  | lit
  | '-' lit
  ;

pat_field
  :                  ident
  |     binding_mode ident
  | BOX              ident
  | BOX binding_mode ident
  |              ident ':' pat
  | binding_mode ident ':' pat
  |        LIT_INTEGER ':' pat
  ;

pat_fields
  : pat_field
  | pat_fields ',' pat_field
  ;

pat_struct
  : pat_fields
  | pat_fields ','
  | pat_fields ',' DOTDOT
  | DOTDOT
  |
  ;

pat_tup
  : pat_tup_elts
  | pat_tup_elts                             ','
  | pat_tup_elts     DOTDOT
  | pat_tup_elts ',' DOTDOT
  | pat_tup_elts     DOTDOT ',' pat_tup_elts
  | pat_tup_elts     DOTDOT ',' pat_tup_elts ','
  | pat_tup_elts ',' DOTDOT ',' pat_tup_elts
  | pat_tup_elts ',' DOTDOT ',' pat_tup_elts ','
  |                  DOTDOT ',' pat_tup_elts
  |                  DOTDOT ',' pat_tup_elts ','
  |                  DOTDOT
  ;

pat_tup_elts
  : pat
  | pat_tup_elts ',' pat
  ;

pat_vec
  : pat_vec_elts
  | pat_vec_elts                             ','
  | pat_vec_elts     DOTDOT
  | pat_vec_elts ',' DOTDOT
  | pat_vec_elts     DOTDOT ',' pat_vec_elts
  | pat_vec_elts     DOTDOT ',' pat_vec_elts ','
  | pat_vec_elts ',' DOTDOT ',' pat_vec_elts
  | pat_vec_elts ',' DOTDOT ',' pat_vec_elts ','
  |                  DOTDOT ',' pat_vec_elts
  |                  DOTDOT ',' pat_vec_elts ','
  |                  DOTDOT
  |
  ;

pat_vec_elts
  : pat
  | pat_vec_elts ',' pat
  ;

ty
  : ty_prim
  | ty_closure
  | '<' ty_sum maybe_as_trait_ref '>' MOD_SEP ident
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident maybe_as_trait_ref '>' MOD_SEP ident
  | '(' ty_sums ')'
  | '(' ty_sums ',' ')'
  | '(' ')'
  ;

ty_prim
  : path_generic_args_without_colons
  | MOD_SEP path_generic_args_without_colons
  | SELF MOD_SEP path_generic_args_without_colons
  | path_generic_args_without_colons '!' maybe_ident delimited_token_trees
  | MOD_SEP path_generic_args_without_colons '!' maybe_ident delimited_token_trees
  | BOX ty
  | '*' maybe_mut_or_const ty
  | '&' ty
  | '&' MUT ty
  | ANDAND ty
  | ANDAND MUT ty
  | '&' lifetime maybe_mut ty
  | ANDAND lifetime maybe_mut ty
  | '[' ty ']'
  | '[' ty ',' DOTDOT expr ']'
  | '[' ty ';' expr ']'
  | TYPEOF '(' expr ')'
  | UNDERSCORE
  | ty_bare_fn
  | for_in_type
  ;

ty_bare_fn
  :                         FN ty_fn_decl
  | UNSAFE                  FN ty_fn_decl
  |        EXTERN maybe_abi FN ty_fn_decl
  | UNSAFE EXTERN maybe_abi FN ty_fn_decl
  ;

ty_fn_decl
  : generic_params fn_anon_params ret_ty
  ;

ty_closure
  : UNSAFE '|' anon_params '|' maybe_bounds ret_ty
  |        '|' anon_params '|' maybe_bounds ret_ty
  | UNSAFE OROR maybe_bounds ret_ty
  |        OROR maybe_bounds ret_ty
  ;

for_in_type
  : FOR '<' maybe_lifetimes '>' for_in_type_suffix
  ;

for_in_type_suffix
  : ty_bare_fn
  | trait_ref
  | ty_closure
  ;

maybe_mut
  : MUT
  |
  ;

maybe_mut_or_const
  : MUT
  | CONST
  |
  ;

ty_qualified_path_and_generic_values
  : ty_qualified_path maybe_bindings
  | ty_qualified_path ',' ty_sums maybe_bindings
  ;

ty_qualified_path
  : ty_sum AS trait_ref '>' MOD_SEP ident
  | ty_sum AS trait_ref '>' MOD_SEP ident '+' ty_param_bounds
  ;

maybe_ty_sums
  : ty_sums
  | ty_sums ','
  |
  ;

ty_sums
  : ty_sum
  | ty_sums ',' ty_sum
  ;

ty_sum
  : ty_sum_elt
  | ty_sum '+' ty_sum_elt
  ;

ty_sum_elt
  : ty
  | lifetime
  ;

ty_prim_sum
  : ty_prim_sum_elt
  | ty_prim_sum '+' ty_prim_sum_elt
  ;

ty_prim_sum_elt
  : ty_prim
  | lifetime
  ;

maybe_ty_param_bounds
  : ':' ty_param_bounds
  |
  ;

ty_param_bounds
  : boundseq
  |
  ;

boundseq
  : polybound
  | boundseq '+' polybound
  ;

polybound
  : FOR '<' maybe_lifetimes '>' bound
  | bound
  | '?' FOR '<' maybe_lifetimes '>' bound
  | '?' bound
  ;

bindings
  : binding
  | bindings ',' binding
  ;

binding
  : ident '=' ty
  ;

ty_param
  : ident maybe_ty_param_bounds maybe_ty_default
  | ident '?' ident maybe_ty_param_bounds maybe_ty_default
  ;

maybe_bounds
  : ':' bounds
  |
  ;

bounds
  : bound
  | bounds '+' bound
  ;

bound
  : lifetime
  | trait_ref
  ;

maybe_ltbounds
  : ':' ltbounds
  |
  ;

ltbounds
  : lifetime
  | ltbounds '+' lifetime
  ;

maybe_ty_default
  : '=' ty_sum
  |
  ;

maybe_lifetimes
  : lifetimes
  | lifetimes ','
  |
  ;

lifetimes
  : lifetime_and_bounds
  | lifetimes ',' lifetime_and_bounds
  ;

lifetime_and_bounds
  : LIFETIME maybe_ltbounds
  | STATIC_LIFETIME
  ;

lifetime
  : LIFETIME
  | STATIC_LIFETIME
  ;

trait_ref
  : path_generic_args_without_colons
  | MOD_SEP path_generic_args_without_colons
  ;

inner_attrs_and_block
  : '{' maybe_inner_attrs maybe_stmts '}'
  ;

block
  : '{' maybe_stmts '}'
  ;

maybe_stmts
  : stmts
  | stmts nonblock_expr
  | nonblock_expr
  |
  ;

stmts
  : stmt
  | stmts stmt
  ;

stmt
  : maybe_outer_attrs let
  |                 stmt_item
  |             PUB stmt_item
  | outer_attrs     stmt_item
  | outer_attrs PUB stmt_item
  | full_block_expr
  | maybe_outer_attrs block
  |             nonblock_expr ';'
  | outer_attrs nonblock_expr ';'
  | ';'
  ;

maybe_exprs
  : exprs
  | exprs ','
  |
  ;

maybe_expr
  : expr
  |
  ;

exprs
  : expr
  | exprs ',' expr
  ;

path_expr
  : path_generic_args_with_colons
  | MOD_SEP path_generic_args_with_colons
  | SELF MOD_SEP path_generic_args_with_colons
  ;

path_generic_args_with_colons
  : ident
  | SUPER
  | path_generic_args_with_colons MOD_SEP ident
  | path_generic_args_with_colons MOD_SEP SUPER
  | path_generic_args_with_colons MOD_SEP generic_args
  ;

macro_expr
  : path_expr '!' maybe_ident parens_delimited_token_trees
  | path_expr '!' maybe_ident brackets_delimited_token_trees
  ;

nonblock_expr
  : lit
  | path_expr
  | SELF
  | macro_expr
  | path_expr '{' struct_expr_fields '}'
  | nonblock_expr '?'
  | nonblock_expr '.' path_generic_args_with_colons
  | nonblock_expr '.' LIT_INTEGER
  | nonblock_expr '[' maybe_expr ']'
  | nonblock_expr '(' maybe_exprs ')'
  | '[' vec_expr ']'
  | '(' maybe_exprs ')'
  | CONTINUE
  | CONTINUE lifetime
  | RETURN
  | RETURN expr
  | BREAK
  | BREAK lifetime
  | YIELD
  | YIELD expr
  | nonblock_expr LARROW expr
  | nonblock_expr '=' expr
  | nonblock_expr SHLEQ expr
  | nonblock_expr SHREQ expr
  | nonblock_expr MINUSEQ expr
  | nonblock_expr ANDEQ expr
  | nonblock_expr OREQ expr
  | nonblock_expr PLUSEQ expr
  | nonblock_expr STAREQ expr
  | nonblock_expr SLASHEQ expr
  | nonblock_expr CARETEQ expr
  | nonblock_expr PERCENTEQ expr
  | nonblock_expr OROR expr
  | nonblock_expr ANDAND expr
  | nonblock_expr EQEQ expr
  | nonblock_expr NE expr
  | nonblock_expr '<' expr
  | nonblock_expr '>' expr
  | nonblock_expr LE expr
  | nonblock_expr GE expr
  | nonblock_expr '|' expr
  | nonblock_expr '^' expr
  | nonblock_expr '&' expr
  | nonblock_expr SHL expr
  | nonblock_expr SHR expr
  | nonblock_expr '+' expr
  | nonblock_expr '-' expr
  | nonblock_expr '*' expr
  | nonblock_expr '/' expr
  | nonblock_expr '%' expr
  | nonblock_expr DOTDOT
  | nonblock_expr DOTDOT expr
  |               DOTDOT expr
  |               DOTDOT
  | nonblock_expr AS ty
  | nonblock_expr ':' ty
  | BOX expr
  | expr_qualified_path
  | nonblock_prefix_expr
  ;

expr
  : lit
  | path_expr
  | SELF
  | macro_expr
  | path_expr '{' struct_expr_fields '}'
  | expr '?'
  | expr '.' path_generic_args_with_colons
  | expr '.' LIT_INTEGER
  | expr '[' maybe_expr ']'
  | expr '(' maybe_exprs ')'
  | '(' maybe_exprs ')'
  | '[' vec_expr ']'
  | CONTINUE
  | CONTINUE ident
  | RETURN
  | RETURN expr
  | BREAK
  | BREAK ident
  | YIELD
  | YIELD expr
  | expr LARROW expr
  | expr '=' expr
  | expr SHLEQ expr
  | expr SHREQ expr
  | expr MINUSEQ expr
  | expr ANDEQ expr
  | expr OREQ expr
  | expr PLUSEQ expr
  | expr STAREQ expr
  | expr SLASHEQ expr
  | expr CARETEQ expr
  | expr PERCENTEQ expr
  | expr OROR expr
  | expr ANDAND expr
  | expr EQEQ expr
  | expr NE expr
  | expr '<' expr
  | expr '>' expr
  | expr LE expr
  | expr GE expr
  | expr '|' expr
  | expr '^' expr
  | expr '&' expr
  | expr SHL expr
  | expr SHR expr
  | expr '+' expr
  | expr '-' expr
  | expr '*' expr
  | expr '/' expr
  | expr '%' expr
  | expr DOTDOT
  | expr DOTDOT expr
  |      DOTDOT expr
  |      DOTDOT
  | expr AS ty
  | expr ':' ty
  | BOX expr
  | expr_qualified_path
  | block_expr
  | block
  | nonblock_prefix_expr
  ;

expr_nostruct
  : lit
  | path_expr
  | SELF
  | macro_expr
  | expr_nostruct '?'
  | expr_nostruct '.' path_generic_args_with_colons
  | expr_nostruct '.' LIT_INTEGER
  | expr_nostruct '[' maybe_expr ']'
  | expr_nostruct '(' maybe_exprs ')'
  | '[' vec_expr ']'
  | '(' maybe_exprs ')'
  | CONTINUE
  | CONTINUE ident
  | RETURN
  | RETURN expr
  | BREAK
  | BREAK ident
  | YIELD
  | YIELD expr
  | expr_nostruct LARROW expr_nostruct
  | expr_nostruct '=' expr_nostruct
  | expr_nostruct SHLEQ expr_nostruct
  | expr_nostruct SHREQ expr_nostruct
  | expr_nostruct MINUSEQ expr_nostruct
  | expr_nostruct ANDEQ expr_nostruct
  | expr_nostruct OREQ expr_nostruct
  | expr_nostruct PLUSEQ expr_nostruct
  | expr_nostruct STAREQ expr_nostruct
  | expr_nostruct SLASHEQ expr_nostruct
  | expr_nostruct CARETEQ expr_nostruct
  | expr_nostruct PERCENTEQ expr_nostruct
  | expr_nostruct OROR expr_nostruct
  | expr_nostruct ANDAND expr_nostruct
  | expr_nostruct EQEQ expr_nostruct
  | expr_nostruct NE expr_nostruct
  | expr_nostruct '<' expr_nostruct
  | expr_nostruct '>' expr_nostruct
  | expr_nostruct LE expr_nostruct
  | expr_nostruct GE expr_nostruct
  | expr_nostruct '|' expr_nostruct
  | expr_nostruct '^' expr_nostruct
  | expr_nostruct '&' expr_nostruct
  | expr_nostruct SHL expr_nostruct
  | expr_nostruct SHR expr_nostruct
  | expr_nostruct '+' expr_nostruct
  | expr_nostruct '-' expr_nostruct
  | expr_nostruct '*' expr_nostruct
  | expr_nostruct '/' expr_nostruct
  | expr_nostruct '%' expr_nostruct
  | expr_nostruct DOTDOT
  | expr_nostruct DOTDOT expr_nostruct
  |               DOTDOT expr_nostruct
  |               DOTDOT
  | expr_nostruct AS ty
  | expr_nostruct ':' ty
  | BOX expr
  | expr_qualified_path
  | block_expr
  | block
  | nonblock_prefix_expr_nostruct
  ;

nonblock_prefix_expr_nostruct
  : '-' expr_nostruct
  | '!' expr_nostruct
  | '*' expr_nostruct
  | '&' maybe_mut expr_nostruct
  | ANDAND maybe_mut expr_nostruct
  | lambda_expr_nostruct
  | MOVE lambda_expr_nostruct
  ;

nonblock_prefix_expr
  : '-' expr
  | '!' expr
  | '*' expr
  | '&' maybe_mut expr
  | ANDAND maybe_mut expr
  | lambda_expr
  | MOVE lambda_expr
  ;

expr_qualified_path
  : '<' ty_sum maybe_as_trait_ref '>' MOD_SEP ident maybe_qpath_params
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident maybe_as_trait_ref '>' MOD_SEP ident
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident generic_args maybe_as_trait_ref '>' MOD_SEP ident
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident maybe_as_trait_ref '>' MOD_SEP ident generic_args
  | SHL ty_sum maybe_as_trait_ref '>' MOD_SEP ident generic_args maybe_as_trait_ref '>' MOD_SEP ident generic_args
  ;

maybe_qpath_params
  : MOD_SEP generic_args
  |
  ;

maybe_as_trait_ref
  : AS trait_ref
  |
  ;

lambda_expr
  : OROR ret_ty expr
  | '|' '|' ret_ty expr
  | '|' inferrable_params '|' ret_ty expr
  | '|' inferrable_params OROR lambda_expr_no_first_bar
  ;

lambda_expr_no_first_bar
  : '|' ret_ty expr
  | inferrable_params '|' ret_ty expr
  | inferrable_params OROR lambda_expr_no_first_bar
  ;

lambda_expr_nostruct
  : OROR expr_nostruct
  | '|' '|' ret_ty expr_nostruct
  | '|' inferrable_params '|' expr_nostruct
  | '|' inferrable_params OROR lambda_expr_nostruct_no_first_bar
  ;

lambda_expr_nostruct_no_first_bar
  : '|' ret_ty expr_nostruct
  | inferrable_params '|' ret_ty expr_nostruct
  | inferrable_params OROR lambda_expr_nostruct_no_first_bar
  ;

vec_expr
  : maybe_exprs
  | exprs ';' expr
  ;

struct_expr_fields
  : field_inits
  | field_inits ','
  | maybe_field_inits default_field_init
  |
  ;

maybe_field_inits
  : field_inits
  | field_inits ','
  |
  ;

field_inits
  : field_init
  | field_inits ',' field_init
  ;

field_init
  : ident
  | ident ':' expr
  | LIT_INTEGER ':' expr
  ;

default_field_init
  : DOTDOT expr
  ;

block_expr
  : expr_match
  | expr_if
  | expr_if_let
  | expr_while
  | expr_while_let
  | expr_loop
  | expr_for
  | UNSAFE block
  | path_expr '!' maybe_ident braces_delimited_token_trees
  ;

full_block_expr
  : block_expr
  | block_expr_dot
  ;

block_expr_dot
  : block_expr     '.' path_generic_args_with_colons
  | block_expr_dot '.' path_generic_args_with_colons
  | block_expr     '.' path_generic_args_with_colons '[' maybe_expr ']'
  | block_expr_dot '.' path_generic_args_with_colons '[' maybe_expr ']'
  | block_expr     '.' path_generic_args_with_colons '(' maybe_exprs ')'
  | block_expr_dot '.' path_generic_args_with_colons '(' maybe_exprs ')'
  | block_expr     '.' LIT_INTEGER
  | block_expr_dot '.' LIT_INTEGER
  ;

expr_match
  : MATCH expr_nostruct '{' '}'
  | MATCH expr_nostruct '{' match_clauses                       '}'
  | MATCH expr_nostruct '{' match_clauses nonblock_match_clause '}'
  | MATCH expr_nostruct '{'               nonblock_match_clause '}'
  ;

match_clauses
  : match_clause
  | match_clauses match_clause
  ;

match_clause
  : nonblock_match_clause ','
  | block_match_clause
  | block_match_clause ','
  ;

nonblock_match_clause
  : maybe_outer_attrs pats_or maybe_guard FAT_ARROW nonblock_expr
  | maybe_outer_attrs pats_or maybe_guard FAT_ARROW block_expr_dot
  ;

block_match_clause
  : maybe_outer_attrs pats_or maybe_guard FAT_ARROW block
  | maybe_outer_attrs pats_or maybe_guard FAT_ARROW block_expr
  ;

maybe_guard
  : IF expr_nostruct
  |
  ;

expr_if
  : IF expr_nostruct block
  | IF expr_nostruct block ELSE block_or_if
  ;

expr_if_let
  : IF LET pat '=' expr_nostruct block
  | IF LET pat '=' expr_nostruct block ELSE block_or_if
  ;

block_or_if
  : block
  | expr_if
  | expr_if_let
  ;

expr_while
  : maybe_label WHILE expr_nostruct block
  ;

expr_while_let
  : maybe_label WHILE LET pat '=' expr_nostruct block
  ;

expr_loop
  : maybe_label LOOP block
  ;

expr_for
  : maybe_label FOR pat IN expr_nostruct block
  ;

maybe_label
  : lifetime ':'
  |
  ;

let
  : LET pat maybe_ty_ascription maybe_init_expr ';'
  ;

lit
  : LIT_BYTE
  | LIT_CHAR
  | LIT_INTEGER
  | LIT_FLOAT
  | TRUE
  | FALSE
  | str
  ;

str
  : LIT_STR
  | LIT_STR_RAW
  | LIT_BYTE_STR
  | LIT_BYTE_STR_RAW
  ;

maybe_ident
  :
  | ident
  ;

ident
  : IDENT
  | CATCH
  | DEFAULT
  | UNION
  ;

unpaired_token
  : SHL
  | SHR
  | LE
  | EQEQ
  | NE
  | GE
  | ANDAND
  | OROR
  | LARROW
  | SHLEQ
  | SHREQ
  | MINUSEQ
  | ANDEQ
  | OREQ
  | PLUSEQ
  | STAREQ
  | SLASHEQ
  | CARETEQ
  | PERCENTEQ
  | DOTDOT
  | DOTDOTDOT
  | MOD_SEP
  | RARROW
  | FAT_ARROW
  | LIT_BYTE
  | LIT_CHAR
  | LIT_INTEGER
  | LIT_FLOAT
  | LIT_STR
  | LIT_STR_RAW
  | LIT_BYTE_STR
  | LIT_BYTE_STR_RAW
  | IDENT
  | UNDERSCORE
  | LIFETIME
  | SELF
  | STATIC
  | ABSTRACT
  | ALIGNOF
  | AS
  | BECOME
  | BREAK
  | CATCH
  | CRATE
  | DEFAULT
  | DO
  | ELSE
  | ENUM
  | EXTERN
  | FALSE
  | FINAL
  | FN
  | FOR
  | IF
  | IMPL
  | IN
  | LET
  | LOOP
  | MACRO
  | MATCH
  | MOD
  | MOVE
  | MUT
  | OFFSETOF
  | OVERRIDE
  | PRIV
  | PUB
  | PURE
  | REF
  | RETURN
  | STRUCT
  | SIZEOF
  | SUPER
  | TRUE
  | TRAIT
  | TYPE
  | UNION
  | UNSAFE
  | UNSIZED
  | USE
  | VIRTUAL
  | WHILE
  | YIELD
  | CONTINUE
  | PROC
  | BOX
  | CONST
  | WHERE
  | TYPEOF
  | INNER_DOC_COMMENT
  | OUTER_DOC_COMMENT
  | SHEBANG
  | STATIC_LIFETIME
  | ';'
  | ','
  | '.'
  | '@'
  | '#'
  | '~'
  | ':'
  | '$'
  | '='
  | '?'
  | '!'
  | '<'
  | '>'
  | '-'
  | '&'
  | '|'
  | '+'
  | '*'
  | '/'
  | '^'
  | '%'
  ;

token_trees
  :
  | token_trees token_tree
  ;

token_tree
  : delimited_token_trees
  | unpaired_token
  ;

delimited_token_trees
  : parens_delimited_token_trees
  | braces_delimited_token_trees
  | brackets_delimited_token_trees
  ;

parens_delimited_token_trees
  : '(' token_trees ')'
  ;

braces_delimited_token_trees
  : '{' token_trees '}'
  ;

brackets_delimited_token_trees
  : '[' token_trees ']'
  ;
---

int main() {

}
