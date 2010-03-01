# -*- coding: iso-8859-15 -*-
#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
import string
from mcrl2_utility import *

TOKEN_PREFIX = r'''%%}
Id         [a-zA-Z\\_][a-zA-Z0-9\\_']*
Number     "0"|([1-9][0-9]*)

%%option c++
%%option prefix="mcrl3yy"
%%option nounput

%%%%

[ \\t]      { col_nr += YYLeng(); /* whitespace */ }

\\r?\\n      { line_nr++; col_nr = 1; /* newline */ }

"%%".*      { col_nr += YYLeng(); /* comment */ }

'''

TOKEN_POSTFIX = '''.          {
             col_nr += YYLeng(); yyerror("unknown character");
             /* remaining characters */
     }

%%%%
'''

TAGS = '''
identifier
sort_expr
data_expr
data_spec
mult_act
proc_expr
proc_spec
state_frm
action_rename
pbes_spec
data_vars
'''

TOKENS = '''
"||_"    | LMERGE
"->"     | ARROW
"<="     | LTE
">="     | GTE
"|>"     | CONS
"<|"     | SNOC
"++"     | CONCAT
"=="     | EQ
"!="     | NEQ
"&&"     | AND
"||"     | BARS
"=>"     | IMP
"<<"     | BINIT
"<>"     | ELSE
"/"      | SLASH
"*"      | STAR
"+"      | PLUS
"-"      | MINUS
"="      | EQUALS
"."      | DOT
","      | COMMA
":"      | COLON
";"      | SEMICOLON
"?"      | QMARK
"!"      | EXCLAM
"@"      | AT
"#"      | HASH
"|"      | BAR
"("      | LPAR
")"      | RPAR
"["      | LBRACK
"]"      | RBRACK
"<"      | LANG
">"      | RANG
"{"      | LBRACE
"}"      | RBRACE
sort     | KWSORT
cons     | KWCONS
map      | KWMAP
var      | KWVAR
eqn      | KWEQN
act      | KWACT
glob     | KWGLOB
proc     | KWPROC
pbes     | KWPBES
init     | KWINIT
struct   | KWSTRUCT
Bool     | BOOL
Pos      | POS
Nat      | NAT
Int      | INT
Real     | REAL
List     | LIST
Set      | SET
Bag      | BAG
true     | CTRUE
false    | CFALSE
if       | IF
div      | DIV
mod      | MOD
in       | IN
lambda   | LAMBDA
forall   | FORALL
exists   | EXISTS
whr      | WHR
end      | END
delta    | DELTA
tau      | TAU
sum      | SUM
block    | BLOCK
allow    | ALLOW
hide     | HIDE
rename   | RENAME
comm     | COMM
val      | VAL
mu       | MU
nu       | NU
delay    | DELAY
yaled    | YALED
nil      | NIL
{Id}     | ID
{Number} | NUMBER
'''

NONTERMINAL_TERMS = '''
start
'''

