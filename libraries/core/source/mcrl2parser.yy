// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2parser.yy

//Remarks
//-------
//
//For reasons of efficiency recursive grammar rules are left-recursive if
//possible. This implies that lists are parsed from right to left. For the
//efficient use of the ATerm library parsed lists are stored in an ATermList
//in reverse order. When the lists are completely parsed they are reversed
//once.

%{

#include <stdio.h>
#include <string.h>

#include <aterm2.h>
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/standard_utility.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.ll
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATerm mcrl2_spec_tree;
extern ATermIndexedSet mcrl2_parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 640000

//local declarations
ATermAppl gsDataSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains zero or more occurrences of sort, constructor,
//     operation and data equation specifications.
//Ret: data specification containing one sort, constructor, operation,
//     and data equation specification, in that order.

ATermAppl gsProcSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one process initialisation and zero or more
//     occurrences of sort, constructor, operation, data equation, action and
//     process equation specifications.
//Ret: process specification containing one sort, constructor, operation,
//     data equation, action and process equation specification, and one
//     process initialisation, in that order.

ATermAppl gsActionRenameEltsToActionRename(ATermList SpecElts);
//Pre: ActionRenameElts contains zero or more occurrences of
//     sort, constructor, operation, equation, action and action rename
//     rules.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and action rename rules in that order.

ATermAppl gsPBESSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one parameterised boolean initialisation and zero or
//     more occurrences of sort, constructor, operation, data equation, action
//     and parameterised boolean equation specifications.
//Ret: BPES specification containing one sort, constructor, operation,
//     data equation, action and parameterised boolean equation specification,
//     and one parameterised boolean initialisation, in that order.

#define safe_assign(lhs, rhs) { ATbool b; lhs = rhs; ATindexedSetPut(mcrl2_parser_protect_table, (ATerm) lhs, &b); }
%}

%union {
  ATerm term;
  ATermAppl appl;
  ATermList list;
};

//generate a GLR parser
%glr-parser

//more verbose and specific error messages
%error-verbose

//set name prefix
%name-prefix="mcrl2yy"

//start token
%start start

//Uncomment the line below to enable the bison debug facilities.
//To produce traces, yydebug needs to be set to 1 (see mcrl2lexer.ll)
//%debug

//terminals
//---------

%token <appl> TAG_IDENTIFIER TAG_SORT_EXPR TAG_DATA_EXPR TAG_DATA_SPEC
%token <appl> TAG_MULT_ACT TAG_PROC_EXPR TAG_PROC_SPEC TAG_PBES_SPEC
%token <appl> TAG_STATE_FRM TAG_DATA_VARS TAG_ACTION_RENAME
%token <appl> LMERGE ARROW LTE GTE CONS SNOC CONCAT EQ NEQ AND BARS IMP BINIT
%token <appl> ELSE
%token <appl> SLASH STAR PLUS MINUS EQUALS DOT COMMA COLON SEMICOLON QMARK
%token <appl> EXCLAM AT HASH BAR
%token <appl> LPAR RPAR LBRACK RBRACK LANG RANG LBRACE RBRACE
%token <appl> KWSORT KWCONS KWMAP KWVAR KWEQN KWACT KWGLOB KWPROC KWPBES KWINIT
%token <appl> KWSTRUCT BOOL POS NAT INT REAL LIST SET BAG
%token <appl> CTRUE CFALSE IF DIV MOD IN LAMBDA FORALL EXISTS WHR END
%token <appl> DELTA TAU SUM BLOCK ALLOW HIDE RENAME COMM
%token <appl> VAL MU NU DELAY YALED NIL
%token <appl> ID NUMBER

//non-terminals
//-------------

//start
%type <term> start

//sort expressions
%type <appl> sort_expr sort_expr_arrow domain_no_arrow_elt sort_expr_struct
%type <appl> struct_constructor recogniser struct_projection sort_expr_primary
%type <appl> sort_constant sort_constructor
//data expressions
%type <appl> data_expr
%type <appl> data_expr_whr id_init
%type <appl> data_expr_quant
%type <appl> data_expr_imp data_expr_imp_rhs
%type <appl> data_expr_and data_expr_and_rhs
%type <appl> data_expr_eq data_expr_eq_rhs
%type <appl> data_expr_rel
%type <appl> data_expr_cons data_expr_snoc data_expr_concat
%type <appl> data_expr_add data_expr_div data_expr_mult
%type <appl> data_expr_prefix data_expr_quant_prefix
%type <appl> data_expr_postfix
%type <appl> data_expr_primary
%type <appl> data_constant data_enumeration data_comprehension
%type <appl> data_var_decl
//data specifications
%type <appl> data_spec data_spec_elt sort_spec cons_spec map_spec
%type <appl> data_eqn_spec data_eqn_decl
//multi-actions
%type <appl> mult_act param_id
//process expressions
%type <appl> proc_expr proc_expr_choice proc_expr_sum proc_expr_merge
%type <appl> proc_expr_merge_rhs proc_expr_binit proc_expr_binit_rhs
%type <appl> proc_expr_cond proc_expr_cond_la proc_expr_seq
%type <appl> proc_expr_seq_wo_cond proc_expr_seq_rhs proc_expr_seq_rhs_wo_cond
%type <appl> proc_expr_at proc_expr_at_wo_cond proc_expr_sync
%type <appl> proc_expr_sync_wo_cond proc_expr_sync_rhs
%type <appl> proc_expr_sync_rhs_wo_cond proc_expr_primary proc_constant
%type <appl> id_assignment proc_quant ren_expr comm_expr comm_expr_lhs
%type <appl> mult_act_name
//process specifications
%type <appl> proc_spec proc_spec_elt act_spec glob_var_spec proc_eqn_spec
%type <appl> proc_eqn_decl proc_init
//state formulas
%type <appl> state_frm state_frm_quant state_frm_imp state_frm_imp_rhs
%type <appl> state_frm_and state_frm_and_rhs state_frm_prefix
%type <appl> state_frm_quant_prefix state_frm_primary
%type <appl> data_var_decl_init
%type <appl> reg_frm reg_frm_alt_naf reg_frm_alt reg_frm_seq_naf reg_frm_seq
%type <appl> reg_frm_postfix_naf reg_frm_postfix
%type <appl> reg_frm_primary_naf reg_frm_primary
%type <appl> act_frm act_frm_quant act_frm_imp act_frm_imp_rhs
%type <appl> act_frm_and act_frm_and_rhs act_frm_at act_frm_prefix
%type <appl> act_frm_quant_prefix act_frm_primary
//action rename
%type <appl> action_rename_spec action_rename_spec_elt action_rename_rule_spec
%type <appl> action_rename_rule action_rename_rule_rhs
//parameterised boolean expressions
%type <appl> pb_expr pb_expr_quant pb_expr_imp pb_expr_imp_rhs pb_expr_and
%type <appl> pb_expr_and_rhs pb_expr_not pb_expr_quant_not pb_expr_primary
//PBES's
%type <appl> pbes_spec pbes_spec_elt pb_eqn_spec pb_eqn_decl
%type <appl> fixpoint pb_init

