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
#include <stdbool.h>

#include "aterm2.h"
#include "gsfunc.h"

//Global precondition: the ATerm library has been initialised

//external declarations
extern ATermAppl gsTree;        /* declared in gslexer.l */
extern void gsyyerror(char *s); /* declared in gslexer.l */
extern int gsyylex(void);       /* declared in gslexer.c */
extern bool gsDebug;            /* declared in libgsparse.c */

//local declarations
ATermAppl gsSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.
%}

%union {
  ATermAppl appl;
  ATermList list;
}

//generate a GLR parser
%glr-parser

//start token
%start spec

//terminals
//---------

%token LMERGE ARROW LTE GTE CONS SNOC CONCAT EQ NEQ AND BARS IMP BINIT
%token STAR PLUS MINUS EQUALS DOT COMMA COLON SEMICOLON QMARK EXCLAM AT HASH BAR
%token LPAR RPAR PBRACK LBRACK RBRACK LANG RANG PBRACE LBRACE RBRACE
%token SORT CONS MAP VAR EQN ACT PROC INIT
%token BOOL POS NAT INT LIST SET BAG STRUCT
%token TRUE FALSE WHR END LAMBDA FORALL EXISTS DIV MOD IN
%token DELTA TAU SUM RESTRICT ALLOW HIDE RENAME COMM
%token <appl> ID NUMBER

//non-terminals
//-------------

%type <appl> spec spec_elt sort_spec sort_decl constr_decl proj_decl recog_decl
%type <appl> cons_spec map_spec op_decl ids_decl id_decl eqn_spec eqn_sect
%type <appl> eqn_var_decl eqn_decl act_spec act_decl proc_spec proc_decl
%type <appl> proc_var_decl initialisation domain_no_arrow_elt
%type <appl> sort_expr sort_expr_arrow sort_expr_primary sort_constant
%type <appl> sort_constructor data_expr data_expr_whr whr_decl data_expr_lambda
%type <appl> data_expr_imp data_expr_and data_expr_eq data_expr_rel
%type <appl> data_expr_cons data_expr_snoc data_expr_concat data_expr_add
%type <appl> data_expr_mult data_expr_quant data_expr_prefix data_expr_postfix
%type <appl> data_expr_primary data_constant data_enumeration bag_enum_elt
%type <appl> data_comprehension proc_expr proc_expr_choice proc_expr_merge
%type <appl> proc_expr_binit proc_expr_cond proc_expr_seq proc_expr_at
%type <appl> proc_expr_sync proc_expr_primary proc_constant proc_ref proc_quant
%type <appl> ma_id ren_expr comm_expr

%type <list> spec_elts sort_decls ids constr_decls proj_decls domain op_decls
%type <list> eqn_var_decls eqn_decls act_decls proc_decls proc_var_decls
%type <list> domain_no_arrow domain_no_arrow_elts whr_decls data_exprs
%type <list> bag_enum_elts ma_ids_set ma_ids ma_id_elts ren_expr_set ren_exprs
%type <list> comm_expr_set comm_exprs
%%

//specification
spec:
  spec_elts
    {
      $$ = gsSpecEltsToSpec(ATreverse($1));
      if (gsDebug) {
        ATprintf("parsed specification\n  %t\n", $$);
      }
      gsTree = $$;
    }
  ;

//specification elements
spec_elts:
  spec_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed specification elements\n  %t\n", $$);
      }
    }
   | spec_elts spec_elt
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed specification elements\n  %t\n", $$);
      }
    }
   ;

//specification element
spec_elt:
  sort_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    } 
  | cons_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  | map_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  | eqn_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  | act_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  | proc_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  | initialisation
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed specification element\n  %t\n", $$);
      }
    }
  ;

//sort specification
sort_spec:
  SORT sort_decls
    {
      $$ = gsMakeSortSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed sort specification\n  %t\n", $$);
      }
    }
  ;

//sort declarations
sort_decls:
  sort_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed sort declarations\n  %t\n", $$);
      }
    }
  | sort_decls sort_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed sort declarations\n  %t\n", $$);
      }
    }
  ;