NONTERMINAL_FUNCTION_APPLICATIONS = '''
sort_expr
sort_expr_arrow
domain_no_arrow_elt
sort_expr_struct
struct_constructor
recogniser
struct_projection
sort_expr_primary
sort_constant
sort_constructor
data_expr
data_expr_whr
id_init
data_expr_quant
data_expr_imp
data_expr_imp_rhs
data_expr_and
data_expr_and_rhs
data_expr_eq
data_expr_eq_rhs
data_expr_rel
data_expr_cons
data_expr_snoc
data_expr_concat
data_expr_add
data_expr_div
data_expr_mult
data_expr_prefix
data_expr_quant_prefix
data_expr_postfix
data_expr_primary
data_constant
data_enumeration
data_comprehension
data_var_decl
data_spec
data_spec_elt
sort_spec
cons_spec
map_spec
data_eqn_spec
data_eqn_decl
mult_act
param_id
proc_expr
proc_expr_choice
proc_expr_sum
proc_expr_merge
proc_expr_merge_rhs
proc_expr_binit
proc_expr_binit_rhs
proc_expr_cond
proc_expr_cond_la
proc_expr_seq
proc_expr_seq_wo_cond
proc_expr_seq_rhs
proc_expr_seq_rhs_wo_cond
proc_expr_at
proc_expr_at_wo_cond
proc_expr_sync
proc_expr_sync_wo_cond
proc_expr_sync_rhs
proc_expr_sync_rhs_wo_cond
proc_expr_primary
proc_constant
id_assignment
proc_quant
ren_expr
comm_expr
comm_expr_lhs
mult_act_name
proc_spec
proc_spec_elt
act_spec
glob_var_spec
proc_eqn_spec
proc_eqn_decl
proc_init
state_frm
state_frm_quant
state_frm_imp
state_frm_imp_rhs
state_frm_and
state_frm_and_rhs
state_frm_prefix
state_frm_quant_prefix
state_frm_primary
data_var_decl_init
reg_frm
reg_frm_alt_naf
reg_frm_alt
reg_frm_seq_naf
reg_frm_seq
reg_frm_postfix_naf
reg_frm_postfix
reg_frm_primary_naf
reg_frm_primary
act_frm
act_frm_quant
act_frm_imp
act_frm_imp_rhs
act_frm_and
act_frm_and_rhs
act_frm_at
act_frm_prefix
act_frm_quant_prefix
act_frm_primary
action_rename_spec
action_rename_spec_elt
action_rename_rule_spec
action_rename_rule
action_rename_rule_rhs
pb_expr
pb_expr_quant
pb_expr_imp
pb_expr_imp_rhs
pb_expr_and
pb_expr_and_rhs
pb_expr_not
pb_expr_quant_not
pb_expr_primary
pbes_spec
pbes_spec_elt
pb_eqn_spec
pb_eqn_decl
fixpoint
pb_init
'''

NONTERMINAL_LISTS = '''
domain_no_arrow
domain_no_arrow_elts_hs
struct_constructors_bs
struct_projections_cs
bag_enum_elt
id_inits_cs
data_exprs_cs
bag_enum_elts_cs
data_vars_decls_cs
data_vars_decl
data_spec_elts
ids_cs
sorts_decls_scs
sorts_decl
domain
ops_decls_scs
ops_decl
data_eqn_sect
data_eqn_decls_scs
data_vars_decls_scs
act_names_set
ren_expr_set
ren_exprs_cs
comm_expr_set
comm_exprs_cs
mult_act_names_set
mult_act_names_cs
ids_bs
param_ids_bs
proc_spec_elts
acts_decls_scs
acts_decl
proc_eqn_decls_scs
fixpoint_params
data_var_decl_inits_cs
action_rename_spec_elts
action_rename_rules_scs
action_rename_rule_sect
pbes_spec_elts
pb_eqn_decls_scs
'''

START_GRAMMAR = '''
start:
    TAG_IDENTIFIER ID
  | TAG_SORT_EXPR sort_expr
  | TAG_DATA_EXPR data_expr
  | TAG_DATA_SPEC data_spec
  | TAG_MULT_ACT mult_act
  | TAG_PROC_EXPR proc_expr
  | TAG_PROC_SPEC proc_spec
  | TAG_STATE_FRM state_frm
  | TAG_ACTION_RENAME action_rename_spec
  | TAG_PBES_SPEC pbes_spec
  | TAG_DATA_VARS data_vars_decls_scs
  ;
'''

SORT_EXPRESSION_GRAMMAR = '''
sort_expr:
  sort_expr_arrow
  ;

sort_expr_arrow:
    sort_expr_struct
  | domain_no_arrow ARROW sort_expr_arrow
  ;

domain_no_arrow:
  domain_no_arrow_elts_hs
  ;

domain_no_arrow_elts_hs:
    domain_no_arrow_elt
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
  ;

domain_no_arrow_elt:
  sort_expr_struct
  ;

sort_expr_struct:
    sort_expr_primary
  | KWSTRUCT struct_constructors_bs
  ;

struct_constructors_bs:
    struct_constructor
  | struct_constructors_bs BAR struct_constructor
  ;

struct_constructor:
    ID recogniser
  | ID LPAR struct_projections_cs RPAR recogniser
  ;

recogniser:
    /* empty */
  | QMARK ID
  ;

struct_projections_cs:
    struct_projection
  | struct_projections_cs COMMA struct_projection
  ;

struct_projection:
    sort_expr
  | ID COLON sort_expr
  ;

sort_expr_primary:
    ID
  | sort_constant
  | sort_constructor
  | LPAR sort_expr RPAR
  ;

sort_constant:
    BOOL
  | POS
  | NAT
  | INT
  | REAL
  ;

sort_constructor:
    LIST LPAR sort_expr RPAR
  | SET LPAR sort_expr RPAR
  | BAG LPAR sort_expr RPAR
  ;
'''

