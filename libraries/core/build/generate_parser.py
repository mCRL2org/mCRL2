# -*- coding: iso-8859-15 -*-
#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
import string
from mcrl2_utility import *

TOKEN_PREFIX = r'''%%}
String     [a-zA-Z\\_][a-zA-Z0-9\\_']*
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
{String} | STRING
{Number} | NUMBER
'''

NONTERMINAL_TERMS = '''
start
'''

NONTERMINAL_FUNCTION_APPLICATIONS = '''
SortExpr
SortArrow
SortStruct
SortId
sort_expr_arrow
domain_no_arrow_elt
sort_expr_struct
StructCons
StructLabel
StructProj
StructProjLabel
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
StructCons_list_bar_separated
StructProj_list_comma_separated
StructProj_list
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

MCRL2_GRAMMAR = '''
//Start
//-----

//start
start:
  TAG_IDENTIFIER STRING
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_SORT_EXPR SortExpr
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_DATA_EXPR data_expr
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_DATA_SPEC data_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_MULT_ACT mult_act
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_PROC_EXPR proc_expr
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_PROC_SPEC proc_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_STATE_FRM state_frm
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_ACTION_RENAME action_rename_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_PBES_SPEC pbes_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  | TAG_DATA_VARS data_vars_decls_scs
    {
      safe_assign($$, (ATerm) $2);
      mcrl3_spec_tree = $$;
    }
  ;

//Sort expressions
//----------------

//sort expression
SortExpr:
  sort_expr_arrow
    {
      safe_assign($$, $1);
    }
  ;

// SortArrow(<SortExpr>+, <SortExpr>)
SortArrow:
  domain_no_arrow ARROW sort_expr_arrow
  {
    safe_assign($$, gsMakeSortArrow($1, $3));
  }
  ;

//arrow sort expression
sort_expr_arrow:
  sort_expr_struct
    {
      safe_assign($$, $1);
    }
  | SortArrow
  ;

//domain
domain_no_arrow:
  domain_no_arrow_elts_hs
    {
      safe_assign($$, ATreverse($1));
    }
  ;

//one or more domain elements, separated by hashes
domain_no_arrow_elts_hs:
  domain_no_arrow_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//domain element
domain_no_arrow_elt:
  sort_expr_struct
    {
      safe_assign($$, $1);
    }
  ;

// SortStruct(<StructCons>+)
SortStruct:
  KWSTRUCT StructCons_list_bar_separated
  {
    safe_assign($$, gsMakeSortStruct(ATreverse($2)));
  }
  ;

//structured sort
sort_expr_struct:
  sort_expr_primary
    {
      safe_assign($$, $1);
    }
  | SortStruct
    {
      safe_assign($$, $1);
    }
  ;

//one ore more structured sort constructors, separated by bars
StructCons_list_bar_separated:
  StructCons
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | StructCons_list_bar_separated BAR StructCons
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//structured sort constructor
//<StructCons>   ::= StructCons(<STRING>, <StructProj>*, <StringOrNil>)
StructCons:
  STRING StructProj_list StructLabel
    {
      safe_assign($$, gsMakeStructCons($1, $2, $3));
    }
  ;

StructProj_list:
  /* empty */
    {
      safe_assign($$, ATmakeList0());
    }
  | LPAR StructProj_list_comma_separated RPAR
    {
      safe_assign($$, ATreverse($2));
    }
  ;
  
//StructLabel
StructLabel:
  /* empty */
    {
      safe_assign($$, gsMakeNil());
    }
  | QMARK STRING
    {
      safe_assign($$, $2);
    }
  ;

//one or more structured sort projections, separated by comma's
StructProj_list_comma_separated:
  StructProj
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | StructProj_list_comma_separated COMMA StructProj
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//<StructProj>   ::= StructProj(<StringOrNil>, <SortExpr>)
StructProj:
    SortExpr
    {
      safe_assign($$, gsMakeStructProj(gsMakeNil(), $1));
    }
    | StructProjLabel SortExpr
    {
      safe_assign($$, gsMakeStructProj($1, $2));
    }  

StructProjLabel:
    STRING COLON
    {
      safe_assign($$, $1);
    }
    ;

// SortId(<STRING>)
SortId:
    STRING
    {
      safe_assign($$, gsMakeSortId($1));
    }
    ;

//primary sort expression
sort_expr_primary:
  SortId
    {
      safe_assign($$, $1);
    }
  | sort_constant
    {
      safe_assign($$, $1);
    }
  | sort_constructor
    {
      safe_assign($$, $1);
    }
  | LPAR SortExpr RPAR
    {
      safe_assign($$, $2);
    }
  ;

//sort constant
sort_constant:
  BOOL
    {
      safe_assign($$, gsMakeSortId($1));
    }
  | POS
    {
      safe_assign($$, gsMakeSortId($1));
    }
  | NAT
    {
      safe_assign($$, gsMakeSortId($1));
    }
  | INT
    {
      safe_assign($$, gsMakeSortId($1));
    }
  | REAL
    {
      safe_assign($$, gsMakeSortId($1));
    }
  ;

//sort constructor
sort_constructor:
  LIST LPAR SortExpr RPAR
    {
      safe_assign($$, mcrl2::data::sort_list::list(mcrl2::data::sort_expression($3)));
    }
  | SET LPAR SortExpr RPAR
    {
      safe_assign($$, mcrl2::data::sort_set::set_(mcrl2::data::sort_expression($3)));
    }
  | BAG LPAR SortExpr RPAR
    {
      safe_assign($$, mcrl2::data::sort_bag::bag(mcrl2::data::sort_expression($3)));
    }
  ;

//Data expressions
//----------------

//data expression
data_expr:
  data_expr_whr
    {
      safe_assign($$, $1);
    }
  ;