//sort expressions
%type <list> domain_no_arrow domain_no_arrow_elts_hs struct_constructors_bs
%type <list> struct_projections_cs
//data expressions
%type <list> bag_enum_elt
%type <list> id_inits_cs data_exprs_cs bag_enum_elts_cs data_vars_decls_cs
%type <list> data_vars_decl
//data specifications
%type <list> data_spec_elts ids_cs sorts_decls_scs sorts_decl domain
%type <list> ops_decls_scs ops_decl data_eqn_sect data_eqn_decls_scs
%type <list> data_vars_decls_scs
//process expressions
%type <list> act_names_set ren_expr_set ren_exprs_cs comm_expr_set comm_exprs_cs
%type <list> mult_act_names_set mult_act_names_cs ids_bs
//multi-actions
%type <list> param_ids_bs
//process specifications
%type <list> proc_spec_elts acts_decls_scs acts_decl proc_eqn_decls_scs
//state formulas
%type <list> fixpoint_params data_var_decl_inits_cs
//action rename
%type <list> action_rename_spec_elts action_rename_rules_scs
%type <list> action_rename_rule_sect
//PBES's
%type <list> pbes_spec_elts pb_eqn_decls_scs


%%


//Start
//-----

//start
start:
  TAG_IDENTIFIER ID
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_SORT_EXPR sort_expr
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_DATA_EXPR data_expr
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_DATA_SPEC data_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_MULT_ACT mult_act
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_PROC_EXPR proc_expr
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_PROC_SPEC proc_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_STATE_FRM state_frm
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_ACTION_RENAME action_rename_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_PBES_SPEC pbes_spec
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  | TAG_DATA_VARS data_vars_decls_scs
    {
      safe_assign($$, (ATerm) $2);
      mcrl2_spec_tree = $$;
    }
  ;

//Sort expressions
//----------------

//sort expression
sort_expr:
  sort_expr_arrow
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed sort expression\n  %T\n", $$);
    }
  ;

//arrow sort expression
sort_expr_arrow:
  sort_expr_struct
    {
      safe_assign($$, $1);
    }
  | domain_no_arrow ARROW sort_expr_arrow
    {
      safe_assign($$, gsMakeSortArrow($1, $3));
      gsDebugMsg("parsed arrow sort\n  %T\n", $$);
    }
  ;

//domain
domain_no_arrow:
  domain_no_arrow_elts_hs
    {
      safe_assign($$, ATreverse($1));
      gsDebugMsg("parsed non-arrow domain\n  %T\n", $$);
    }
  ;

//one or more domain elements, separated by hashes
domain_no_arrow_elts_hs:
  domain_no_arrow_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", $$);
    }
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", $$);
    }
  ;

//domain element
domain_no_arrow_elt:
  sort_expr_struct
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", $$);
    }
  ;

//structured sort
sort_expr_struct:
  sort_expr_primary
    {
      safe_assign($$, $1);
    }
  | KWSTRUCT struct_constructors_bs
    {
      safe_assign($$, gsMakeSortStruct(ATreverse($2)));
      gsDebugMsg("parsed structured sort\n  %T\n", $$);
    }
  ;

//one ore more structured sort constructors, separated by bars
struct_constructors_bs:
  struct_constructor
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", $$);
    }
  | struct_constructors_bs BAR struct_constructor
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed structured sort constructors\n  %T\n", $$);
    }
  ;

//structured sort constructor
struct_constructor:
  ID recogniser
    {
      safe_assign($$, gsMakeStructCons($1, ATmakeList0(), $2));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", $$);
    }
  | ID LPAR struct_projections_cs RPAR recogniser
    {
      safe_assign($$, gsMakeStructCons($1, ATreverse($3), $5));
      gsDebugMsg("parsed structured sort constructor\n  %T\n", $$);
    }
  ;

//recogniser
recogniser:
  /* empty */
    {
      safe_assign($$, gsMakeNil());
      gsDebugMsg("parsed recogniser\n  %T\n", $$);
    }
  | QMARK ID
    {
      safe_assign($$, $2);
      gsDebugMsg("parsed recogniser id\n  %T\n", $$);
    }
  ;

//one or more structured sort projections, separated by comma's
struct_projections_cs:
  struct_projection
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed structured sort projections\n  %T\n", $$);
    }
  | struct_projections_cs COMMA struct_projection
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed structured sort projections\n  %T\n", $$);
    }
  ;

//structured sort projection
struct_projection:
  sort_expr
    {
      safe_assign($$, gsMakeStructProj(gsMakeNil(), $1));
      gsDebugMsg("parsed structured sort projection\n  %T\n", $$);
    }
  | ID COLON sort_expr
    {
      safe_assign($$, gsMakeStructProj($1, $3));
      gsDebugMsg("parsed structured sort projection\n  %T\n", $$);
    }
  ;