DATA_EXPRESSION_GRAMMAR = '''
data_expr:
  data_expr_whr
  ;

data_expr_whr:
    data_expr_quant
  | data_expr_whr WHR id_inits_cs END
  ;

id_inits_cs:
    id_init
  | id_inits_cs COMMA id_init
  ;

id_init:
  ID EQUALS data_expr
  ;

data_expr_quant:
    data_expr_imp
  | LAMBDA data_vars_decls_cs DOT data_expr_quant
  | FORALL data_vars_decls_cs DOT data_expr_quant
  | EXISTS data_vars_decls_cs DOT data_expr_quant
  ;

data_vars_decls_cs:
    data_vars_decl
  | data_vars_decls_cs COMMA data_vars_decl
  ;

data_vars_decl:
  ids_cs COLON sort_expr
  ;

data_expr_imp:
    data_expr_and
  | data_expr_and IMP data_expr_imp_rhs
  ;

data_expr_imp_rhs:
    data_expr_imp
  | FORALL data_vars_decls_cs DOT data_expr_imp_rhs
  | EXISTS data_vars_decls_cs DOT data_expr_imp_rhs
  ;

data_expr_and:
    data_expr_eq
  | data_expr_eq AND data_expr_and_rhs
  | data_expr_eq BARS data_expr_and_rhs
  ;

data_expr_and_rhs:
    data_expr_and
  | FORALL data_vars_decls_cs DOT data_expr_and_rhs
  | EXISTS data_vars_decls_cs DOT data_expr_and_rhs
  ;

data_expr_eq:
    data_expr_rel
  | data_expr_rel EQ data_expr_eq_rhs
  | data_expr_rel NEQ data_expr_eq_rhs
  ;

data_expr_eq_rhs:
    data_expr_eq
  | LAMBDA data_vars_decls_cs DOT data_expr_eq_rhs
  | FORALL data_vars_decls_cs DOT data_expr_eq_rhs
  | EXISTS data_vars_decls_cs DOT data_expr_eq_rhs
  ;

data_expr_rel:
    data_expr_cons
  | data_expr_cons GTE data_expr_cons
  | data_expr_cons LTE data_expr_cons
  | data_expr_cons RANG data_expr_cons
  | data_expr_cons LANG data_expr_cons
  | data_expr_cons IN data_expr_cons
  ;

data_expr_cons:
    data_expr_snoc
  | data_expr_add CONS data_expr_cons
  ;

data_expr_snoc:
    data_expr_concat
  | data_expr_snoc SNOC data_expr_add
  ;

data_expr_concat:
    data_expr_add
  | data_expr_concat CONCAT data_expr_add
  ;

data_expr_add:
    data_expr_div
  | data_expr_add PLUS data_expr_div
  | data_expr_add MINUS data_expr_div
  ;

data_expr_div:
    data_expr_mult
  | data_expr_div DIV data_expr_mult
  | data_expr_div MOD data_expr_mult
  | data_expr_div SLASH data_expr_mult
  ;

data_expr_mult:
    data_expr_prefix
  | data_expr_mult STAR data_expr_prefix
  | data_expr_mult DOT data_expr_prefix
  ;

data_expr_prefix:
    data_expr_postfix
  | EXCLAM data_expr_quant_prefix
  | MINUS data_expr_prefix
  | HASH data_expr_prefix
  ;

data_expr_quant_prefix:
    data_expr_prefix
  | FORALL data_vars_decls_cs DOT data_expr_quant_prefix
  | EXISTS data_vars_decls_cs DOT data_expr_quant_prefix
  ;

data_expr_postfix:
    data_expr_primary
  | data_expr_postfix LPAR data_exprs_cs RPAR
  ;

data_exprs_cs:
    data_expr
  | data_exprs_cs COMMA data_expr
  ;

data_expr_primary:
    ID
  | data_constant
  | data_enumeration
  | data_comprehension
  | LPAR data_expr RPAR
  ;

data_constant:
    CTRUE
  | CFALSE
  | IF
  | NUMBER
  | LBRACK RBRACK
  | LBRACE RBRACE
  ;

data_enumeration:
    LBRACK data_exprs_cs RBRACK
  | LBRACE data_exprs_cs RBRACE
  | LBRACE bag_enum_elts_cs RBRACE
  ;

bag_enum_elts_cs:
    bag_enum_elt
  | bag_enum_elts_cs COMMA bag_enum_elt
  ;

bag_enum_elt:
  data_expr COLON data_expr
  ;

data_comprehension:
  LBRACE data_var_decl BAR data_expr RBRACE
  ;

data_var_decl:
  ID COLON sort_expr
  ;
'''