//where clause
data_expr_whr:
  data_expr_quant
    {
      safe_assign($$, $1);
    }
  | data_expr_whr WHR id_inits_cs END
    {
      safe_assign($$, gsMakeWhr($1, ATreverse($3)));
    }
  ;

//declaration of one or more identifier initialisations, separated by comma's
id_inits_cs:
  id_init
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | id_inits_cs COMMA id_init
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//identifier initialisation
id_init:
  STRING EQUALS data_expr
    {
      safe_assign($$, gsMakeIdInit($1, $3));
    }
  ;

//quantifications and lambda abstraction
data_expr_quant:
  data_expr_imp
    {
      safe_assign($$, $1);
    }
  | LAMBDA data_vars_decls_cs DOT data_expr_quant
    {
      safe_assign($$, gsMakeBinder(gsMakeLambda(), $2, $4));
    }
  | FORALL data_vars_decls_cs DOT data_expr_quant
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
    }
  ;

//one or more declarations of one or more data variables, separated by
//comma's
data_vars_decls_cs:
  data_vars_decl
    {
      safe_assign($$, $1);
    }
  | data_vars_decls_cs COMMA data_vars_decl
    {
      safe_assign($$, ATconcat($1, $3));
    }
  ;

//declaration of one or more data variables
data_vars_decl:
  ids_cs COLON SortExpr
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeDataVarId(ATAelementAt($1, i), $3)));
      }
    }
  ;

//implication (right associative)
data_expr_imp:
  data_expr_and
    {
      safe_assign($$, $1);
    }
  | data_expr_and IMP data_expr_imp_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//right argument of implication
data_expr_imp_rhs:
  data_expr_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT data_expr_imp_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
    }
  | EXISTS data_vars_decls_cs DOT data_expr_imp_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
    }
  ;

//conjunction and disjunction (right associative)
data_expr_and:
  data_expr_eq
    {
      safe_assign($$, $1);
    }
  | data_expr_eq AND data_expr_and_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_eq BARS data_expr_and_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//right argument of conjunction and disjunction
data_expr_and_rhs:
  data_expr_and
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT data_expr_and_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
    }
  | EXISTS data_vars_decls_cs DOT data_expr_and_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
    }
  ;

//equality (right associative)
data_expr_eq:
  data_expr_rel
    {
      safe_assign($$, $1);
    }
  | data_expr_rel EQ data_expr_eq_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_rel NEQ data_expr_eq_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//right argument of equality
data_expr_eq_rhs:
  data_expr_eq
    {
      safe_assign($$, $1);
    }
  | LAMBDA data_vars_decls_cs DOT data_expr_eq_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeLambda(), $2, $4));
    }
  | FORALL data_vars_decls_cs DOT data_expr_eq_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
    }
  | EXISTS data_vars_decls_cs DOT data_expr_eq_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
    }
  ;

//relations
data_expr_rel:
  data_expr_cons
    {
      safe_assign($$, $1);
    }
  | data_expr_cons GTE data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_cons LTE data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_cons RANG data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_cons LANG data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_cons IN data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//cons (right associative)
data_expr_cons:
  data_expr_snoc
    {
      safe_assign($$, $1);
    }
  | data_expr_add CONS data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//snoc (left associative)
data_expr_snoc:
  data_expr_concat
    {
      safe_assign($$, $1);
    }
  | data_expr_snoc SNOC data_expr_add
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//concatenation (left associative)
data_expr_concat:
  data_expr_add
    {
      safe_assign($$, $1);
    }
  | data_expr_concat CONCAT data_expr_add
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//addition and subtraction (left associative)
data_expr_add:
  data_expr_div
    {
      safe_assign($$, $1);
    }
  | data_expr_add PLUS data_expr_div
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_add MINUS data_expr_div
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//division (left associative)
data_expr_div:
  data_expr_mult
    {
      safe_assign($$, $1);
    }
  | data_expr_div DIV data_expr_mult
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_div MOD data_expr_mult
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_div SLASH data_expr_mult
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//multiplication and list at (left associative)
data_expr_mult:
  data_expr_prefix
    {
      safe_assign($$, $1);
    }
  | data_expr_mult STAR data_expr_prefix
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  | data_expr_mult DOT data_expr_prefix
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
    }
  ;

//prefix data expression
data_expr_prefix:
  data_expr_postfix
    {
      safe_assign($$, $1);
    }
  | EXCLAM data_expr_quant_prefix
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId($1), ATmakeList1((ATerm) $2)));
    }
  | MINUS data_expr_prefix
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId($1), ATmakeList1((ATerm) $2)));
    }
  | HASH data_expr_prefix
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId($1), ATmakeList1((ATerm) $2)));
    }
  ;

//quantifier or prefix data expression
data_expr_quant_prefix:
  data_expr_prefix
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT data_expr_quant_prefix
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant_prefix
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
    }
  ;

//postfix data expression
data_expr_postfix:
  data_expr_primary
    {
      safe_assign($$, $1);
    }
  | data_expr_postfix LPAR data_exprs_cs RPAR
    {
      safe_assign($$, gsMakeDataAppl($1, ATreverse($3)));
    }
/*  | data_expr_postfix LBRACK data_expr ARROW data_expr RBRACK
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId(gsMakeOpIdNameFuncUpdate()), ATmakeList3((ATerm) $1, (ATerm) $3, (ATerm) $5)));
    } */
  ;

//one or more data expressions, separated by comma's
data_exprs_cs:
  data_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | data_exprs_cs COMMA data_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//primary data expression
data_expr_primary:
  STRING
    {
      safe_assign($$, gsMakeId($1));
    }
  | data_constant
    {
      safe_assign($$, $1);
    }
  | data_enumeration
    {
      safe_assign($$, $1);
    }
  | data_comprehension
    {
      safe_assign($$, $1);
    }
  | LPAR data_expr RPAR
    {
      safe_assign($$, $2);
    }
  ;