//primary sort expression
sort_expr_primary:
  ID
    {
      safe_assign($$, gsMakeSortId($1));
      gsDebugMsg("parsed primary sort\n  %T\n", $$);
    }
  | sort_constant
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed sort constant\n  %T\n", $$);
    }
  | sort_constructor
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed sort constructor\n  %T\n", $$);
    }
  | LPAR sort_expr RPAR
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
  LIST LPAR sort_expr RPAR
    {
      safe_assign($$, mcrl2::data::sort_list::list(mcrl2::data::sort_expression($3)));
    }
  | SET LPAR sort_expr RPAR
    {
      safe_assign($$, mcrl2::data::sort_set::set_(mcrl2::data::sort_expression($3)));
    }
  | BAG LPAR sort_expr RPAR
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
      gsDebugMsg("parsed data expression\n  %T\n", $$);
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
      gsDebugMsg("parsed where clause\n  %T\n", $$);
    }
  ;

//declaration of one or more identifier initialisations, separated by comma's
id_inits_cs:
  id_init
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed identifier initialisations\n  %T\n", $$);
    }
  | id_inits_cs COMMA id_init
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed identifier initialisations\n  %T\n", $$);
    }
  ;

//identifier initialisation
id_init:
  ID EQUALS data_expr
    {
      safe_assign($$, gsMakeIdInit($1, $3));
      gsDebugMsg("parsed identifier initialisation\n  %T\n", $$);
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
      gsDebugMsg("parsed lambda abstraction\n  %T\n", $$);
    }
  | FORALL data_vars_decls_cs DOT data_expr_quant
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more data variables, separated by
//comma's
data_vars_decls_cs:
  data_vars_decl
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  | data_vars_decls_cs COMMA data_vars_decl
    {
      safe_assign($$, ATconcat($1, $3));
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//declaration of one or more data variables
data_vars_decl:
  ids_cs COLON sort_expr
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeDataVarId(ATAelementAt($1, i), $3)));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
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
      gsDebugMsg("parsed implication\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_imp_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | data_expr_eq BARS data_expr_and_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed disjunction\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_and_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
    }
  | data_expr_rel NEQ data_expr_eq_rhs
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
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
      gsDebugMsg("parsed lambda abstraction\n  %T\n", $$);
    }
  | FORALL data_vars_decls_cs DOT data_expr_eq_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeForall(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_eq_rhs
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LTE data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons RANG data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LANG data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons IN data_expr_cons
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
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
      gsDebugMsg("parsed list cons expression\n  %T\n", $$);
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
      gsDebugMsg("parsed list snoc expression\n  %T\n", $$);
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
      gsDebugMsg("parsed list concat expression\n  %T\n", $$);
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
      gsDebugMsg("parsed addition or set union\n  %T\n", $$);
    }
  | data_expr_add MINUS data_expr_div
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", $$);
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
      gsDebugMsg("parsed div expression\n  %T\n", $$);
    }
  | data_expr_div MOD data_expr_mult
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed mod expression\n  %T\n", $$);
    }
  | data_expr_div SLASH data_expr_mult
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed division expression\n  %T\n", $$);
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
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", $$);
    }
  | data_expr_mult DOT data_expr_prefix
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId($2), ATmakeList2((ATerm) $1, (ATerm) $3)));
      gsDebugMsg("parsed list at expression\n  %T\n", $$);
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
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | MINUS data_expr_prefix
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId($1), ATmakeList1((ATerm) $2)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | HASH data_expr_prefix
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId($1), ATmakeList1((ATerm) $2)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant_prefix
    {
      safe_assign($$, gsMakeBinder(gsMakeExists(), $2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed postfix data expression (function application)\n  %T\n", $$);
    }
/*  | data_expr_postfix LBRACK data_expr ARROW data_expr RBRACK
    {
      safe_assign($$,
        gsMakeDataAppl(gsMakeId(gsMakeOpIdNameFuncUpdate()), ATmakeList3((ATerm) $1, (ATerm) $3, (ATerm) $5)));
      gsDebugMsg("parsed postfix data expression (function update)\n  %T\n", $$);
    } */
  ;

//one or more data expressions, separated by comma's
data_exprs_cs:
  data_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed data expressions\n  %T\n", $$);
    }
  | data_exprs_cs COMMA data_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed data expressions\n  %T\n", $$);
    }
  ;

//primary data expression
data_expr_primary:
  ID
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed primary data expression\n  %T\n", $$);
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
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | CFALSE
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | IF
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | NUMBER
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | LBRACK RBRACK
    {
      safe_assign($$, gsMakeId(mcrl2::data::sort_list::nil_name()));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | LBRACE RBRACE
    {
      safe_assign($$, gsMakeId(mcrl2::data::sort_set::emptyset_name()));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs_cs RBRACK
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_list::list_enumeration_name()), ATreverse($2)));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE data_exprs_cs RBRACE
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_set::set_enumeration_name()), ATreverse($2)));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE bag_enum_elts_cs RBRACE
    {
      safe_assign($$, gsMakeDataAppl(gsMakeId(mcrl2::data::sort_bag::bag_enumeration_name()), ATreverse($2)));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  ;

//one or more bag enumeration elements, separated by comma's
bag_enum_elts_cs:
  bag_enum_elt
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  | bag_enum_elts_cs COMMA bag_enum_elt
    {
      safe_assign($$, ATconcat($3, $1));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
    {
      safe_assign($$, ATmakeList2((ATerm) $3, (ATerm) $1));
      gsDebugMsg("parsed bag enumeration element\n  %T\n", $$);
    }
  ;

//comprehension
data_comprehension:
  LBRACE data_var_decl BAR data_expr RBRACE
    {
      safe_assign($$, gsMakeBinder(gsMakeSetBagComp(), ATmakeList1((ATerm) $2), $4));
      gsDebugMsg("parsed data comprehension\n  %T\n", $$);
    }
  ;

//declaration of a data variable
data_var_decl:
  ID COLON sort_expr
    {
      safe_assign($$, gsMakeDataVarId($1, $3));
      gsDebugMsg("parsed data variable declaration\n  %T\n", $$);
    }
  ;

//Data specifications
//-------------------

//data specification
data_spec:
  data_spec_elts
    {
      safe_assign($$, gsDataSpecEltsToSpec(ATreverse($1)));
      gsDebugMsg("parsed data specification\n  %T\n", $$);
    }
  ;

//data specification elements
data_spec_elts:
  data_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed data specification elements\n  %T\n", $$);
    }
   | data_spec_elts data_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed data specification elements\n  %T\n", $$);
    }
   ;