//sort declaration
sort_decl:
  ids SEMICOLON
    {
      $$ = gsMakeSortDeclStandard(ATreverse($1));
      if (gsDebug) {
        ATprintf("parsed standard sort declaration\n  %t\n", $$);
      }
    }
  | ID EQUALS sort_expr SEMICOLON
    {
      $$ = gsMakeSortDeclRef($1, $3);
      if (gsDebug) {
        ATprintf("parsed reference sort declaration\n  %t\n", $$);
      }
    }
  | ID EQUALS STRUCT constr_decls SEMICOLON
    {
      $$ = gsMakeSortDeclStruct($1, ATreverse($4));
      if (gsDebug) {
        ATprintf("parsed structured sort declaration\n  %t\n", $$);
      }
    }
  ;

//comma-separated list of identifiers
ids:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
      }
    }
  | ids COMMA ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
      }
    }
  ;

//declaration of a structured sort
constr_decls:
  constr_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed constructor declarations\n  %t\n", $$);
      }
    }
  | constr_decls BAR constr_decl
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed constructor declarations\n  %t\n", $$);
      }
    }
  ;

//constructor declaration
constr_decl:
  ID recog_decl
    {
      $$ = gsMakeStructDeclCons($1, ATmakeList0(), $2);
      if (gsDebug) {
        ATprintf("parsed constructor declaration\n  %t\n", $$);
      }
    }
  | ID LPAR proj_decls RPAR recog_decl
    {
      $$ = gsMakeStructDeclCons($1, ATreverse($3), $5);
      if (gsDebug) {
        ATprintf("parsed constructor declaration\n  %t\n", $$);
      }
    }
  ;

recog_decl:
  /* empty */ 
    {
      $$ = gsMakeNil();
      if (gsDebug) {
        ATprintf("parsed recogniser\n  %t\n", $$);
      }
    }
  | QMARK ID
    {
      $$ = $2;
      if (gsDebug) {
        ATprintf("parsed recogniser id\n  %t\n", $$);
      }
    }
  ;

//comma-separated list of projection declarations
proj_decls:
  proj_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed projection declarations\n  %t\n", $$);
      }
    }
  | proj_decls COMMA proj_decl
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed projection declarations\n  %t\n", $$);
      }
    }
  ;

//projection declaration
proj_decl:
  domain
    {
      $$ = gsMakeStructDeclProj(gsMakeNil(), $1);
      if (gsDebug) {
        ATprintf("parsed projection declaration\n  %t\n", $$);
      }
    }
  | ID COLON domain
    {
      $$ = gsMakeStructDeclProj($1, $3);
      if (gsDebug) {
        ATprintf("parsed projection declaration\n  %t\n", $$);
      }
    }
  ;

//domain
domain:
  domain_no_arrow
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed domain\n  %t\n", $$);
      }
    }
  | domain_no_arrow ARROW sort_expr
    {
      $$ = ATmakeList1((ATerm) gsMakeSortArrow($1, $3));
      if (gsDebug) {
        ATprintf("parsed domain\n  %t\n", $$);
      }
    }
  ;

//constructor operation specification
cons_spec:
  CONS op_decls
    {
      $$ = gsMakeConsSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed constructor operation specification\n  %t\n", $$);
      }
    }
  ;

//operation specification
map_spec:
  MAP op_decls
    {
      $$ = gsMakeMapSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed operation specification\n  %t\n", $$);
      }
    }
  ;

//operation declarations
op_decls:
  op_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed operation declarations\n  %t\n", $$);
      }
    }
  | op_decls op_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed operation declarations\n  %t\n", $$);
      }
    }
  ;

//operation declaration
op_decl:
  ids_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed operation declaration\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more identifiers of the same sort
ids_decl:
  ids COLON sort_expr
    {
      $$ = gsMakeIdsDecl(ATreverse($1), $3);
      if (gsDebug) {
        ATprintf("parsed identifiers declaration\n  %t\n", $$);
      }
    }
  ;

//equation specification
eqn_spec:
  eqn_sect    
    {
      $$ = gsMakeEqnSpec(ATmakeList1((ATerm) $1));
      if (gsDebug) {
        ATprintf("parsed equation specification\n  %t\n", $$);
      }
    }
  ;