//constant
data_constant:
  CTRUE
    {
      safe_assign($$, gsMakeId($1));
    }
  | CFALSE
    {
      safe_assign($$, gsMakeId($1));
    }
  | IF
    {
      safe_assign($$, gsMakeId($1));
    }
  | NUMBER
    {
      safe_assign($$, gsMakeId($1));
    }
  | LBRACK RBRACK
    {
      safe_assign($$, gsMakeId(mcrl2::data::sort_list::nil_name()));
    }
  | LBRACE RBRACE
    {
      safe_assign($$, gsMakeId(mcrl2::data::sort_set::emptyset_name()));
    }
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs_cs RBRACK
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_list::list_enumeration_name()), ATreverse($2)));
    }
  | LBRACE data_exprs_cs RBRACE
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_set::set_enumeration_name()), ATreverse($2)));
    }
  | LBRACE bag_enum_elts_cs RBRACE
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_bag::bag_enumeration_name()), ATreverse($2)));
    }
  ;

//one or more bag enumeration elements, separated by comma's
bag_enum_elts_cs:
  bag_enum_elt
    {
      safe_assign($$, $1);
    }
  | bag_enum_elts_cs COMMA bag_enum_elt
    {
      safe_assign($$, ATconcat($3, $1));
    }
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
    {
      safe_assign($$, ATmakeList2((ATerm) $3, (ATerm) $1));
    }
  ;

//comprehension
data_comprehension:
  LBRACE data_var_decl BAR data_expr RBRACE
    {
      safe_assign($$, gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) $2), $4));
    }
  ;

//declaration of a data variable
data_var_decl:
  STRING COLON SortExpr
    {
      safe_assign($$, gsMakeDataVarId($1, $3));
    }
  ;

//Data specifications
//-------------------

//data specification
data_spec:
  data_spec_elts
    {
      safe_assign($$, gsDataSpecEltsToSpec(ATreverse($1)));
    }
  ;

//data specification elements
data_spec_elts:
  data_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
   | data_spec_elts data_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
   ;

//data specification element
data_spec_elt:
  sort_spec
    {
      safe_assign($$, $1);
    }
  | cons_spec
    {
      safe_assign($$, $1);
    }
  | map_spec
    {
      safe_assign($$, $1);
    }
  | data_eqn_spec
    {
      safe_assign($$, $1);
    }
  ;

//sort specification
sort_spec:
  KWSORT sorts_decls_scs
    {
      safe_assign($$, gsMakeSortSpec($2));
    }
  ;

//declaration of one or more sorts, separated by semicolons
sorts_decls_scs:
  sorts_decl SEMICOLON
    {
      safe_assign($$, $1);
    }
  | sorts_decls_scs sorts_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
    }
  ;

//declaration of one or more sorts
sorts_decl:
  ids_cs
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeSortId(ATAelementAt($1, i))));
      }
    }
  | STRING EQUALS SortExpr
    {
      safe_assign($$, ATmakeList1((ATerm) gsMakeSortRef($1, $3)));
    }
  ;

//one or more identifiers, separated by comma's
ids_cs:
  STRING
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | ids_cs COMMA STRING
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//domain
domain:
  domain_no_arrow
    {
      safe_assign($$, $1);
    }
  | domain_no_arrow ARROW SortExpr
    {
      safe_assign($$, ATmakeList1((ATerm) gsMakeSortArrow($1, $3)));
    }
  ;

//constructor operation specification
cons_spec:
  KWCONS ops_decls_scs
    {
      safe_assign($$, gsMakeConsSpec($2));
    }
  ;

//operation specification
map_spec:
  KWMAP ops_decls_scs
    {
      safe_assign($$, gsMakeMapSpec($2));
    }
  ;

//one or more declarations of one or more operations of the same sort,
//separated by semicolons
ops_decls_scs:
  ops_decl SEMICOLON
    {
      safe_assign($$, $1);
    }
  | ops_decls_scs ops_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
    }
  ;

//declaration of one or more operations of the same sort
ops_decl:
  ids_cs COLON SortExpr
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeOpId(ATAelementAt($1, i), $3)));
      }
    }
  ;

//data equation specification
data_eqn_spec:
  data_eqn_sect
    {
      safe_assign($$, gsMakeDataEqnSpec($1));
    }
  ;

//data equation section
data_eqn_sect:
  KWEQN data_eqn_decls_scs
    {
      safe_assign($$, ATreverse($2));
    }
  | KWVAR data_vars_decls_scs KWEQN data_eqn_decls_scs
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($4);
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt($4, i);
        safe_assign($$,
          ATinsert($$, (ATerm) gsMakeDataEqn($2, ATAgetArgument(DataEqn, 1),
            ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3))));
      }
    }
  ;

//declaration of one or more data equations, separated by semicolons
data_eqn_decls_scs:
  data_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | data_eqn_decls_scs data_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
  ;

//data equation declaration
data_eqn_decl:
  data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), mcrl2::data::sort_bool::true_(), $1, $3));
    }
  | data_expr ARROW data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), $1, $3, $5));
    }
  ;

//one or more declarations of one or more data variables,
//separated by semicolons
data_vars_decls_scs:
  data_vars_decl SEMICOLON
    {
      safe_assign($$, $1);
    }
  | data_vars_decls_scs data_vars_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
    }
  ;

//Multi-actions
//-------------

//multi-action
mult_act:
  param_ids_bs
    {
      safe_assign($$, gsMakeMultAct(ATreverse($1)));
    }
  | TAU
    {
      safe_assign($$, gsMakeMultAct(ATmakeList0()));
    }
  ;