//data specification element
data_spec_elt:
  sort_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data specification element\n  %T\n", $$);
    }
  | cons_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data specification element\n  %T\n", $$);
    }
  | map_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data specification element\n  %T\n", $$);
    }
  | data_eqn_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data specification element\n  %T\n", $$);
    }
  ;

//sort specification
sort_spec:
  KWSORT sorts_decls_scs
    {
      safe_assign($$, gsMakeSortSpec($2));
      gsDebugMsg("parsed sort specification\n  %T\n", $$);
    }
  ;

//declaration of one or more sorts, separated by semicolons
sorts_decls_scs:
  sorts_decl SEMICOLON
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed sort declarations\n  %T\n", $$);
    }
  | sorts_decls_scs sorts_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
      gsDebugMsg("parsed sort declarations\n  %T\n", $$);
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
      gsDebugMsg("parsed standard sort declarations\n  %T\n", $$);
    }
  | ID EQUALS sort_expr
    {
      safe_assign($$, ATmakeList1((ATerm) gsMakeSortRef($1, $3)));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", $$);
    }
  ;

//one or more identifiers, separated by comma's
ids_cs:
  ID
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  | ids_cs COMMA ID
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  ;

//domain
domain:
  domain_no_arrow
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed domain\n  %T\n", $$);
    }
  | domain_no_arrow ARROW sort_expr
    {
      safe_assign($$, ATmakeList1((ATerm) gsMakeSortArrow($1, $3)));
      gsDebugMsg("parsed domain\n  %T\n", $$);
    }
  ;

//constructor operation specification
cons_spec:
  KWCONS ops_decls_scs
    {
      safe_assign($$, gsMakeConsSpec($2));
      gsDebugMsg("parsed constructor operation specification\n  %T\n", $$);
    }
  ;

//operation specification
map_spec:
  KWMAP ops_decls_scs
    {
      safe_assign($$, gsMakeMapSpec($2));
      gsDebugMsg("parsed operation specification\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more operations of the same sort,
//separated by semicolons
ops_decls_scs:
  ops_decl SEMICOLON
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  | ops_decls_scs ops_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  ;

//declaration of one or more operations of the same sort
ops_decl:
  ids_cs COLON sort_expr
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeOpId(ATAelementAt($1, i), $3)));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  ;

//data equation specification
data_eqn_spec:
  data_eqn_sect
    {
      safe_assign($$, gsMakeDataEqnSpec($1));
      gsDebugMsg("parsed data equation specification\n  %T\n", $$);
    }
  ;

//data equation section
data_eqn_sect:
  KWEQN data_eqn_decls_scs
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed data equation section\n  %T\n", $$);
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
      gsDebugMsg("parsed data equation section\n  %T\n", $$);
    }
  ;

//declaration of one or more data equations, separated by semicolons
data_eqn_decls_scs:
  data_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed data equation declarations\n  %T\n", $$);
    }
  | data_eqn_decls_scs data_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed equation declarations\n  %T\n", $$);
    }
  ;

//data equation declaration
data_eqn_decl:
  data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), mcrl2::data::sort_bool::true_(), $1, $3));
      gsDebugMsg("parsed data equation declaration\n  %T\n", $$);
    }
  | data_expr ARROW data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), $1, $3, $5));
      gsDebugMsg("parsed data equation declaration\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more data variables,
//separated by semicolons
data_vars_decls_scs:
  data_vars_decl SEMICOLON
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  | data_vars_decls_scs data_vars_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//Multi-actions
//-------------

//multi-action
mult_act:
  param_ids_bs
    {
      safe_assign($$, gsMakeMultAct(ATreverse($1)));
      gsDebugMsg("parsed multi-action\n  %T\n", $$);
    }
  | TAU
    {
      safe_assign($$, gsMakeMultAct(ATmakeList0()));
      gsDebugMsg("parsed multi-action\n  %T\n", $$);
    }
  ;

//one or more parameterised id's, separated by bars
param_ids_bs:
  param_id
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed parameterised id's\n  %T\n", $$);
    }
  | param_ids_bs BAR param_id
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed parameterised id's\n  %T\n", $$);
    }
  ;

//parameterised id
param_id:
  ID
    {
      safe_assign($$, gsMakeParamId($1, ATmakeList0()));
      gsDebugMsg("parsed action or process\n  %T\n", $$);
    }
  | ID LPAR data_exprs_cs RPAR
    {
      safe_assign($$, gsMakeParamId($1, ATreverse($3)));
      gsDebugMsg("parsed action or process\n  %T\n", $$);
    }
  ;

//Process expressions
//-------------------