DATA_SPECIFICATION_GRAMMAR = '''
data_spec:
  data_spec_elts
  ;

data_spec_elts:
    data_spec_elt
   | data_spec_elts data_spec_elt
   ;

data_spec_elt:
    sort_spec
  | cons_spec
  | map_spec
  | data_eqn_spec
  ;

sort_spec:
  KWSORT sorts_decls_scs
  ;

sorts_decls_scs:
    sorts_decl SEMICOLON
  | sorts_decls_scs sorts_decl SEMICOLON
  ;

sorts_decl:
    ids_cs
  | ID EQUALS sort_expr
  ;

ids_cs:
    ID
  | ids_cs COMMA ID
  ;

domain:
    domain_no_arrow
  | domain_no_arrow ARROW sort_expr
  ;

cons_spec:
  KWCONS ops_decls_scs
  ;

map_spec:
  KWMAP ops_decls_scs
  ;

ops_decls_scs:
    ops_decl SEMICOLON
  | ops_decls_scs ops_decl SEMICOLON
  ;

ops_decl:
  ids_cs COLON sort_expr
  ;

data_eqn_spec:
  data_eqn_sect
  ;

data_eqn_sect:
    KWEQN data_eqn_decls_scs
  | KWVAR data_vars_decls_scs KWEQN data_eqn_decls_scs
  ;

data_eqn_decls_scs:
    data_eqn_decl SEMICOLON
  | data_eqn_decls_scs data_eqn_decl SEMICOLON
  ;

data_eqn_decl:
    data_expr EQUALS data_expr
  | data_expr ARROW data_expr EQUALS data_expr
  ;

data_vars_decls_scs:
    data_vars_decl SEMICOLON
  | data_vars_decls_scs data_vars_decl SEMICOLON
  ;
'''

MULTI_ACTION_GRAMMAR = '''
mult_act:
    param_ids_bs
  | TAU
  ;

param_ids_bs:
    param_id
  | param_ids_bs BAR param_id
  ;

param_id:
    ID
  | ID LPAR data_exprs_cs RPAR
  ;
'''