//one or more parameterised id's, separated by bars
param_ids_bs:
  param_id
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | param_ids_bs BAR param_id
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//parameterised id
param_id:
  STRING
    {
      safe_assign($$, gsMakeParamId($1, ATmakeList0()));
    }
  | STRING LPAR data_exprs_cs RPAR
    {
      safe_assign($$, gsMakeParamId($1, ATreverse($3)));
    }
  ;

//Process expressions
//-------------------

//process expression
proc_expr:
  proc_expr_choice
    {
      safe_assign($$, $1);
    }
  ;

//choice (right associative)
proc_expr_choice:
  proc_expr_sum
    {
      safe_assign($$, $1);
    }
  | proc_expr_sum PLUS proc_expr_choice
    {
      safe_assign($$, gsMakeChoice($1, $3));
    }
  ;

//summation
proc_expr_sum:
  proc_expr_merge
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_sum
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//merge and left merge (right associative)
proc_expr_merge:
  proc_expr_binit
    {
      safe_assign($$, $1);
    }
  | proc_expr_binit BARS proc_expr_merge_rhs
    {
      safe_assign($$, gsMakeMerge($1, $3));
    }
  | proc_expr_binit LMERGE proc_expr_merge_rhs
    {
      safe_assign($$, gsMakeLMerge($1, $3));
    }
  ;

//right argument of merge
proc_expr_merge_rhs:
  proc_expr_merge
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_merge_rhs
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//bounded initialisation (left associative)
proc_expr_binit:
  proc_expr_cond
    {
      safe_assign($$, $1);
    }
  | proc_expr_binit BINIT proc_expr_binit_rhs
    {
      safe_assign($$, gsMakeBInit($1, $3));
    }
  ;

//right argument of bounded initialisation
proc_expr_binit_rhs:
  proc_expr_cond
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_binit_rhs
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//conditional
proc_expr_cond:
  proc_expr_seq
    {
      safe_assign($$, $1);
    }
  | data_expr_prefix ARROW proc_expr_cond_la
    {
      safe_assign($$, gsMakeIfThen($1, $3));
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_cond_la
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
    }
  ;

//last argument of conditional
proc_expr_cond_la:
  proc_expr_cond
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_cond_la
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//sequential composition (right associative)
proc_expr_seq:
  proc_expr_at
    {
      safe_assign($$, $1);
    }
  | proc_expr_at DOT proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeSeq($1, $3));
    }
  ;

//sequential composition not mentioning conditional
proc_expr_seq_wo_cond:
  proc_expr_at_wo_cond
    {
      safe_assign($$, $1);
    }
  | proc_expr_at DOT proc_expr_seq_rhs_wo_cond
    {
      safe_assign($$, gsMakeSeq($1, $3));
    }
  ;

//right argument of sequential composition
proc_expr_seq_rhs:
  proc_expr_seq
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeIfThen($1, $3));
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
    }
  ;

//right argument of sequential composition not mentioning conditional
proc_expr_seq_rhs_wo_cond:
  proc_expr_seq_wo_cond
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs_wo_cond
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//timed expression
proc_expr_at:
  proc_expr_sync
    {
      safe_assign($$, $1);
    }
  | proc_expr_at AT data_expr_prefix
    {
      safe_assign($$, gsMakeAtTime($1, $3));
    }
  ;

//timed expression not mentioning conditional
proc_expr_at_wo_cond:
  proc_expr_sync_wo_cond
    {
      safe_assign($$, $1);
    }
  | proc_expr_at_wo_cond AT data_expr_prefix
    {
      safe_assign($$, gsMakeAtTime($1, $3));
    }
  ;

//synchronisation (right associative)
proc_expr_sync:
  proc_expr_primary
    {
      safe_assign($$, $1);
    }
  | proc_expr_primary BAR proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeSync($1, $3));
    }
  ;

//synchronisation (right associative)
proc_expr_sync_wo_cond:
  proc_expr_primary
    {
      safe_assign($$, $1);
    }
  | proc_expr_primary BAR proc_expr_sync_rhs_wo_cond
    {
      safe_assign($$, gsMakeSync($1, $3));
    }
  ;

//right argument of synchronisation
proc_expr_sync_rhs:
  proc_expr_sync
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeIfThen($1, $3));
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs_wo_cond ELSE proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
    }
  ;

//right argument of synchronisation
proc_expr_sync_rhs_wo_cond:
  proc_expr_sync_wo_cond
    {
      safe_assign($$, $1);
    }
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs_wo_cond
    {
      safe_assign($$, gsMakeSum($2, $4));
    }
  ;

//primary process expression
proc_expr_primary:
  proc_constant
    {
      safe_assign($$, $1);
    }
  | param_id
    {
      safe_assign($$, $1);
    }
  | id_assignment
    {
      //mcrl3yyerror("process assignments are not yet supported");
      // YYABORT
      safe_assign($$, $1);
    }
  | proc_quant
    {
      safe_assign($$, $1);
    }
  | LPAR proc_expr RPAR
    {
      safe_assign($$, $2);
    }
  ;

//process constant
proc_constant:
  DELTA
    {
      safe_assign($$, gsMakeDelta());
    }
  | TAU
    {
      safe_assign($$, gsMakeTau());
    }
  ;

//identifier assignment
id_assignment:
  STRING LPAR RPAR
    {
      safe_assign($$, gsMakeIdAssignment($1, ATmakeList0()));
    }
  | STRING LPAR id_inits_cs RPAR
    {
      safe_assign($$, gsMakeIdAssignment($1, ATreverse($3)));
    }
  ;

//process quantification
proc_quant:
  BLOCK LPAR act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeBlock($3, $5));
    }
  | HIDE LPAR act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeHide($3, $5));
    }
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeRename($3, $5));
    }
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeComm($3, $5));
    }
  | ALLOW LPAR mult_act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeAllow($3, $5));
    }
  ;