//process expression
proc_expr:
  proc_expr_choice
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process expression\n  %T\n", $$);
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
      gsDebugMsg("parsed choice expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed merge expression\n  %T\n", $$);
    }
  | proc_expr_binit LMERGE proc_expr_merge_rhs
    {
      safe_assign($$, gsMakeLMerge($1, $3));
      gsDebugMsg("parsed left merge expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_cond_la
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed sequential expression\n  %T\n", $$);
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
      gsDebugMsg("parsed sequential expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeIfThen($1, $3));
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_seq_rhs
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed at time expression\n  %T\n", $$);
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
      gsDebugMsg("parsed at time expression\n  %T\n", $$);
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
      gsDebugMsg("parsed sync expression\n  %T\n", $$);
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
      gsDebugMsg("parsed sync expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeIfThen($1, $3));
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs_wo_cond ELSE proc_expr_sync_rhs
    {
      safe_assign($$, gsMakeIfThenElse($1, $3, $5));
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      //mcrl2yyerror("process assignments are not yet supported");
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
      gsDebugMsg("parsed process constant\n  %T\n", $$);
    }
  | TAU
    {
      safe_assign($$, gsMakeTau());
      gsDebugMsg("parsed process constant\n  %T\n", $$);
    }
  ;

//identifier assignment
id_assignment:
  ID LPAR RPAR
    {
      safe_assign($$, gsMakeIdAssignment($1, ATmakeList0()));
      gsDebugMsg("parsed identifier assignment\n  %T\n", $$);
    }
  | ID LPAR id_inits_cs RPAR
    {
      safe_assign($$, gsMakeIdAssignment($1, ATreverse($3)));
      gsDebugMsg("parsed identifier assignment\n  %T\n", $$);
    }
  ;

//process quantification
proc_quant:
  BLOCK LPAR act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeBlock($3, $5));
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | HIDE LPAR act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeHide($3, $5));
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeRename($3, $5));
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeComm($3, $5));
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | ALLOW LPAR mult_act_names_set COMMA proc_expr RPAR
    {
      safe_assign($$, gsMakeAllow($3, $5));
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  ;

//set of action names
act_names_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
      gsDebugMsg("parsed action name set\n  %T\n", $$);
    }
  | LBRACE ids_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed action name set\n  %T\n", $$);
    }
  ;

//set of renaming expressions
ren_expr_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
      gsDebugMsg("parsed renaming expression set\n  %T\n", $$);
    }
  | LBRACE ren_exprs_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed renaming expression set\n  %T\n", $$);
    }
  ;

//one or more renaming expressions, separated by comma's
ren_exprs_cs:
  ren_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed renaming expressions\n  %T\n", $$);
    }
  | ren_exprs_cs COMMA ren_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed renaming expressions\n  %T\n", $$);
    }
  ;

//renaming expression
ren_expr:
  ID ARROW ID
    {
      safe_assign($$, gsMakeRenameExpr($1, $3));
      gsDebugMsg("parsed renaming expression\n  %T\n", $$);
    }
  ;

//set of communication expressions
comm_expr_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
      gsDebugMsg("parsed communication expression set\n  %T\n", $$);
    }
  | LBRACE comm_exprs_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed communication expression set\n  %T\n", $$);
    }
  ;

//one or more communication expressions, separated by comma's
comm_exprs_cs:
  comm_expr
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed communication expressions\n  %T\n", $$);
    }
  | comm_exprs_cs COMMA comm_expr
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed communication expressions\n  %T\n", $$);
    }
  ;

//communication expression
comm_expr:
  comm_expr_lhs
    {
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | comm_expr_lhs ARROW TAU
    {
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | comm_expr_lhs ARROW ID
    {
      safe_assign($$, gsMakeCommExpr($1, $3));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  ;

//left-hand side of a communication expression
comm_expr_lhs:
  ID BAR ids_bs
    {
      safe_assign($$, gsMakeMultActName(ATinsert(ATreverse($3), (ATerm) $1)));
      gsDebugMsg("parsed left-hand side of communication expression\n  %T\n", $$);
    }
  ;

//one or more id's, separated by bars
ids_bs:
  ID
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  | ids_bs BAR ID
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  ;

//set of multi action names
mult_act_names_set:
  LBRACE RBRACE
    {
      safe_assign($$, ATmakeList0());
      gsDebugMsg("parsed multi action name set\n  %T\n", $$);
    }
  | LBRACE mult_act_names_cs RBRACE
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed multi action name set\n  %T\n", $$);
    }
  ;

//one or more multi action names, separated by comma's
mult_act_names_cs:
  mult_act_name
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed multi action names\n  %T\n", $$);
    }
  | mult_act_names_cs COMMA mult_act_name
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed multi action names\n  %T\n", $$);
    }
  ;

//multi action name
mult_act_name:
  ids_bs
    {
      safe_assign($$, gsMakeMultActName(ATreverse($1)));
      gsDebugMsg("parsed multi action name\n  %T\n", $$);
    }
  ;

//Process specifications
//----------------------

//process specification
proc_spec:
  proc_spec_elts
    {
      safe_assign($$, gsProcSpecEltsToSpec(ATreverse($1)));
      gsDebugMsg("parsed process specification\n  %T\n", $$);
    }
  ;

//process specification elements
proc_spec_elts:
  proc_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed process specification elements\n  %T\n", $$);
    }
   | proc_spec_elts proc_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed process specification elements\n  %T\n", $$);
    }
   ;

//process specification element
proc_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process specification element\n  %T\n", $$);
    }
  | act_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process specification element\n  %T\n", $$);
    }
  | glob_var_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process specification element\n  %T\n", $$);
    }
  | proc_eqn_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process specification element\n  %T\n", $$);
    }
  | proc_init
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed process specification element\n  %T\n", $$);
    }
  ;