PROCESS_EXPRESSION_GRAMMAR = '''
proc_expr:
  proc_expr_choice
  ;

proc_expr_choice:
    proc_expr_sum
  | proc_expr_sum PLUS proc_expr_choice
  ;

proc_expr_sum:
    proc_expr_merge
  | SUM data_vars_decls_cs DOT proc_expr_sum
  ;

proc_expr_merge:
    proc_expr_binit
  | proc_expr_binit BARS proc_expr_merge_rhs
  | proc_expr_binit LMERGE proc_expr_merge_rhs
  ;

proc_expr_merge_rhs:
    proc_expr_merge
  | SUM data_vars_decls_cs DOT proc_expr_merge_rhs
  ;

proc_expr_binit:
    proc_expr_cond
  | proc_expr_binit BINIT proc_expr_binit_rhs
  ;

proc_expr_binit_rhs:
    proc_expr_cond   
  | SUM data_vars_decls_cs DOT proc_expr_binit_rhs
  ;

proc_expr_cond:
    proc_expr_seq
  | data_expr_prefix ARROW proc_expr_cond_la
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_cond_la
  ;

proc_expr_cond_la:
    proc_expr_cond
  | SUM data_vars_decls_cs DOT proc_expr_cond_la
  ;

proc_expr_seq:
    proc_expr_at
  | proc_expr_at DOT proc_expr_seq_rhs
  ;

proc_expr_seq_wo_cond:
    proc_expr_at_wo_cond
  | proc_expr_at DOT proc_expr_seq_rhs_wo_cond
  ;

proc_expr_seq_rhs:
    proc_expr_seq
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs
  | data_expr_prefix ARROW proc_expr_seq_rhs
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_seq_rhs
  ;

proc_expr_seq_rhs_wo_cond:
    proc_expr_seq_wo_cond
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs_wo_cond
  ;

proc_expr_at:
    proc_expr_sync
  | proc_expr_at AT data_expr_prefix
  ;

proc_expr_at_wo_cond:
    proc_expr_sync_wo_cond
  | proc_expr_at_wo_cond AT data_expr_prefix
  ;

proc_expr_sync:
    proc_expr_primary
  | proc_expr_primary BAR proc_expr_sync_rhs
  ;

proc_expr_sync_wo_cond:
    proc_expr_primary
  | proc_expr_primary BAR proc_expr_sync_rhs_wo_cond
  ;

proc_expr_sync_rhs:
    proc_expr_sync
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs
  | data_expr_prefix ARROW proc_expr_sync_rhs
  | data_expr_prefix ARROW proc_expr_sync_rhs_wo_cond ELSE proc_expr_sync_rhs
  ;

proc_expr_sync_rhs_wo_cond:
    proc_expr_sync_wo_cond
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs_wo_cond
  ;

proc_expr_primary:
    proc_constant
  | param_id
  | id_assignment
  | proc_quant
  | LPAR proc_expr RPAR
  ;

proc_constant:
    DELTA
  | TAU
  ;

id_assignment:
    ID LPAR RPAR
  | ID LPAR id_inits_cs RPAR
  ;

proc_quant:
    BLOCK LPAR act_names_set COMMA proc_expr RPAR
  | HIDE LPAR act_names_set COMMA proc_expr RPAR
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR
  | ALLOW LPAR mult_act_names_set COMMA proc_expr RPAR
  ;

act_names_set:
    LBRACE RBRACE
  | LBRACE ids_cs RBRACE
  ;

ren_expr_set:
    LBRACE RBRACE
  | LBRACE ren_exprs_cs RBRACE
  ;

ren_exprs_cs:
    ren_expr
  | ren_exprs_cs COMMA ren_expr
  ;

ren_expr:
  ID ARROW ID
  ;

comm_expr_set:
    LBRACE RBRACE
  | LBRACE comm_exprs_cs RBRACE
  ;

comm_exprs_cs:
    comm_expr
  | comm_exprs_cs COMMA comm_expr
  ;

comm_expr:
    comm_expr_lhs
  | comm_expr_lhs ARROW TAU
  | comm_expr_lhs ARROW ID
  ;

comm_expr_lhs:
  ID BAR ids_bs
  ;

ids_bs:
    ID
  | ids_bs BAR ID
  ;

mult_act_names_set:
    LBRACE RBRACE
  | LBRACE mult_act_names_cs RBRACE
  ;

mult_act_names_cs:
    mult_act_name
  | mult_act_names_cs COMMA mult_act_name
  ;

mult_act_name:
  ids_bs
  ;
'''

PROCESS_SPECIFICATION_GRAMMAR = '''
proc_spec:
  proc_spec_elts
  ;

proc_spec_elts:
     proc_spec_elt
   | proc_spec_elts proc_spec_elt
   ;

proc_spec_elt:
    data_spec_elt
  | act_spec
  | glob_var_spec
  | proc_eqn_spec
  | proc_init
  ;

act_spec:
  KWACT acts_decls_scs
  ;

acts_decls_scs:
    acts_decl SEMICOLON
  | acts_decls_scs acts_decl SEMICOLON
  ;

acts_decl:
    ids_cs
  | ids_cs COLON domain
  ;

glob_var_spec:
  KWGLOB data_vars_decls_scs
  ;

proc_eqn_spec:
  KWPROC proc_eqn_decls_scs
  ;

proc_eqn_decls_scs:
    proc_eqn_decl SEMICOLON
  | proc_eqn_decls_scs proc_eqn_decl SEMICOLON
  ;

proc_eqn_decl:
    ID EQUALS proc_expr
  | ID LPAR data_vars_decls_cs RPAR EQUALS proc_expr
  ;

proc_init:
  KWINIT proc_expr SEMICOLON
  ;
'''