//set of action names
act_names_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
    }
  | LBRACE ids_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
    }
  ;

//set of renaming expressions
ren_expr_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
    }
  | LBRACE ren_exprs_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
    }
  ;

//one or more renaming expressions, separated by comma's
ren_exprs_cs:
  ren_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | ren_exprs_cs COMMA ren_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//renaming expression
ren_expr:
  STRING ARROW STRING
    {
      safe_assign($$, gsMakeRenameExpr($1, $3));
    }
  ;

//set of communication expressions
comm_expr_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
    }
  | LBRACE comm_exprs_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
    }
  ;

//one or more communication expressions, separated by comma's
comm_exprs_cs:
  comm_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | comm_exprs_cs COMMA comm_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//communication expression
comm_expr:
  comm_expr_lhs
    {
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
    }
  | comm_expr_lhs ARROW TAU
    {
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
    }
  | comm_expr_lhs ARROW STRING
    {
      safe_assign($$, gsMakeCommExpr($1, $3));
    }
  ;

//left-hand side of a communication expression
comm_expr_lhs:
  STRING BAR ids_bs
    {
      safe_assign($$, gsMakeMultActName(ATinsert(ATreverse($3), (ATerm) $1)));
    }
  ;

//one or more id's, separated by bars
ids_bs:
  STRING
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | ids_bs BAR STRING
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//set of multi action names
mult_act_names_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
    }
  | LBRACE mult_act_names_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
    }
  ;

//one or more multi action names, separated by comma's
mult_act_names_cs:
  mult_act_name
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | mult_act_names_cs COMMA mult_act_name
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//multi action name
mult_act_name:
  ids_bs
    {
      safe_assign($$, gsMakeMultActName(ATreverse($1)));
    }
  ;

//Process specifications
//----------------------

//process specification
proc_spec:
  proc_spec_elts
    {
      safe_assign($$, gsProcSpecEltsToSpec(ATreverse($1)));
    }
  ;

//process specification elements
proc_spec_elts:
  proc_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
   | proc_spec_elts proc_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
   ;

//process specification element
proc_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
    }
  | act_spec
    {
      safe_assign($$, $1);
    }
  | glob_var_spec
    {
      safe_assign($$, $1);
    }
  | proc_eqn_spec
    {
      safe_assign($$, $1);
    }
  | proc_init
    {
      safe_assign($$, $1);
    }
  ;

//action specification
act_spec:
  KWACT acts_decls_scs
    {
      safe_assign($$, gsMakeActSpec($2));
    }
  ;

//one or more declarations of one or more actions, separated by semicolons
acts_decls_scs:
  acts_decl SEMICOLON
    {
      safe_assign($$, $1);
    }
  | acts_decls_scs acts_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
    }
  ;

//declaration of one or more actions
acts_decl:
  ids_cs
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$,
          ATinsert($$, (ATerm) gsMakeActId(ATAelementAt($1, i), ATmakeList0())));
      }
    }
  | ids_cs COLON domain
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeActId(ATAelementAt($1, i), $3)));
      }
    }
  ;

//global variable specification
glob_var_spec:
  KWGLOB data_vars_decls_scs
    {
      safe_assign($$, gsMakeGlobVarSpec($2));
    }
  ;

//process equation specification
proc_eqn_spec:
  KWPROC proc_eqn_decls_scs
    {
      safe_assign($$, gsMakeProcEqnSpec(ATreverse($2)));
    }
  ;

//one or more process equation declarations, separated by semicolons
proc_eqn_decls_scs:
  proc_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | proc_eqn_decls_scs proc_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
  ;

//process equation declaration
proc_eqn_decl:
  STRING EQUALS proc_expr
    {
      safe_assign($$, gsMakeProcEqn(
        gsMakeProcVarId($1, ATmakeList0()), ATmakeList0(), $3));
    }
  | STRING LPAR data_vars_decls_cs RPAR EQUALS proc_expr
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength($3);
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt($3, i), 1));
      }
      safe_assign($$, gsMakeProcEqn(
        gsMakeProcVarId($1, ATreverse(SortExprs)), $3, $6));
    }
  ;

//process initialisation
proc_init:
  KWINIT proc_expr SEMICOLON
    {
      safe_assign($$, gsMakeProcessInit($2));
    }
  ;

//State formulas
//--------------

//state formula
state_frm:
  state_frm_quant
    {
      safe_assign($$, $1);
    }
  ;

//quantification
state_frm_quant:
  state_frm_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateExists($2, $4));
    }
  | NU STRING fixpoint_params DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
    }
  | MU STRING fixpoint_params DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
    }
  ;

//parameters of a fixpoint variable declaration
fixpoint_params:
  /* empty */
    {
      safe_assign($$, ATmakeList0());
    }
  | LPAR data_var_decl_inits_cs RPAR
    {
      safe_assign($$, ATreverse($2));
    }
  ;

//one or more declarations of a data variable with an
//initialisation, separated by comma's
data_var_decl_inits_cs:
  data_var_decl_init
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | data_var_decl_inits_cs COMMA data_var_decl_init
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
    }
  ;

//data variable declaration and initialisation
data_var_decl_init:
  STRING COLON SortExpr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataVarIdInit(gsMakeDataVarId($1, $3), $5));
    }
  ;

//implication (right associative)
state_frm_imp:
  state_frm_and
    {
      safe_assign($$, $1);
    }
  | state_frm_and IMP state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateImp($1, $3));
    }
  ;

//right argument of implication
state_frm_imp_rhs:
  state_frm_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateExists($2, $4));
    }
  | NU STRING fixpoint_params DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
    }
  | MU STRING fixpoint_params DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
    }
  ;