//action specification
act_spec:
  KWACT acts_decls_scs
    {
      safe_assign($$, gsMakeActSpec($2));
      gsDebugMsg("parsed action specification\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more actions, separated by semicolons
acts_decls_scs:
  acts_decl SEMICOLON
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  | acts_decls_scs acts_decl SEMICOLON
    {
      safe_assign($$, ATconcat($1, $2));
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
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
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  | ids_cs COLON domain
    {
      safe_assign($$, ATmakeList0());
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        safe_assign($$, ATinsert($$, (ATerm) gsMakeActId(ATAelementAt($1, i), $3)));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  ;

//global variable specification
glob_var_spec:
  KWGLOB data_vars_decls_scs
    {
      safe_assign($$, gsMakeGlobVarSpec($2));
      gsDebugMsg("parsed global variables\n  %T\n", $$);
    }
  ;

//process equation specification
proc_eqn_spec:
  KWPROC proc_eqn_decls_scs
    {
      safe_assign($$, gsMakeProcEqnSpec(ATreverse($2)));
      gsDebugMsg("parsed process equation specification\n  %T\n", $$);
    }
  ;

//one or more process equation declarations, separated by semicolons
proc_eqn_decls_scs:
  proc_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed process equation declarations\n  %T\n", $$);
    }
  | proc_eqn_decls_scs proc_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed process equation declarations\n  %T\n", $$);
    }
  ;

//process equation declaration
proc_eqn_decl:
  ID EQUALS proc_expr
    {
      safe_assign($$, gsMakeProcEqn(
        gsMakeProcVarId($1, ATmakeList0()), ATmakeList0(), $3));
      gsDebugMsg("parsed process equation declaration\n  %T\n", $$);
    }
  | ID LPAR data_vars_decls_cs RPAR EQUALS proc_expr
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength($3);
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt($3, i), 1));
      }
      safe_assign($$, gsMakeProcEqn(
        gsMakeProcVarId($1, ATreverse(SortExprs)), $3, $6));
      gsDebugMsg("parsed process equation declaration\n  %T\n", $$);
    }
  ;

//process initialisation
proc_init:
  KWINIT proc_expr SEMICOLON
    {
      safe_assign($$, gsMakeProcessInit($2));
      gsDebugMsg("parsed initialisation\n  %T\n", $$);
    }
  ;

//State formulas
//--------------

//state formula
state_frm:
  state_frm_quant
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed state formula\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | NU ID fixpoint_params DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | MU ID fixpoint_params DOT state_frm_quant
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//parameters of a fixpoint variable declaration
fixpoint_params:
  /* empty */
    {
      safe_assign($$, ATmakeList0());
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", $$);
    }
  | LPAR data_var_decl_inits_cs RPAR
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed fixpoint parameters\n  %T\n", $$);
    }
  ;

//one or more declarations of a data variable with an
//initialisation, separated by comma's
data_var_decl_inits_cs:
  data_var_decl_init
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", $$);
    }
  | data_var_decl_inits_cs COMMA data_var_decl_init
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed data variable declaration and initialisations\n  %T\n", $$);
    }
  ;