STATE_FORMULA_GRAMMAR = '''
state_frm:
  state_frm_quant
  ;

state_frm_quant:
    state_frm_imp
  | FORALL data_vars_decls_cs DOT state_frm_quant
  | EXISTS data_vars_decls_cs DOT state_frm_quant
  | NU ID fixpoint_params DOT state_frm_quant
  | MU ID fixpoint_params DOT state_frm_quant
  ;

fixpoint_params:
    /* empty */
  | LPAR data_var_decl_inits_cs RPAR
  ;

data_var_decl_inits_cs:
    data_var_decl_init
  | data_var_decl_inits_cs COMMA data_var_decl_init
  ;

data_var_decl_init:
  ID COLON sort_expr EQUALS data_expr
  ;

state_frm_imp:
    state_frm_and
  | state_frm_and IMP state_frm_imp_rhs
  ;

state_frm_imp_rhs:
    state_frm_imp
  | FORALL data_vars_decls_cs DOT state_frm_imp_rhs
  | EXISTS data_vars_decls_cs DOT state_frm_imp_rhs
  | NU ID fixpoint_params DOT state_frm_imp_rhs
  | MU ID fixpoint_params DOT state_frm_imp_rhs
  ;

state_frm_and:
    state_frm_prefix
  | state_frm_prefix AND state_frm_and_rhs
  | state_frm_prefix BARS state_frm_and_rhs
  ;

state_frm_and_rhs:
    state_frm_and
  | FORALL data_vars_decls_cs DOT state_frm_and_rhs
  | EXISTS data_vars_decls_cs DOT state_frm_and_rhs
  | NU ID fixpoint_params DOT state_frm_and_rhs
  | MU ID fixpoint_params DOT state_frm_and_rhs
  ;

state_frm_prefix:
    state_frm_primary
  | EXCLAM state_frm_quant_prefix
  | LBRACK reg_frm RBRACK state_frm_quant_prefix
  | LANG reg_frm RANG state_frm_quant_prefix
  | YALED AT data_expr_prefix
  | DELAY AT data_expr_prefix
  ;

state_frm_quant_prefix:
    state_frm_prefix
  | FORALL data_vars_decls_cs DOT state_frm_quant_prefix
  | EXISTS data_vars_decls_cs DOT state_frm_quant_prefix
  | NU ID fixpoint_params DOT state_frm_quant_prefix
  | MU ID fixpoint_params DOT state_frm_quant_prefix
  ;

state_frm_primary:
    VAL LPAR data_expr RPAR
  | param_id
  | CTRUE
  | CFALSE
  | YALED
  | DELAY
  | LPAR state_frm RPAR
  ;

reg_frm:
    act_frm
  | reg_frm_alt_naf
  ;

reg_frm_alt_naf:
    reg_frm_seq_naf
  | reg_frm_seq PLUS reg_frm_alt
  ;

reg_frm_alt:
    reg_frm_seq
  | reg_frm_seq PLUS reg_frm_alt
  ;

reg_frm_seq_naf:
    reg_frm_postfix_naf
  | reg_frm_postfix DOT reg_frm_seq
  ;

reg_frm_seq:
    reg_frm_postfix
  | reg_frm_postfix DOT reg_frm_seq
  ;

reg_frm_postfix_naf:
    reg_frm_primary_naf
  | reg_frm_postfix STAR
  | reg_frm_postfix PLUS
  ;

reg_frm_postfix:
    reg_frm_primary
  | reg_frm_postfix STAR
  | reg_frm_postfix PLUS
  ;

reg_frm_primary_naf:
    NIL
  | LPAR reg_frm_alt_naf RPAR
  ;

reg_frm_primary:
    act_frm
  | NIL
  | LPAR reg_frm_alt_naf RPAR
  ;

act_frm:
  act_frm_quant
  ;

act_frm_quant:
    act_frm_imp
  | FORALL data_vars_decls_cs DOT act_frm_quant
  | EXISTS data_vars_decls_cs DOT act_frm_quant
  ;

act_frm_imp:
    act_frm_and
  | act_frm_and IMP act_frm_imp_rhs
  ;

act_frm_imp_rhs:
    act_frm_imp
  | FORALL data_vars_decls_cs DOT act_frm_imp_rhs
  | EXISTS data_vars_decls_cs DOT act_frm_imp_rhs
  ;

act_frm_and:
    act_frm_at
  | act_frm_prefix AND act_frm_and_rhs
  | act_frm_prefix BARS act_frm_and_rhs
  ;

act_frm_and_rhs:
    act_frm_and
  | FORALL data_vars_decls_cs DOT act_frm_and_rhs
  | EXISTS data_vars_decls_cs DOT act_frm_and_rhs
  ;

act_frm_at:
    act_frm_prefix
  | act_frm_at AT data_expr_prefix
  ;

act_frm_prefix:
    act_frm_primary
  | EXCLAM act_frm_quant_prefix
  ;

act_frm_quant_prefix:
    act_frm_prefix
  | FORALL data_vars_decls_cs DOT act_frm_quant_prefix
  | EXISTS data_vars_decls_cs DOT act_frm_quant_prefix
  ;

act_frm_primary:
    mult_act
  | VAL LPAR data_expr RPAR
  | CTRUE
  | CFALSE
  | LPAR act_frm RPAR
  ;
'''