//equation section
eqn_sect:
  EQN eqn_decls
    {
      $$ = gsMakeEqnSect(ATmakeList0(), ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed equation section\n  %t\n", $$);
      }
    }
  | VAR eqn_var_decls EQN eqn_decls
    {
      $$ = gsMakeEqnSect(ATreverse($2), ATreverse($4));
      if (gsDebug) {
        ATprintf("parsed equation section\n  %t\n", $$);
      }
    }
  ;

//variable declarations of an equation specification
eqn_var_decls:
  eqn_var_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed equation variable declarations\n  %t\n", $$);
      }
    }
  | eqn_var_decls eqn_var_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed equation variable declarations\n  %t\n", $$);
      }
    }
  ;

//variable declaration of an equation specification
eqn_var_decl:
  ids_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed equation variable declaration\n  %t\n", $$);
      }
    }
  ;

//equation declarations
eqn_decls:
  eqn_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed equation declarations\n  %t\n", $$);
      }
    }
  | eqn_decls eqn_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed equation declarations\n  %t\n", $$);
      }
    }
  ;

//equation declaration
eqn_decl:
  data_expr EQUALS data_expr SEMICOLON
    {
      $$ = gsMakeEqnDecl($1, $3);
      if (gsDebug) {
        ATprintf("parsed equation declaration\n  %t\n", $$);
      }
    }
  ;

//action specification
act_spec:
  ACT act_decls
    {
      $$ = gsMakeActSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed action specification\n  %t\n", $$);
      }
    }
  ;

//action declarations
act_decls:
  act_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  | act_decls act_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  ;

//action declaration
act_decl:
  ids SEMICOLON
    {
      $$ = gsMakeActDecl(ATreverse($1), ATmakeList0());
      if (gsDebug) {
        ATprintf("parsed action declaration\n  %t\n", $$);
      }
    }
  | ids COLON domain SEMICOLON
    {
      $$ = gsMakeActDecl(ATreverse($1), $3);
      if (gsDebug) {
        ATprintf("parsed action declaration\n  %t\n", $$);
      }
    }
  ;

//process specification
proc_spec:
  PROC proc_decls
    {
      $$ = gsMakeProcSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed process specification\n  %t\n", $$);
      }
    }
  ;

//process declarations
proc_decls:
  proc_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed process declarations\n  %t\n", $$);
      }
    }
  | proc_decls proc_decl
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed process declarations\n  %t\n", $$);
      }
    }
  ;

//process declaration
proc_decl:
  ID EQUALS proc_expr SEMICOLON
    {
      $$ = gsMakeProcDecl($1, ATmakeList0(), $3);
      if (gsDebug) {
        ATprintf("parsed process declaration\n  %t\n", $$);
      }
    }
  | ID LPAR proc_var_decls RPAR EQUALS proc_expr SEMICOLON
    {
      $$ = gsMakeProcDecl($1, ATreverse($3), $6);
      if (gsDebug) {
        ATprintf("parsed process declaration\n  %t\n", $$);
      }
    }
  ;

//variable declarations of a process declaration
proc_var_decls:
  proc_var_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed process variable declarations\n  %t\n", $$);
      }
    }
  | proc_var_decls COMMA proc_var_decl
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed process variable declarations\n  %t\n", $$);
      }
    }
  ;

//variable declaration of a process declaration
proc_var_decl:
  ids_decl
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed process variable declaration\n  %t\n", $$);
      }
    }
  ;

//initialisation
initialisation:
  INIT proc_expr SEMICOLON
    {
      $$ = gsMakeInit($2);
      if (gsDebug) {
        ATprintf("parsed initialisation\n  %t\n", $$);
      }
    }
  ;

//sort expression
sort_expr:
  sort_expr_arrow
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed sort expression\n  %t\n", $$);
      }
    }
  ;

//arrow sort expression
sort_expr_arrow:
  sort_expr_primary
    {
      $$ = $1;
    }
  | domain_no_arrow ARROW sort_expr_arrow
    {
      $$ = gsMakeSortArrow($1, $3);
      if (gsDebug) {
        ATprintf("parsed arrow sort\n  %t\n", $$);
      }
    }
  ;

//domain elements
domain_no_arrow:
  domain_no_arrow_elts
    {
      $$ = ATreverse($1);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain\n  %t\n", $$);
      }
    }
  ;