//conjunction and disjunction (right associative)
state_frm_and:
  state_frm_prefix
    {
      safe_assign($$, $1);
    }
  | state_frm_prefix AND state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateAnd($1, $3));
    }
  | state_frm_prefix BARS state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateOr($1, $3));
    }
  ;

//right argument of conjunction and disjunction
state_frm_and_rhs:
  state_frm_and
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateExists($2, $4));
    }
  | NU STRING fixpoint_params DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
    }
  | MU STRING fixpoint_params DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
    }
  ;

//prefix state formula
state_frm_prefix:
  state_frm_primary
    {
      safe_assign($$, $1);
    }
  | EXCLAM state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateNot($2));
    }
  | LBRACK reg_frm RBRACK state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMust($2, $4));
    }
  | LANG reg_frm RANG state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMay($2, $4));
    }
  | YALED AT data_expr_prefix
    {
      safe_assign($$, gsMakeStateYaledTimed($3));
    }
  | DELAY AT data_expr_prefix
    {
      safe_assign($$, gsMakeStateDelayTimed($3));
    }
  ;

//quantifier or prefix state formula
state_frm_quant_prefix:
  state_frm_prefix
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateExists($2, $4));
    }
  | NU STRING fixpoint_params DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
    }
  | MU STRING fixpoint_params DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
    }
  ;

//primary state formula
state_frm_primary:
  VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
    }
  | param_id
    {
      safe_assign($$, gsMakeStateVar(ATAgetArgument($1, 0), ATLgetArgument($1, 1)));
    }
  | CTRUE
    {
      safe_assign($$, gsMakeStateTrue());
    }
  | CFALSE
    {
      safe_assign($$, gsMakeStateFalse());
    }
  | YALED
    {
      safe_assign($$, gsMakeStateYaled());
    }
  | DELAY
    {
      safe_assign($$, gsMakeStateDelay());
    }
  | LPAR state_frm RPAR
    {
      safe_assign($$, $2);
    }
  ;

//regular formula
reg_frm:
  act_frm
    {
      safe_assign($$, $1);
    }
  | reg_frm_alt_naf
    {
      safe_assign($$, $1);
    }
  ;

//alternative composition, no action formula at top level (right associative)
reg_frm_alt_naf:
  reg_frm_seq_naf
    {
      safe_assign($$, $1);
    }
  | reg_frm_seq PLUS reg_frm_alt
    {
      safe_assign($$, gsMakeRegAlt($1, $3));
    }
  ;

//alternative composition (right assoiciative)
reg_frm_alt:
  reg_frm_seq
    {
      safe_assign($$, $1);
    }
  | reg_frm_seq PLUS reg_frm_alt
    {
      safe_assign($$, gsMakeRegAlt($1, $3));
    }
  ;

//sequential composition, no action formula at top level (right associative)
reg_frm_seq_naf:
  reg_frm_postfix_naf
    {
      safe_assign($$, $1);
    }
  | reg_frm_postfix DOT reg_frm_seq
    {
      safe_assign($$, gsMakeRegSeq($1, $3));
    }
  ;

//sequential composition (right associative)
reg_frm_seq:
  reg_frm_postfix
    {
      safe_assign($$, $1);
    }
  | reg_frm_postfix DOT reg_frm_seq
    {
      safe_assign($$, gsMakeRegSeq($1, $3));
    }
  ;

//postfix regular formula, no action formula at top level
reg_frm_postfix_naf:
  reg_frm_primary_naf
    {
      safe_assign($$, $1);
    }
  | reg_frm_postfix STAR
    {
      safe_assign($$, gsMakeRegTransOrNil($1));
    }
  | reg_frm_postfix PLUS
    {
      safe_assign($$, gsMakeRegTrans($1));
    }
  ;

//postfix regular formula
reg_frm_postfix:
  reg_frm_primary
    {
      safe_assign($$, $1);
    }
  | reg_frm_postfix STAR
    {
      safe_assign($$, gsMakeRegTransOrNil($1));
    }
  | reg_frm_postfix PLUS
    {
      safe_assign($$, gsMakeRegTrans($1));
    }
  ;

//primary regular formula, no action formula at top level
reg_frm_primary_naf:
  NIL
    {
      safe_assign($$, gsMakeRegNil());
    }
  | LPAR reg_frm_alt_naf RPAR
    {
      safe_assign($$, $2);
    }
  ;

//primary regular formula
reg_frm_primary:
  act_frm
    {
      safe_assign($$, $1);
    }
  | NIL
    {
      safe_assign($$, gsMakeRegNil());
    }
  | LPAR reg_frm_alt_naf RPAR
    {
      safe_assign($$, $2);
    }
  ;

//action formula
act_frm:
  act_frm_quant
    {
      safe_assign($$, $1);
    }
  ;