//data variable declaration and initialisation
data_var_decl_init:
  ID COLON sort_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataVarIdInit(gsMakeDataVarId($1, $3), $5));
      gsDebugMsg("parsed data variable declaration and initialisation\n  %T\n", $$);
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
      gsDebugMsg("parsed implication\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | NU ID fixpoint_params DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | MU ID fixpoint_params DOT state_frm_imp_rhs
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | state_frm_prefix BARS state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateOr($1, $3));
      gsDebugMsg("parsed disjunction\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | NU ID fixpoint_params DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | MU ID fixpoint_params DOT state_frm_and_rhs
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed prefix state formula\n  %T\n", $$);
    }
  | LBRACK reg_frm RBRACK state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMust($2, $4));
      gsDebugMsg("parsed prefix state formula\n  %T\n", $$);
    }
  | LANG reg_frm RANG state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMay($2, $4));
      gsDebugMsg("parsed prefix state formula\n  %T\n", $$);
    }
  | YALED AT data_expr_prefix
    {
      safe_assign($$, gsMakeStateYaledTimed($3));
      gsDebugMsg("parsed prefix state formula\n  %T\n", $$);
    }
  | DELAY AT data_expr_prefix
    {
      safe_assign($$, gsMakeStateDelayTimed($3));
      gsDebugMsg("parsed prefix state formula\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | NU ID fixpoint_params DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateNu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | MU ID fixpoint_params DOT state_frm_quant_prefix
    {
      safe_assign($$, gsMakeStateMu($2, $3, $5));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//primary state formula
state_frm_primary:
  VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
    }
  | param_id
    {
      safe_assign($$, gsMakeStateVar(ATAgetArgument($1, 0), ATLgetArgument($1, 1)));
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
    }
  | CTRUE
    {
      safe_assign($$, gsMakeStateTrue());
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
    }
  | CFALSE
    {
      safe_assign($$, gsMakeStateFalse());
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
    }
  | YALED
    {
      safe_assign($$, gsMakeStateYaled());
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
    }
  | DELAY
    {
      safe_assign($$, gsMakeStateDelay());
      gsDebugMsg("parsed primary state formula\n  %T\n", $$);
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
      gsDebugMsg("parsed regular formula\n  %T\n", $$);
    }
  | reg_frm_alt_naf
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed regular formula\n  %T\n", $$);
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
      gsDebugMsg("parsed alternative composition\n  %T\n", $$);
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
      gsDebugMsg("parsed alternative composition\n  %T\n", $$);
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
      gsDebugMsg("parsed sequential composition\n  %T\n", $$);
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
      gsDebugMsg("parsed sequential composition\n  %T\n", $$);
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
      gsDebugMsg("parsed postfix regular formula\n  %T\n", $$);
    }
  | reg_frm_postfix PLUS
    {
      safe_assign($$, gsMakeRegTrans($1));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", $$);
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
      gsDebugMsg("parsed postfix regular formula\n  %T\n", $$);
    }
  | reg_frm_postfix PLUS
    {
      safe_assign($$, gsMakeRegTrans($1));
      gsDebugMsg("parsed postfix regular formula\n  %T\n", $$);
    }
  ;

//primary regular formula, no action formula at top level
reg_frm_primary_naf:
  NIL
    {
      safe_assign($$, gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", $$);
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
      gsDebugMsg("parsed regular formula\n  %T\n", $$);
    }
  | NIL
    {
      safe_assign($$, gsMakeRegNil());
      gsDebugMsg("parsed primary regular formula\n  %T\n", $$);
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
      gsDebugMsg("parsed action formula\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT act_frm_quant
    {
      safe_assign($$, gsMakeActExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed implication\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT act_frm_imp_rhs
    {
      safe_assign($$, gsMakeActExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | act_frm_prefix BARS act_frm_and_rhs
    {
      safe_assign($$, gsMakeActOr($1, $3));
      gsDebugMsg("parsed disjunction\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT act_frm_and_rhs
    {
      safe_assign($$, gsMakeActExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
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
      gsDebugMsg("parsed prefix action formula\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT act_frm_quant_prefix
    {
      safe_assign($$, gsMakeActExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//primary action formula
act_frm_primary:
  mult_act
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed primary action formula\n  %T\n", $$);
    }
  | VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
      gsDebugMsg("parsed primary action formula\n  %T\n", $$);
    }
  | CTRUE
    {
      safe_assign($$, gsMakeActTrue());
      gsDebugMsg("parsed primary action formula\n  %T\n", $$);
    }
  | CFALSE
    {
      safe_assign($$, gsMakeActFalse());
      gsDebugMsg("parsed primary action formula\n  %T\n", $$);
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
      gsDebugMsg("parsed action rename specification\n  %T\n", $$);
    }
  ;

//action rename specification elements
action_rename_spec_elts:
  action_rename_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed action rename specification element\n  %T\n", $$);
    }
   | action_rename_spec_elts action_rename_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed action rename specification element\n  %T\n", $$);
    }
   ;

//action rename specification element
action_rename_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed action rename specification element\n  %T\n", $$);
    }
  | act_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed action rename specification element\n  %T\n", $$);
    }
  | action_rename_rule_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed action rename specification element\n  %T\n", $$);
    }
  ;

//action rename rule_specification
action_rename_rule_spec:
  action_rename_rule_sect
    {
      safe_assign($$, gsMakeActionRenameRules($1));
      gsDebugMsg("parsed action rename specification\n  %T\n", $$);
    }
  ;

//var section before action rename rules
action_rename_rule_sect:
  RENAME action_rename_rules_scs
    {
      safe_assign($$, $2);
      gsDebugMsg("parsed action rename rule section\n  %T\n", $$);
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
      gsDebugMsg("parsed action rename rule section\n  %T\n", $$);
    }


//action rename rules comma separated
action_rename_rules_scs:
  action_rename_rule SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed action rename rules\n  %T\n", $$);
    }
  | action_rename_rules_scs action_rename_rule SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed action rename rules\n  %T\n", $$);
    }
  ;

//action rename rule
action_rename_rule:
  data_expr ARROW param_id IMP action_rename_rule_rhs
    {
      safe_assign($$, gsMakeActionRenameRule(ATmakeList0(), $1, $3, $5));
      gsDebugMsg("parsed action rename rule\n %T\n", $$);
    }
  | param_id IMP action_rename_rule_rhs
    {
      safe_assign($$, gsMakeActionRenameRule(ATmakeList0(), mcrl2::data::sort_bool::true_(), $1, $3));
      gsDebugMsg("parsed action rename rule\n %T\n", $$);
    }
  ;

//right-hand side of an action rename rule
action_rename_rule_rhs:
  param_id
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed right-hand-side of an action rename rule\n %T\n", $$);
    }
  | proc_constant
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed right-hand-side of an action rename rule\n %T\n", $$);
    }
  ;

//Parameterised boolean expressions
//---------------------------------

//parameterised boolean expression
pb_expr:
  pb_expr_quant
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed parameterised boolean expression\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_quant
    {
      safe_assign($$, gsMakePBESExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed implication\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_imp_rhs
    {
      safe_assign($$, gsMakePBESExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | pb_expr_not BARS pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESOr($1, $3));
      gsDebugMsg("parsed disjunction\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_and_rhs
    {
      safe_assign($$, gsMakePBESExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      gsDebugMsg("parsed negation\n  %T\n", $$);
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
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT pb_expr_quant_not
    {
      safe_assign($$, gsMakePBESExists($2, $4));
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//primary expression
pb_expr_primary:
  VAL LPAR data_expr RPAR
    {
      safe_assign($$, $3);
      gsDebugMsg("parsed primary expression\n  %T\n", $$);
    }
  | param_id
    {
      safe_assign($$, gsMakePropVarInst(ATAgetArgument($1, 0), ATLgetArgument($1, 1)));
      gsDebugMsg("parsed primary expression\n  %T\n", $$);
    }
  | CTRUE
    {
      safe_assign($$, gsMakePBESTrue());
      gsDebugMsg("parsed primary expression\n  %T\n", $$);
    }
  | CFALSE
    {
      safe_assign($$, gsMakePBESFalse());
      gsDebugMsg("parsed primary expression\n  %T\n", $$);
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
      gsDebugMsg("parsed PBES specification\n  %T\n", $$);
    }
  ;

//PBES specification elements
pbes_spec_elts:
  pbes_spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed PBES specification elements\n  %T\n", $$);
    }
   | pbes_spec_elts pbes_spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed PBES specification elements\n  %T\n", $$);
    }
   ;

//PBES specification element
pbes_spec_elt:
  data_spec_elt
    {
      safe_assign($$, $1);
     gsDebugMsg("parsed PBES specification element\n  %T\n", $$);
    }
  | glob_var_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed PBES specification element\n  %T\n", $$);
    }
  | pb_eqn_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed PBES specification element\n  %T\n", $$);
    }
  | pb_init
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed PBES specification element\n  %T\n", $$);
    }
  ;

//parameterised boolean equation specification
pb_eqn_spec:
  KWPBES pb_eqn_decls_scs
    {
      safe_assign($$, gsMakePBEqnSpec(ATreverse($2)));
      gsDebugMsg("parsed parameterised boolean equation specification\n  %T\n", $$);
    }
  ;

//one or more parameterised boolean equation declarations, separated by semicolons
pb_eqn_decls_scs:
  pb_eqn_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed parameterised boolean equation declarations\n  %T\n", $$);
    }
  | pb_eqn_decls_scs pb_eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed parameterised boolean equation declarations\n  %T\n", $$);
    }
  ;

//parameterised boolean equation declaration
pb_eqn_decl:
  fixpoint ID EQUALS pb_expr
    {
      safe_assign($$,
        gsMakePBEqn($1, gsMakePropVarDecl($2, ATmakeList0()), $4));
      gsDebugMsg("parsed parameterised boolean equation declaration\n  %T\n", $$);
    }
  | fixpoint ID LPAR data_vars_decls_cs RPAR EQUALS pb_expr
    {
      safe_assign($$,
        gsMakePBEqn($1, gsMakePropVarDecl($2, $4), $7));
      gsDebugMsg("parsed parameterised boolean equation declaration\n  %T\n", $$);
    }
  ;

//fixpoint
fixpoint:
  MU
    {
      safe_assign($$, gsMakeMu());
      gsDebugMsg("parsed fixpoint\n  %T\n", $$);
    }
  | NU
    {
      safe_assign($$, gsMakeNu());
      gsDebugMsg("parsed fixpoint\n  %T\n", $$);
    }
  ;

//parameterised boolean initialisation
pb_init:
  KWINIT param_id SEMICOLON
    {
      safe_assign($$,
        gsMakePBInit(gsMakePropVarInst(ATAgetArgument($2, 0), ATLgetArgument($2, 1))));
      gsDebugMsg("parsed initialisation\n  %T\n", $$);
    }
  ;

%%

//Uncomment the lines below to enable the use of SIZE_MAX
//#define __STDC_LIMIT_MACROS 1
//#include <stdint.h>

//Uncomment the line below to enable the use of std::cerr, std::cout and std::endl;
//#include <iostream>

ATermAppl gsDataSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
    if (gsIsSortSpec(SpecElt)) {
      SortDecls = ATconcat(SortDecls, SpecEltArg0);
    } else if (gsIsConsSpec(SpecElt)) {
      ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
    } else if (gsIsMapSpec(SpecElt)) {
      MapDecls = ATconcat(MapDecls, SpecEltArg0);
    } else if (gsIsDataEqnSpec(SpecElt)) {
      DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
    }
  }
  Result = gsMakeDataSpec(
    gsMakeSortSpec(SortDecls),
    gsMakeConsSpec(ConsDecls),
    gsMakeMapSpec(MapDecls),
    gsMakeDataEqnSpec(DataEqnDecls)
  );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //std::cerr << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //std::cerr << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //std::cerr << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //std::cerr << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

ATermAppl gsProcSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList GlobVars = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ProcEqnDecls = ATmakeList0();
  ATermAppl ProcInit = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsProcessInit(SpecElt)) {
      if (ProcInit == NULL) {
        ProcInit = SpecElt;
      } else {
        //ProcInit != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt)) {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      } else if (gsIsSortSpec(SpecElt)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(SpecElt)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(SpecElt)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsDataEqnSpec(SpecElt)) {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      } else if (gsIsActSpec(SpecElt)) {
        ActDecls = ATconcat(ActDecls, SpecEltArg0);
      } else if (gsIsProcEqnSpec(SpecElt)) {
        ProcEqnDecls = ATconcat(ProcEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether an initialisation is present
  if (ProcInit == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakeProcSpec(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeGlobVarSpec(GlobVars),
    gsMakeProcEqnSpec(ProcEqnDecls),
    ProcInit
  );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //std::cerr << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //std::cerr << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //std::cerr << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //std::cerr << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

ATermAppl gsPBESSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList GlobVars = ATmakeList0();
  ATermAppl PBEqnSpec = NULL;
  ATermAppl PBInit = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsPBEqnSpec(SpecElt)) {
      if (PBEqnSpec == NULL) {
        PBEqnSpec = SpecElt;
      } else {
        //PBEqnSpec != NULL
        gsErrorMsg("parse error: multiple parameterised boolean equation specifications\n");
        return NULL;
      }
    } else if (gsIsPBInit(SpecElt)) {
      if (PBInit == NULL) {
        PBInit = SpecElt;
      } else {
        //PBInit != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt)) {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      } else if (gsIsSortSpec(SpecElt)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(SpecElt)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(SpecElt)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsDataEqnSpec(SpecElt)) {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether a parameterised boolean equation specification is present
  if (PBEqnSpec == NULL) {
    gsErrorMsg("parse error: missing parameterised boolean equation specification\n");
    return NULL;
  }
  //check whether an initialisation is present
  if (PBInit == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakePBES(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeGlobVarSpec(GlobVars),
    PBEqnSpec,
    PBInit
  );
  return Result;
}

ATermAppl gsActionRenameEltsToActionRename(ATermList ActionRenameElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ActionRenameRules = ATmakeList0();
  int n = ATgetLength(ActionRenameElts);
  for (int i = 0; i < n; i++) {
    ATermAppl ActionRenameElt = ATAelementAt(ActionRenameElts, i);
    ATermList ActionRenameEltArg0 = ATLgetArgument(ActionRenameElt, 0);
    if (gsIsSortSpec(ActionRenameElt)) {
      SortDecls = ATconcat(SortDecls, ActionRenameEltArg0);
    } else if (gsIsConsSpec(ActionRenameElt)) {
      ConsDecls = ATconcat(ConsDecls, ActionRenameEltArg0);
    } else if (gsIsMapSpec(ActionRenameElt)) {
      MapDecls = ATconcat(MapDecls, ActionRenameEltArg0);
    } else if (gsIsDataEqnSpec(ActionRenameElt)) {
      DataEqnDecls = ATconcat(DataEqnDecls, ActionRenameEltArg0);
    } else if (gsIsActSpec(ActionRenameElt)) {
      ActDecls = ATconcat(ActDecls, ActionRenameEltArg0);
    } else if (gsIsActionRenameRules(ActionRenameElt)) {
      ActionRenameRules = ATconcat(ActionRenameRules, ActionRenameEltArg0);
    } else {
      assert(false);
    }
  }

  Result = gsMakeActionRenameSpec(
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeActionRenameRules(ActionRenameRules)
  );
  return Result;
}