domain_no_arrow_elts:
  domain_no_arrow_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain elements\n  %t\n", $$);
      }
    }
  | domain_no_arrow_elts HASH domain_no_arrow_elt
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain elements\n  %t\n", $$);
      }
    }
  ;

domain_no_arrow_elt:
  sort_expr_primary
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed non-arrow domain element\n  %t\n", $$);
      }
    }
  ;

//primary sort expression
sort_expr_primary:
  ID
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed primary sort\n  %t\n", $$);
      }
    }
  | sort_constant
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed sort constant\n  %t\n", $$);
      }
    }
  | sort_constructor
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed sort constructor\n  %t\n", $$);
      }
    }
  | LPAR sort_expr RPAR
    {
      $$ = $2;
    }
  ;

//sort constant
sort_constant:
  BOOL
    {
      $$ = gsMakeSortBool();
    }
  | POS
    {
      $$ = gsMakeSortPos();
    }
  | NAT
    {
      $$ = gsMakeSortNat();
    }
  | INT
    {
      $$ = gsMakeSortInt();
    }
  ;

//sort constructor
sort_constructor:
  LIST LPAR sort_expr RPAR
    {
      $$ = gsMakeSortList($3);
    }
  | SET LPAR sort_expr RPAR
    {
      $$ = gsMakeSortSet($3);
    }
  | BAG LPAR sort_expr RPAR
    {
      $$ = gsMakeSortBag($3);
    }
  ;

//data expression
data_expr:
  data_expr_whr
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed data expression\n  %t\n", $$);
      }
    }
  ;

//where clause
data_expr_whr:
  data_expr_lambda
    {
      $$ = $1;
    }
  | data_expr_whr WHR whr_decls END
    {
      $$ = gsMakeWhr($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed where clause\n  %t\n", $$);
      }
    }
  ;

//where clause declarations
whr_decls:
  whr_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed where clause declarations\n  %t\n", $$);
      }
    }
  | whr_decls COMMA whr_decl
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed where clause declarations\n  %t\n", $$);
      }
    }
  ;

//where clause declaration
whr_decl:
  ID EQ data_expr
    {
      $$ = gsMakeWhrDecl($1, $3);
      if (gsDebug) {
        ATprintf("parsed where clause declaration\n  %t\n", $$);
      }
    }
  ;

//lambda abstraction
data_expr_lambda:
  data_expr_imp
    {
      $$ = $1;
    }
  | LAMBDA id_decl DOT data_expr_lambda
    {
      $$ = gsMakeLambda($2, $4);
      if (gsDebug) {
        ATprintf("parsed lambda abstraction\n  %t\n", $$);
      }
    }
  ;

//identifier declaration
id_decl:
  ID COLON sort_expr
    {
      $$ = gsMakeIdDecl($1, $3);
      if (gsDebug) {
        ATprintf("parsed identifier declaration\n  %t\n", $$);
      }
    }
  ;

//implication
data_expr_imp:
  data_expr_and
    {
      $$ = $1;
    }
  | data_expr_and IMP data_expr_imp
    {
      $$ = gsMakeImp($1, $3);
      if (gsDebug) {
        ATprintf("parsed implication\n  %t\n", $$);
      }
    }
  ;

//conjunction and disjunction
data_expr_and:
  data_expr_eq
    {
      $$ = $1;
    }
  | data_expr_and AND data_expr_eq
    {
      $$ = gsMakeAnd($1, $3);
      if (gsDebug) {
        ATprintf("parsed conjunction\n  %t\n", $$);
      }
    }
  | data_expr_and BARS data_expr_eq
    {
      $$ = gsMakeOr($1, $3);
      if (gsDebug) {
        ATprintf("parsed disjunction\n  %t\n", $$);
      }
    }
  ;

//equality
data_expr_eq:
  data_expr_rel
    {
      $$ = $1;
    }
  | data_expr_eq EQ data_expr_rel
    {
      $$ = gsMakeEq($1, $3);
      if (gsDebug) {
        ATprintf("parsed equality expression\n  %t\n", $$);
      }
    }
  | data_expr_eq NEQ data_expr_rel
    {
      $$ = gsMakeNeq($1, $3);
      if (gsDebug) {
        ATprintf("parsed equality expression\n  %t\n", $$);
      }
    }
  ;