//quantification
act_frm_quant:
  act_frm_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT act_frm_quant
    {
      safe_assign($$, gsMakeActForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT act_frm_quant
    {
      safe_assign($$, gsMakeActExists($2, $4));
    }
  ;

//implication (right associative)
act_frm_imp:
  act_frm_and
    {
      safe_assign($$, $1);
    }
  | act_frm_and IMP act_frm_imp_rhs
    {
      safe_assign($$, gsMakeActImp($1, $3));
    }
  ;

//right argument of implication
act_frm_imp_rhs:
  act_frm_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT act_frm_imp_rhs
    {
      safe_assign($$, gsMakeActForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT act_frm_imp_rhs
    {
      safe_assign($$, gsMakeActExists($2, $4));
    }
  ;

//conjunction and disjunction (right associative)
act_frm_and:
  act_frm_at
    {
      safe_assign($$, $1);
    }
  | act_frm_prefix AND act_frm_and_rhs
    {
      safe_assign($$, gsMakeActAnd($1, $3));
    }
  | act_frm_prefix BARS act_frm_and_rhs
    {
      safe_assign($$, gsMakeActOr($1, $3));
    }
  ;

//right argument of conjunction and disjunction
act_frm_and_rhs:
  act_frm_and
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT act_frm_and_rhs
    {
      safe_assign($$, gsMakeActForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT act_frm_and_rhs
    {
      safe_assign($$, gsMakeActExists($2, $4));
    }
  ;

//at (left associative)
act_frm_at:
  act_frm_prefix
    {
      safe_assign($$, $1);
    }
  | act_frm_at AT data_expr_prefix
    {
      safe_assign($$, gsMakeActAt($1, $3));
    }
  ;

//prefix action formula
act_frm_prefix:
  act_frm_primary
    {
      safe_assign($$, $1);
    }
  | EXCLAM act_frm_quant_prefix
    {
      safe_assign($$, gsMakeActNot($2));
    }
  ;

//quantifier or prefix action formula
act_frm_quant_prefix:
  act_frm_prefix
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT act_frm_quant_prefix
    {
      safe_assign($$, gsMakeActForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT act_frm_quant_prefix
    {
      safe_assign($$, gsMakeActExists($2, $4));
    }
  ;

//primary action formula
act_frm_primary:
  mult_act
    {
      safe_assign($$, $1);
    }
  | VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
    }
  | CTRUE
    {
      safe_assign($$, gsMakeActTrue());
    }
  | CFALSE
    {
      safe_assign($$, gsMakeActFalse());
    }
  | LPAR act_frm RPAR
    {
      safe_assign($$, $2);
    }
  ;

//Action rename specifications
//----------------------------

//action rename specification
action_rename_spec:
  action_rename_spec_elts
    {
      safe_assign($$, gsActionRenameEltsToActionRename(ATreverse($1)));
    }
  ;

//action rename specification elements
action_rename_spec_elts:
  action_rename_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
   | action_rename_spec_elts action_rename_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
   ;

//action rename specification element
action_rename_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
    }
  | act_spec
    {
      safe_assign($$, $1);
    }
  | action_rename_rule_spec
    {
      safe_assign($$, $1);
    }
  ;

//action rename rule_specification
action_rename_rule_spec:
  action_rename_rule_sect
    {
      safe_assign($$, gsMakeActionRenameRules($1));
    }
  ;

//var section before action rename rules
action_rename_rule_sect:
  RENAME action_rename_rules_scs
    {
      safe_assign($$, $2);
    }
  | KWVAR data_vars_decls_scs RENAME action_rename_rules_scs
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($4);
      for (int i = 0; i < n; i++) {
        ATermAppl ActionRenameRule = ATAelementAt($4, i);
  safe_assign($$, ATinsert($$,
          (ATerm) gsMakeActionRenameRule($2,
            ATAgetArgument(ActionRenameRule, 1),
            ATAgetArgument(ActionRenameRule, 2),
            ATAgetArgument(ActionRenameRule, 3))));
      }
    }


//action rename rules comma separated
action_rename_rules_scs:
  action_rename_rule SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | action_rename_rules_scs action_rename_rule SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
  ;

//action rename rule
action_rename_rule:
  data_expr ARROW param_id IMP action_rename_rule_rhs
    {
      safe_assign($$, gsMakeActionRenameRule(ATmakeList0(), $1, $3, $5));
    }
  | param_id IMP action_rename_rule_rhs
    {
      safe_assign($$, gsMakeActionRenameRule(ATmakeList0(), mcrl2::data::sort_bool::true_(), $1, $3));
    }
  ;

//right-hand side of an action rename rule
action_rename_rule_rhs:
  param_id
    {
      safe_assign($$, $1);
    }
  | proc_constant
    {
      safe_assign($$, $1);
    }
  ;

//Parameterised boolean expressions
//---------------------------------

//parameterised boolean expression
pb_expr:
  pb_expr_quant
    {
      safe_assign($$, $1);
    }
  ;

//quantification
pb_expr_quant:
  pb_expr_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT pb_expr_quant
    {
      safe_assign($$, gsMakePBESForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_quant
    {
      safe_assign($$, gsMakePBESExists($2, $4));
    }
  ;

//implication (right associative)
pb_expr_imp:
  pb_expr_and
    {
      safe_assign($$, $1);
    }
  | pb_expr_and IMP pb_expr_imp_rhs
    {
      safe_assign($$, gsMakePBESImp($1, $3));
    }
  ;

//right argument of implication
pb_expr_imp_rhs:
  pb_expr_imp
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT pb_expr_imp_rhs
    {
      safe_assign($$, gsMakePBESForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_imp_rhs
    {
      safe_assign($$, gsMakePBESExists($2, $4));
    }
  ;

//conjunction and disjunction (right associative)
pb_expr_and:
  pb_expr_not
    {
      safe_assign($$, $1);
    }
  | pb_expr_not AND pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESAnd($1, $3));
    }
  | pb_expr_not BARS pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESOr($1, $3));
    }
  ;

//right argument of conjunction and disjunction
pb_expr_and_rhs:
  pb_expr_and
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESExists($2, $4));
    }
  ;

//negation
pb_expr_not:
  pb_expr_primary
    {
      safe_assign($$, $1);
    }
  | EXCLAM pb_expr_quant_not
    {
      safe_assign($$, gsMakePBESNot($2));
    }
  ;

//quantifier or not
pb_expr_quant_not:
  pb_expr_not
    {
      safe_assign($$, $1);
    }
  | FORALL data_vars_decls_cs DOT pb_expr_quant_not
    {
      safe_assign($$, gsMakePBESForall($2, $4));
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_quant_not
    {
      safe_assign($$, gsMakePBESExists($2, $4));
    }
  ;

//primary expression
pb_expr_primary:
  VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
    }
  | param_id
    {
      safe_assign($$, gsMakePropVarInst(ATAgetArgument($1, 0), ATLgetArgument($1, 1)));
    }
  | CTRUE
    {
      safe_assign($$, gsMakePBESTrue());
    }
  | CFALSE
    {
      safe_assign($$, gsMakePBESFalse());
    }
  | LPAR pb_expr RPAR
    {
      safe_assign($$, $2);
    }
  ;


//PBES's
//------

//PBES specification
pbes_spec:
  pbes_spec_elts
    {
      safe_assign($$, gsPBESSpecEltsToSpec(ATreverse($1)));
    }
  ;

//PBES specification elements
pbes_spec_elts:
  pbes_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
   | pbes_spec_elts pbes_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
   ;

//PBES specification element
pbes_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
    }
  | glob_var_spec
    {
      safe_assign($$, $1);
    }
  | pb_eqn_spec
    {
      safe_assign($$, $1);
    }
  | pb_init
    {
      safe_assign($$, $1);
    }
  ;

//parameterised boolean equation specification
pb_eqn_spec:
  KWPBES pb_eqn_decls_scs
    {
      safe_assign($$, gsMakePBEqnSpec(ATreverse($2)));
    }
  ;

//one or more parameterised boolean equation declarations, separated by semicolons
pb_eqn_decls_scs:
  pb_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
    }
  | pb_eqn_decls_scs pb_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
    }
  ;