ACTION_RENAME_GRAMMAR = '''
action_rename_spec:
  action_rename_spec_elts
  ;

action_rename_spec_elts:
     action_rename_spec_elt
   | action_rename_spec_elts action_rename_spec_elt
   ;

action_rename_spec_elt:
    data_spec_elt
  | act_spec
  | action_rename_rule_spec
  ;

action_rename_rule_spec:
  action_rename_rule_sect
  ;

action_rename_rule_sect:
    RENAME action_rename_rules_scs
  | KWVAR data_vars_decls_scs RENAME action_rename_rules_scs


action_rename_rules_scs:
    action_rename_rule SEMICOLON
  | action_rename_rules_scs action_rename_rule SEMICOLON
  ;

action_rename_rule:
    data_expr ARROW param_id IMP action_rename_rule_rhs
  | param_id IMP action_rename_rule_rhs
  ;

action_rename_rule_rhs:
    param_id
  | proc_constant
  ;
'''

PBES_EXPRESSION_GRAMMAR = '''
pb_expr:
  pb_expr_quant
  ;

pb_expr_quant:
    pb_expr_imp
  | FORALL data_vars_decls_cs DOT pb_expr_quant
  | EXISTS data_vars_decls_cs DOT pb_expr_quant
  ;

pb_expr_imp:
    pb_expr_and
  | pb_expr_and IMP pb_expr_imp_rhs
  ;

pb_expr_imp_rhs:
    pb_expr_imp
  | FORALL data_vars_decls_cs DOT pb_expr_imp_rhs
  | EXISTS data_vars_decls_cs DOT pb_expr_imp_rhs
  ;

pb_expr_and:
    pb_expr_not
  | pb_expr_not AND pb_expr_and_rhs
  | pb_expr_not BARS pb_expr_and_rhs
  ;

pb_expr_and_rhs:
    pb_expr_and
  | FORALL data_vars_decls_cs DOT pb_expr_and_rhs
  | EXISTS data_vars_decls_cs DOT pb_expr_and_rhs
  ;

pb_expr_not:
    pb_expr_primary
  | EXCLAM pb_expr_quant_not
  ;

pb_expr_quant_not:
    pb_expr_not
  | FORALL data_vars_decls_cs DOT pb_expr_quant_not
  | EXISTS data_vars_decls_cs DOT pb_expr_quant_not
  ;

pb_expr_primary:
    VAL LPAR data_expr RPAR
  | param_id
  | CTRUE
  | CFALSE
  | LPAR pb_expr RPAR
  ;
'''