//relations
data_expr_rel:
  data_expr_cons
    {
      $$ = $1;
    }
  | data_expr_cons GTE data_expr_cons
    {
      $$ = gsMakeGTE($1, $3);
      if (gsDebug) {
        ATprintf("parsed relational expression\n  %t\n", $$);
      }
    }
  | data_expr_cons LTE data_expr_cons
    {
      $$ = gsMakeLTEOrSubset($1, $3);
      if (gsDebug) {
        ATprintf("parsed relational expression\n  %t\n", $$);
      }
    }
  | data_expr_cons RANG data_expr_cons
    {
      $$ = gsMakeGT($1, $3);
      if (gsDebug) {
        ATprintf("parsed relational expression\n  %t\n", $$);
      }
    }
  | data_expr_cons LANG data_expr_cons
    {
      $$ = gsMakeLTOrPropSubset($1, $3);
      if (gsDebug) {
        ATprintf("parsed relational expression\n  %t\n", $$);
      }
    }
  | data_expr_cons IN data_expr_cons
    {
      $$ = gsMakeIn($1, $3);
      if (gsDebug) {
        ATprintf("parsed relational expression\n  %t\n", $$);
      }
    }
  ;

//cons
data_expr_cons:
  data_expr_snoc
    {
      $$ = $1;
    }
  | data_expr_add CONS data_expr_cons
    {
      $$ = gsMakeCons($1, $3);
      if (gsDebug) {
        ATprintf("parsed list cons expression\n  %t\n", $$);
      }
    }
  ;

//snoc
data_expr_snoc:
  data_expr_concat
    {
      $$ = $1;
    }
  | data_expr_snoc SNOC data_expr_add
    {
      $$ = gsMakeSnoc($1, $3);
      if (gsDebug) {
        ATprintf("parsed list snoc expression\n  %t\n", $$);
      }
    }
  ; 

//concatenation
data_expr_concat:
  data_expr_add
    {
      $$ = $1;
    }
  | data_expr_concat CONCAT data_expr_add
    {
      $$ = gsMakeConcat($1, $3);
      if (gsDebug) {
        ATprintf("parsed list concat expression\n  %t\n", $$);
      }
    }
  ;

//addition and subtraction
data_expr_add:
  data_expr_mult
    {
      $$ = $1;
    }
  | data_expr_add PLUS data_expr_mult
    {
      $$ = gsMakeAddOrUnion($1, $3);
      if (gsDebug) {
        ATprintf("parsed addition or set union\n  %t\n", $$);
      }
    }
  | data_expr_add MINUS data_expr_mult
    {
      $$ = gsMakeSubtOrDiff($1, $3);
      if (gsDebug) {
        ATprintf("parsed subtraction or set difference\n  %t\n", $$);
      }
    }
  ;

//multiplication and division
data_expr_mult:
  data_expr_quant
    {
      $$ = $1;
    }
  | data_expr_mult STAR data_expr_quant
    {
      $$ = gsMakeMultOrIntersect($1, $3);
      if (gsDebug) {
        ATprintf("parsed multiplication or set intersection\n  %t\n", $$);
      }
    }
  | data_expr_mult DIV data_expr_quant
    {
      $$ = gsMakeDiv($1, $3);
      if (gsDebug) {
        ATprintf("parsed div expression\n  %t\n", $$);
      }
    }
  | data_expr_mult MOD data_expr_quant
    {
      $$ = gsMakeMod($1, $3);
      if (gsDebug) {
        ATprintf("parsed mod expression\n  %t\n", $$);
      }
    }
  | data_expr_mult DOT data_expr_quant
    {
      $$ = gsMakeListAt($1, $3);
      if (gsDebug) {
        ATprintf("parsed list at expression\n  %t\n", $$);
      }
    }
  ;

//universal and existential quantification
data_expr_quant:
  data_expr_prefix
    {
      $$ = $1;
    }
  | FORALL id_decl DOT data_expr_quant
    {
      $$ = gsMakeForall($2, $4);
      if (gsDebug) {
        ATprintf("parsed quantification\n  %t\n", $$);
      }
    }
  | EXISTS id_decl DOT data_expr_quant
    {
      $$ = gsMakeExists($2, $4);
      if (gsDebug) {
        ATprintf("parsed quantification\n  %t\n", $$);
      }
    }
  ;