//parameterised boolean equation declaration
pb_eqn_decl:
  fixpoint STRING EQUALS pb_expr
    {
      safe_assign($$,
        gsMakePBEqn($1, gsMakePropVarDecl($2, ATmakeList0()), $4));
    }
  | fixpoint STRING LPAR data_vars_decls_cs RPAR EQUALS pb_expr
    {
      safe_assign($$,
        gsMakePBEqn($1, gsMakePropVarDecl($2, $4), $7));
    }
  ;

//fixpoint
fixpoint:
  MU
    {
      safe_assign($$, gsMakeMu());
    }
  | NU
    {
      safe_assign($$, gsMakeNu());
    }
  ;

//parameterised boolean initialisation
pb_init:
  KWINIT param_id SEMICOLON
    {
      safe_assign($$,
        gsMakePBInit(gsMakePropVarInst(ATAgetArgument($2, 0), ATLgetArgument($2, 1))));
    }
  ;
'''

START_GRAMMAR = '''
start:
    TAG_IDENTIFIER STRING
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
  | KWSTRUCT StructCons_list_bar_separated
  ;

StructCons_list_bar_separated:
    StructCons
  | StructCons_list_bar_separated BAR StructCons
  ;

StructCons:
    STRING StructLabel
  | STRING LPAR StructProj_list_comma_separated RPAR StructLabel
  ;

StructLabel:
    /* empty */
  | QMARK STRING
  ;

StructProj_list_comma_separated:
    StructProj
  | StructProj_list_comma_separated COMMA StructProj
  ;

StructProj:
    sort_expr
  | STRING COLON sort_expr
  ;

sort_expr_primary:
    STRING
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
  STRING EQUALS data_expr
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
    STRING
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
  STRING COLON sort_expr
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
  | STRING EQUALS sort_expr
  ;

ids_cs:
    STRING
  | ids_cs COMMA STRING
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
    STRING
  | STRING LPAR data_exprs_cs RPAR
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
    STRING LPAR RPAR
  | STRING LPAR id_inits_cs RPAR
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
  STRING ARROW STRING
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
  | comm_expr_lhs ARROW STRING
  ;

comm_expr_lhs:
  STRING BAR ids_bs
  ;

ids_bs:
    STRING
  | ids_bs BAR STRING
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
    STRING EQUALS proc_expr
  | STRING LPAR data_vars_decls_cs RPAR EQUALS proc_expr
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
  | NU STRING fixpoint_params DOT state_frm_quant
  | MU STRING fixpoint_params DOT state_frm_quant
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
  STRING COLON sort_expr EQUALS data_expr
  ;

state_frm_imp:
    state_frm_and
  | state_frm_and IMP state_frm_imp_rhs
  ;

state_frm_imp_rhs:
    state_frm_imp
  | FORALL data_vars_decls_cs DOT state_frm_imp_rhs
  | EXISTS data_vars_decls_cs DOT state_frm_imp_rhs
  | NU STRING fixpoint_params DOT state_frm_imp_rhs
  | MU STRING fixpoint_params DOT state_frm_imp_rhs
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
  | NU STRING fixpoint_params DOT state_frm_and_rhs
  | MU STRING fixpoint_params DOT state_frm_and_rhs
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
  | NU STRING fixpoint_params DOT state_frm_quant_prefix
  | MU STRING fixpoint_params DOT state_frm_quant_prefix
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
    fixpoint STRING EQUALS pb_expr
  | fixpoint STRING LPAR data_vars_decls_cs RPAR EQUALS pb_expr
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
    insert_text_in_file(yaccfile, text, 'generated terminals')

    # generate non-terminals
    text = ''
    text = text + parse_nonterminals(NONTERMINAL_TERMS, 'term')
    text = text + parse_nonterminals(NONTERMINAL_FUNCTION_APPLICATIONS, 'appl')
    text = text + parse_nonterminals(NONTERMINAL_LISTS, 'list')
    insert_text_in_file(yaccfile, text, 'generated non-terminals')
    insert_text_in_file(yaccfile, MCRL2_GRAMMAR, 'generated parser')

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
os.system('bison -p mcrl3 --defines=../../core/include/mcrl2/core/detail/mcrl3parser.h -o mcrl2parser.cpp mcrl2parser.yy')
os.system('sed -i \'s+#include "mcrl3parser.h"+#include "mcrl2/core/detail/mcrl3parser.h"+\' mcrl2parser.cpp')
os.system("sed -i '/isatty/d' mcrl2lexer.cpp")