PBES_SPECIFICATION_GRAMMAR = '''
pbes_spec:
  pbes_spec_elts
  ;

pbes_spec_elts:
     pbes_spec_elt
   | pbes_spec_elts pbes_spec_elt
   ;

pbes_spec_elt:
    data_spec_elt
  | glob_var_spec
  | pb_eqn_spec
  | pb_init
  ;

pb_eqn_spec:
  KWPBES pb_eqn_decls_scs
  ;

pb_eqn_decls_scs:
    pb_eqn_decl SEMICOLON
  | pb_eqn_decls_scs pb_eqn_decl SEMICOLON
  ;

pb_eqn_decl:
    fixpoint ID EQUALS pb_expr
  | fixpoint ID LPAR data_vars_decls_cs RPAR EQUALS pb_expr
  ;

fixpoint:
    MU
  | NU
  ;

pb_init:
  KWINIT param_id SEMICOLON
  ;
'''

# parses token lines that contain entries separated by '|'
def parse_tokens(text):
    result = []
    lines = text.rsplit('\n')
    for line in lines:
        words = re.split(r'\s+\|\s+', line)
        if len(words) < 2:
            continue
        result.append(words)
    return result

# parses tag lines
def parse_tags(text):
    result = []
    lines = text.rsplit('\n')
    lines = map(string.strip, lines)
    for line in lines:
        if len(line) == 0:
            continue
        result.append(['"%s"' % line, 'TAG_%s' % line.upper()])
    return result

# parses nonterminals
# type is one of 'term', 'appl' or 'list'
def parse_nonterminals(text, type):
    result = []
    lines = text.rsplit('\n')
    lines = map(string.strip, lines)
    for line in lines:
        if len(line) == 0:
            continue
        result.append('%%%%type <%s> %s\n' % (type, line))
    return ''.join(result)

def make_parser(lexfile, yaccfile, tag_text, token_text, grammar_text):
    tokens = parse_tags(tag_text) + parse_tokens(token_text)

    #--------------------------------------------------------#
    # generate lex file
    #--------------------------------------------------------#
    text = ''
    for t in tokens:
        text = text + '%-20s { process_string(); return %s; }\n' % (t[0], t[1])
    text = TOKEN_PREFIX + text + TOKEN_POSTFIX
    insert_text_in_file(lexfile, text, 'generated tokens')

    #--------------------------------------------------------#
    # generate yacc file
    #--------------------------------------------------------#
    
    # generate terminals
    text = ''
    for t in tokens:
        text = text + '%%%%token <appl> %s\n' % t[1]
    print text
    insert_text_in_file(yaccfile, text, 'generated terminals')

    # generate non-terminals
    text = ''
    text = text + parse_nonterminals(NONTERMINAL_TERMS, 'term')
    text = text + parse_nonterminals(NONTERMINAL_FUNCTION_APPLICATIONS, 'appl')
    text = text + parse_nonterminals(NONTERMINAL_LISTS, 'list')
    insert_text_in_file(yaccfile, text, 'generated non-terminals')

GRAMMAR = SORT_EXPRESSION_GRAMMAR        + \
          DATA_EXPRESSION_GRAMMAR        + \
          DATA_SPECIFICATION_GRAMMAR     + \
          MULTI_ACTION_GRAMMAR           + \
          PROCESS_EXPRESSION_GRAMMAR     + \
          PROCESS_SPECIFICATION_GRAMMAR  + \
          STATE_FORMULA_GRAMMAR          + \
          ACTION_RENAME_GRAMMAR          + \
          PBES_EXPRESSION_GRAMMAR        + \
          PBES_SPECIFICATION_GRAMMAR

make_parser('../../lps/example/mcrl2lexer.ll', '../../lps/example/mcrl2parser.yy', TAGS, TOKENS, GRAMMAR)
os.chdir('../../lps/example')
os.system('flex -Pmcrl3 -omcrl2lexer.cpp mcrl2lexer.ll')
os.system('bison -p mcrl3 --defines=mcrl2parser.h -o mcrl2parser.cpp mcrl2parser.yy')
os.system('sed -i \'s+#include "mcrl2parser.h"+#include "mcrl2/core/detail/mcrl2parser.h"+\' mcrl2parser.cpp')
os.system("sed -i '/isatty/d' mcrl2lexer.cpp")