//prefix data expression
data_expr_prefix:
  data_expr_postfix
    {
      $$ = $1;
    }
  | EXCLAM data_expr_prefix
    {
      $$ = gsMakeNotOrCompl($2);
      if (gsDebug) {
        ATprintf("parsed prefix data expression\n  %t\n", $$);
      }
    }
  | MINUS data_expr_prefix                 //arithmetic negation
    {
      $$ = gsMakeNeg($2);
      if (gsDebug) {
        ATprintf("parsed prefix data expression\n  %t\n", $$);
      }
    }
  | HASH data_expr_prefix                  //list/set/bag size
    {
      $$ = gsMakeSize($2);
      if (gsDebug) {
        ATprintf("parsed prefix data expression\n  %t\n", $$);
      }
    }
  ;  

//postfix data expression
data_expr_postfix:
  data_expr_primary
    {
      $$ = $1;
    }
  | data_expr_postfix LPAR data_exprs RPAR
    {
      $$ = gsMakeFuncApp($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed postfix data expression\n  %t\n", $$);
      }
    }
  ;

//comma-separated list of data expressions
data_exprs:
  data_expr
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed data expressions\n  %t\n", $$);
      }
    }
  | data_exprs COMMA data_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed data expressions\n  %t\n", $$);
      }
    }
  ;
  
//primary data expression
data_expr_primary:
  ID
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed primary data expression\n  %t\n", $$);
      }
    }
  | data_constant
    {
      $$ = $1;
    }
  | data_enumeration
    {
      $$ = $1;
    }
  | data_comprehension
    {
      $$ = $1;
    }
  | LPAR data_expr RPAR
    {
      $$ = $2;
    }
  ;

//constant
data_constant:
  TRUE
    {
      $$ = gsMakeTrue();
      if (gsDebug) {
        ATprintf("parsed data constant\n  %t\n", $$);
      }
    }
  | FALSE
    {
      $$ = gsMakeFalse();
      if (gsDebug) {
        ATprintf("parsed data constant\n  %t\n", $$);
      }
    }
  | NUMBER
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed data constant\n  %t\n", $$);
      }
    }
  | PBRACK
    {
      $$ = gsMakeEmptyList();
      if (gsDebug) {
        ATprintf("parsed data constant\n  %t\n", $$);
      }
    }
  | PBRACE
    {
      $$ = gsMakeEmptySetBag();
      if (gsDebug) {
        ATprintf("parsed data constant\n  %t\n", $$);
      }
    }
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs RBRACK
    {
      $$ = gsMakeListEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  | LBRACE data_exprs RBRACE
    {
      $$ = gsMakeSetEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  | LBRACE bag_enum_elts RBRACE
    {
      $$ = gsMakeBagEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  ;

//comma-separated list of bag enumeration elements
bag_enum_elts:
  bag_enum_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed bag enumeration elements\n  %t\n", $$);
      }
    }
  | bag_enum_elts COMMA bag_enum_elt
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed bag enumeration elements\n  %t\n", $$);
      }
    }
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
    {
      $$ = gsMakeBagEnumElt($1, $3);
      if (gsDebug) {
        ATprintf("parsed bag enumeration element\n  %t\n", $$);
      }
    }
  ;

//comprehension
data_comprehension:
  LBRACE id_decl BAR data_expr RBRACE
    {
      $$ = gsMakeSetBagComp($2, $4);
      if (gsDebug) {
        ATprintf("parsed data comprehension\n  %t\n", $$);
      }
    }
  ;

//process expression
proc_expr:
  proc_expr_choice
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed process expression\n  %t\n", $$);
      }
    }
  ;

//choice (associative)
proc_expr_choice:
  proc_expr_merge
    {
      $$ = $1;
    }
  | proc_expr_choice PLUS proc_expr_merge
    {
      $$ = gsMakeChoice($1, $3);
      if (gsDebug) {
        ATprintf("parsed choice expression\n  %t\n", $$);
      }
    }
  ;

//merge (associative) and left merge (left associative)
proc_expr_merge:
  proc_expr_binit
    {
      $$ = $1;
    }
  | proc_expr_merge BARS proc_expr_binit
    {
      $$ = gsMakeMerge($1, $3);
      if (gsDebug) {
        ATprintf("parsed merge expression\n  %t\n", $$);
      }
    }
  | proc_expr_merge LMERGE proc_expr_binit
    {
      $$ = gsMakeLMerge($1, $3);
      if (gsDebug) {
        ATprintf("parsed left merge expression\n  %t\n", $$);
      }
    }
  ;

//bounded initialisation (left associative)
proc_expr_binit:
  proc_expr_cond
    {
      $$ = $1;
    }
  | proc_expr_binit BINIT proc_expr_cond
    {
      $$ = gsMakeBInit($1, $3);
      if (gsDebug) {
        ATprintf("parsed bounded initialisation expression\n  %t\n", $$);
      }
    }
  ;

//conditional
proc_expr_cond:
  proc_expr_seq
    {
      $$ = $1;
    }
  | data_expr_prefix ARROW proc_expr_seq
    {
      $$ = gsMakeCond($1, $3);
      if (gsDebug) {
        ATprintf("parsed conditional expression\n  %t\n", $$);
      }
    }
  ;

//sequential (associative)
proc_expr_seq:
  proc_expr_at
    {
      $$ = $1;
    }
  | proc_expr_seq DOT proc_expr_at
    {
      $$ = gsMakeSeq($1, $3);
      if (gsDebug) {
        ATprintf("parsed sequential expression\n  %t\n", $$);
      }
    }
  ;

//timed expression
proc_expr_at:
  proc_expr_sync
    {
      $$ = $1;
    }
  | proc_expr_at AT data_expr_prefix
    {
      $$ = gsMakeAtTime($1, $3);
      if (gsDebug) {
        ATprintf("parsed at time expression\n  %t\n", $$);
      }
    }
  ;

//synchronisation (associative)
proc_expr_sync:
  proc_expr_primary
    {
      $$ = $1;
    }
  | proc_expr_sync BAR proc_expr_primary
    {
      $$ = gsMakeSync($1, $3);
      if (gsDebug) {
        ATprintf("parsed sync expression\n  %t\n", $$);
      }
    }
  ;

//primary process expression
proc_expr_primary:
  proc_constant
    {
      $$ = $1;
    }
  | proc_ref
    {
      $$ = $1;
    }
  | proc_quant
    {
      $$ = $1;
    }
  | LPAR proc_expr RPAR
    {
      $$ = $2;
    }
  ;

//process constant
proc_constant:
  DELTA
    {
      $$ = gsMakeDelta();
      if (gsDebug) {
        ATprintf("parsed process constant\n  %t\n", $$);
      }
    }
  | TAU
    {
      $$ = gsMakeTau();
      if (gsDebug) {
        ATprintf("parsed process constant\n  %t\n", $$);
      }
    }
  ;

//action or process reference
proc_ref:
  ID
    {
      $$ = gsMakeActProcRef($1, ATmakeList0());
      if (gsDebug) {
        ATprintf("parsed action or process reference\n  %t\n", $$);
      }
    }
  | ID LPAR data_exprs RPAR
    {
      $$ = gsMakeActProcRef($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed action or process reference\n  %t\n", $$);
      }
    }
  ;

//process quantification
proc_quant:
  SUM LPAR id_decl COMMA proc_expr RPAR
    {
      $$ = gsMakeSum($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | RESTRICT LPAR ma_ids_set COMMA proc_expr RPAR
    {
      $$ = gsMakeRestrict($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | ALLOW LPAR ma_ids_set COMMA proc_expr RPAR
    {
      $$ = gsMakeAllow($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | HIDE LPAR ma_ids_set COMMA proc_expr RPAR
    {
      $$ = gsMakeHide($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR
    {
      $$ = gsMakeRename($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR
    {
      $$ = gsMakeComm($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  ;

//set of multiaction identifiers
ma_ids_set:
  PBRACE
    {
      $$ = ATmakeList0();
      if (gsDebug) {
        ATprintf("parsed multi action identifier set\n  %t\n", $$);
      }
    }
  | LBRACE ma_ids RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed multi action identifier set\n  %t\n", $$);
      }
    }
  ;

//multiaction identifiers
ma_ids:
  ma_id
    {
      $$ = ATmakeList0();
      if (gsDebug) {
        ATprintf("parsed multi actions\n  %t\n", $$);
      }
    }
  | ma_ids COMMA ma_id
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed multi actions\n  %t\n", $$);
      }
    }
  ;

//multiaction identifier
ma_id:
  ma_id_elts
    {
      $$ = gsMakeMAId(ATreverse($1));
      if (gsDebug) {
        ATprintf("parsed multi action\n  %t\n", $$);
      }
    }
  ;

//multiaction identifier elements
ma_id_elts:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed multi action elements\n  %t\n", $$);
      }
    }
  | ma_id_elts BAR ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed multi action elements\n  %t\n", $$);
      }
    }
  ;

//set of renaming expressions
ren_expr_set:
  PBRACE
    {
      $$ = ATmakeList0();
      if (gsDebug) {
        ATprintf("parsed renaming expression set\n  %t\n", $$);
      }
    }
  | LBRACE ren_exprs RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed renaming expression set\n  %t\n", $$);
      }
    }
  ;

//renaming expressions
ren_exprs:
  ren_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed renaming expressions\n  %t\n", $$);
      }
    }
  | ren_exprs COMMA ren_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed renaming expressions\n  %t\n", $$);
      }
    }
  ;

//renaming expression
ren_expr:
  ID ARROW ID
    {
      $$ = gsMakeRenExpr($1, $3);
      if (gsDebug) {
        ATprintf("parsed renaming expression\n  %t\n", $$);
      }
    }
  ;

//set of communication expressions
comm_expr_set:
  PBRACE
    {
      $$ = ATmakeList0();
      if (gsDebug) {
        ATprintf("parsed communication expression set\n  %t\n", $$);
      }
    }
  | LBRACE comm_exprs RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed communication expression set\n  %t\n", $$);
      }
    }
  ;

//communication expressions
comm_exprs:
  comm_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed communication expressions\n  %t\n", $$);
      }
    }
  | comm_exprs COMMA comm_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed communication expressions\n  %t\n", $$);
      }
    }
  ;

//communication expression
comm_expr:
  ma_id
    {
      $$ = gsMakeCommExpr($1, gsMakeNil());
      if (gsDebug) {
        ATprintf("parsed communication expression\n  %t\n", $$);
      }
    }
  | ma_id ARROW ID
    {
      $$ = gsMakeCommExpr($1, $3);
      if (gsDebug) {
        ATprintf("parsed communication expression\n  %t\n", $$);
      }
    }
  ;

%% 

ATermAppl gsSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList EqnSects = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ProcDecls = ATmakeList0();
  ATermAppl Init = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    char *HeadName = ATgetName(ATgetAFun(SpecElt));
    if (gsIsInit(HeadName)) {
      if (Init == NULL) {
        Init = SpecElt;
      } else {
        //Init != NULL
        ThrowM0("parse error: multiple initialisations\n");
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsSortSpec(HeadName)) {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      } else if (gsIsConsSpec(HeadName)) {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      } else if (gsIsMapSpec(HeadName)) {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      } else if (gsIsEqnSpec(HeadName)) {
        EqnSects = ATconcat(EqnSects, SpecEltArg0);
      } else if (gsIsActSpec(HeadName)) {
        ActDecls = ATconcat(ActDecls, SpecEltArg0);
      } else if (gsIsProcSpec(HeadName)) {
        ProcDecls = ATconcat(ProcDecls, SpecEltArg0);
      }
    }
  }
  //check whether an initialisation is present
  if (Init == NULL) {
    ThrowM0("parse error: missing initialisation\n");
  }
  Result = gsMakeSpec(
    gsMakeSortSpec(SortDecls),
    gsMakeConsSpec(ConsDecls),
    gsMakeMapSpec(MapDecls),
    gsMakeEqnSpec(EqnSects),
    gsMakeActSpec(ActDecls),
    gsMakeProcSpec(ProcDecls),
    Init
  );
finally:
  return Result;
}
