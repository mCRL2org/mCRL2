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

%type <appl> spec spec_elt sort_spec cons_spec map_spec eqn_spec eqn_sect
%type <appl> eqn_decl act_spec proc_spec proc_decl initialisation sort_expr
%type <appl> sort_expr_arrow domain_no_arrow_elt sort_expr_struct
%type <appl> struct_constructor recogniser struct_projection sort_expr_primary
%type <appl> sort_constant sort_constructor data_expr data_expr_whr whr_decl
%type <appl> data_expr_lambda data_expr_imp data_expr_and data_expr_eq
%type <appl> data_expr_rel data_expr_cons data_expr_snoc data_expr_concat
%type <appl> data_expr_add data_expr_mult data_expr_quant data_expr_prefix
%type <appl> data_expr_postfix data_expr_primary data_constant data_enumeration
%type <appl> bag_enum_elt data_comprehension data_var_decl proc_expr
%type <appl> proc_expr_choice proc_expr_sum proc_expr_merge proc_expr_binit
%type <appl> proc_expr_cond proc_expr_seq proc_expr_at proc_expr_sync
%type <appl> proc_expr_primary proc_constant act_proc_ref proc_quant
%type <appl> mult_act_name ren_expr comm_expr

%type <list> spec_elts sorts_decls_scs sorts_decl ids_cs domain ops_decls_scs
%type <list> ops_decl eqn_decls_scs data_vars_decls_scs data_vars_decl
%type <list> acts_decls_scs acts_decl proc_decls_scs data_vars_decls_cs
%type <list> domain_no_arrow domain_no_arrow_elts_hs struct_constructors_bs
%type <list> struct_projections_cs whr_decls_cs data_exprs_cs bag_enum_elts_cs
%type <list> mult_act_names_set mult_act_names_cs ids_bs ren_expr_set
%type <list> ren_exprs_cs comm_expr_set comm_exprs_cs

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
  SORT sorts_decls_scs
    {
      $$ = gsMakeSortSpec($2);
      if (gsDebug) {
        ATprintf("parsed sort specification\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more sorts, separated by semicolons
sorts_decls_scs:
  sorts_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed sort declarations\n  %t\n", $$);
      }
    }
  | sorts_decls_scs sorts_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      if (gsDebug) {
        ATprintf("parsed sort declarations\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more sorts
sorts_decl:
  ids_cs
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeSortDeclStandard(ATAelementAt($1, i)));
      }
      if (gsDebug) {
        ATprintf("parsed standard sort declarations\n  %t\n", $$);
      }
    }
  | ID EQUALS sort_expr
    {
      $$ = ATmakeList1((ATerm) gsMakeSortDeclRef($1, $3));
      if (gsDebug) {
        ATprintf("parsed reference sort declarations\n  %t\n", $$);
      }
    }
  ;

//one or more identifiers, separated by comma's
ids_cs:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
      }
    }
  | ids_cs COMMA ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
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
  CONS ops_decls_scs
    {
      $$ = gsMakeConsSpec($2);
      if (gsDebug) {
        ATprintf("parsed constructor operation specification\n  %t\n", $$);
      }
    }
  ;

//operation specification
map_spec:
  MAP ops_decls_scs
    {
      $$ = gsMakeMapSpec($2);
      if (gsDebug) {
        ATprintf("parsed operation specification\n  %t\n", $$);
      }
    }
  ;

//one or more declarations of one or more operations of the same sort,
//separated by semicolons
ops_decls_scs:
  ops_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed operation declarations\n  %t\n", $$);
      }
    }
  | ops_decls_scs ops_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      if (gsDebug) {
        ATprintf("parsed operation declarations\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more operations of the same sort
ops_decl:
  ids_cs COLON sort_expr
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeOpDecl(ATAelementAt($1, i), $3));
      }
      if (gsDebug) {
        ATprintf("parsed operation declarations\n  %t\n", $$);
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
  EQN eqn_decls_scs
    {
      $$ = gsMakeEqnSect(ATmakeList0(), ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed equation section\n  %t\n", $$);
      }
    }
  | VAR data_vars_decls_scs EQN eqn_decls_scs
    {
      $$ = gsMakeEqnSect($2, ATreverse($4));
      if (gsDebug) {
        ATprintf("parsed equation section\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more equations, separated by semicolons
eqn_decls_scs:
  eqn_decl SEMICOLON
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed equation declarations\n  %t\n", $$);
      }
    }
  | eqn_decls_scs eqn_decl SEMICOLON
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed equation declarations\n  %t\n", $$);
      }
    }
  ;

//equation declaration
eqn_decl:
  data_expr EQUALS data_expr
    {
      $$ = gsMakeEqnDecl($1, $3);
      if (gsDebug) {
        ATprintf("parsed equation declaration\n  %t\n", $$);
      }
    }
  ;

//one or more declarations of one or more data variables,
//separated by semicolons
data_vars_decls_scs:
  data_vars_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed data variable declarations\n  %t\n", $$);
      }
    }
  | data_vars_decls_scs data_vars_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      if (gsDebug) {
        ATprintf("parsed data variable declarations\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more data variables
data_vars_decl:
  ids_cs COLON sort_expr
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeDataVarDecl(ATAelementAt($1, i), $3));
      }
      if (gsDebug) {
        ATprintf("parsed data variable declarations\n  %t\n", $$);
      }
    }
  ;

//action specification
act_spec:
  ACT acts_decls_scs
    {
      $$ = gsMakeActSpec($2);
      if (gsDebug) {
        ATprintf("parsed action specification\n  %t\n", $$);
      }
    }
  ;

//one or more declarations of one or more actions, separated by semicolons
acts_decls_scs:
  acts_decl SEMICOLON
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  | acts_decls_scs acts_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more actions
acts_decl:
  ids_cs
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$,
          (ATerm) gsMakeActDecl(ATAelementAt($1, i), ATmakeList0()));
      }
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  | ids_cs COLON domain
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeActDecl(ATAelementAt($1, i), $3));
      }
      if (gsDebug) {
        ATprintf("parsed action declarations\n  %t\n", $$);
      }
    }
  ;

//process specification
proc_spec:
  PROC proc_decls_scs
    {
      $$ = gsMakeProcSpec(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed process specification\n  %t\n", $$);
      }
    }
  ;

//one or more process declarations, separated by semicolons
proc_decls_scs:
  proc_decl SEMICOLON
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed process declarations\n  %t\n", $$);
      }
    }
  | proc_decls_scs proc_decl SEMICOLON
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf("parsed process declarations\n  %t\n", $$);
      }
    }
  ;

//process declaration
proc_decl:
  ID EQUALS proc_expr
    {
      $$ = gsMakeProcDecl($1, ATmakeList0(), $3);
      if (gsDebug) {
        ATprintf("parsed process declaration\n  %t\n", $$);
      }
    }
  | ID LPAR data_vars_decls_cs RPAR EQUALS proc_expr
    {
      $$ = gsMakeProcDecl($1, $3, $6);
      if (gsDebug) {
        ATprintf("parsed process declaration\n  %t\n", $$);
      }
    }
  ;

//one or more declarations of one or more operation variables, separated by
//comma's
data_vars_decls_cs:
  data_vars_decl
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed data variable declarations\n  %t\n", $$);
      }
    }
  | data_vars_decls_cs COMMA data_vars_decl
    {
      $$ = ATconcat($1, $3);
      if (gsDebug) {
        ATprintf("parsed data variable declarations\n  %t\n", $$);
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
  sort_expr_struct
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

//domain
domain_no_arrow:
  domain_no_arrow_elts_hs
    {
      $$ = ATreverse($1);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain\n  %t\n", $$);
      }
    }
  ;

//one or more domain elements, separated by hashes
domain_no_arrow_elts_hs:
  domain_no_arrow_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain elements\n  %t\n", $$);
      }
    }
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed non-arrow domain elements\n  %t\n", $$);
      }
    }
  ;

//domain element
domain_no_arrow_elt:
  sort_expr_struct
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf("parsed non-arrow domain element\n  %t\n", $$);
      }
    }
  ;

//structured sort
sort_expr_struct:
  sort_expr_primary
    {
      $$ = $1;
    }
  | STRUCT struct_constructors_bs
    {
      $$ = gsMakeSortStruct(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed structured sort\n  %t\n", $$);
      }
    }
  ;

//one ore more structured sort constructors, separated by bars
struct_constructors_bs:
  struct_constructor
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed structured sort constructors\n  %t\n", $$);
      }
    }
  | struct_constructors_bs BAR struct_constructor
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed structured sort constructors\n  %t\n", $$);
      }
    }
  ;

//structured sort constructor
struct_constructor:
  ID recogniser
    {
      $$ = gsMakeStructCons($1, ATmakeList0(), $2);
      if (gsDebug) {
        ATprintf("parsed structured sort constructor\n  %t\n", $$);
      }
    }
  | ID LPAR struct_projections_cs RPAR recogniser
    {
      $$ = gsMakeStructCons($1, ATreverse($3), $5);
      if (gsDebug) {
        ATprintf("parsed structured sort constructor\n  %t\n", $$);
      }
    }
  ;

//recogniser
recogniser:
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

//one or more structured sort projections, separated by comma's
struct_projections_cs:
  struct_projection
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed structured sort projections\n  %t\n", $$);
      }
    }
  | struct_projections_cs COMMA struct_projection
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed structured sort projections\n  %t\n", $$);
      }
    }
  ;

//structured sort projection
struct_projection:
  domain
    {
      $$ = gsMakeStructProj(gsMakeNil(), $1);
      if (gsDebug) {
        ATprintf("parsed structured sort projection\n  %t\n", $$);
      }
    }
  | ID COLON domain
    {
      $$ = gsMakeStructProj($1, $3);
      if (gsDebug) {
        ATprintf("parsed structured sort projection\n  %t\n", $$);
      }
    }
  ;

//primary sort expression
sort_expr_primary:
  ID
    {
      $$ = gsMakeSortId($1);
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
  | data_expr_whr WHR whr_decls_cs END
    {
      $$ = gsMakeWhr($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed where clause\n  %t\n", $$);
      }
    }
  ;

//declaration of one or more where clauses, separated by comma's
whr_decls_cs:
  whr_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed where clause declarations\n  %t\n", $$);
      }
    }
  | whr_decls_cs COMMA whr_decl
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
  | LAMBDA data_vars_decls_cs DOT data_expr_lambda
    {
      $$ = gsMakeLambda($2, $4);
      if (gsDebug) {
        ATprintf("parsed lambda abstraction\n  %t\n", $$);
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
  | FORALL data_vars_decls_cs DOT data_expr_quant
    {
      $$ = gsMakeForall($2, $4);
      if (gsDebug) {
        ATprintf("parsed quantification\n  %t\n", $$);
      }
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant
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
  | data_expr_postfix LPAR data_exprs_cs RPAR
    {
      $$ = gsMakeDataAppl($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed postfix data expression\n  %t\n", $$);
      }
    }
  ;

//one or more data expressions, separated by comma's
data_exprs_cs:
  data_expr
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed data expressions\n  %t\n", $$);
      }
    }
  | data_exprs_cs COMMA data_expr
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
      $$ = gsMakeDataVarOpId($1);
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
      $$ = gsMakeNumber($1);
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
  LBRACK data_exprs_cs RBRACK
    {
      $$ = gsMakeListEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  | LBRACE data_exprs_cs RBRACE
    {
      $$ = gsMakeSetEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  | LBRACE bag_enum_elts_cs RBRACE
    {
      $$ = gsMakeBagEnum(ATreverse($2));
      if (gsDebug) {
        ATprintf("parsed data enumeration\n  %t\n", $$);
      }
    }
  ;

//one or more bag enumeration elements, separated by comma's
bag_enum_elts_cs:
  bag_enum_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed bag enumeration elements\n  %t\n", $$);
      }
    }
  | bag_enum_elts_cs COMMA bag_enum_elt
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
  LBRACE data_var_decl BAR data_expr RBRACE
    {
      $$ = gsMakeSetBagComp($2, $4);
      if (gsDebug) {
        ATprintf("parsed data comprehension\n  %t\n", $$);
      }
    }
  ;

//declaration of a data variable
data_var_decl:
  ID COLON sort_expr
    {
      $$ = gsMakeDataVarDecl($1, $3);
      if (gsDebug) {
        ATprintf("parsed data variable declaration\n  %t\n", $$);
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
  proc_expr_sum
    {
      $$ = $1;
    }
  | proc_expr_choice PLUS proc_expr_sum
    {
      $$ = gsMakeChoice($1, $3);
      if (gsDebug) {
        ATprintf("parsed choice expression\n  %t\n", $$);
      }
    }
  ;

//summation
proc_expr_sum:
  proc_expr_merge
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_sum
    {
      $$ = gsMakeSum($2, $4);
      if (gsDebug) {
        ATprintf("parsed summation\n  %t\n", $$);
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
      $$ = gsMakeCond($1, $3, gsMakeDelta());
      if (gsDebug) {
        ATprintf("parsed conditional expression\n  %t\n", $$);
      }
    }
  | data_expr_prefix ARROW proc_expr_seq COMMA proc_expr_seq
    {
      $$ = gsMakeCond($1, $3, $5);
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
  | act_proc_ref
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
act_proc_ref:
  ID
    {
      $$ = gsMakeActProcRef($1, ATmakeList0());
      if (gsDebug) {
        ATprintf("parsed action or process reference\n  %t\n", $$);
      }
    }
  | ID LPAR data_exprs_cs RPAR
    {
      $$ = gsMakeActProcRef($1, ATreverse($3));
      if (gsDebug) {
        ATprintf("parsed action or process reference\n  %t\n", $$);
      }
    }
  ;

//process quantification
proc_quant:
  RESTRICT LPAR mult_act_names_set COMMA proc_expr RPAR
    {
      $$ = gsMakeRestrict($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | ALLOW LPAR mult_act_names_set COMMA proc_expr RPAR
    {
      $$ = gsMakeAllow($3, $5);
      if (gsDebug) {
        ATprintf("parsed process quantification\n  %t\n", $$);
      }
    }
  | HIDE LPAR mult_act_names_set COMMA proc_expr RPAR
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

//set of multi action names
mult_act_names_set:
  PBRACE
    {
      $$ = ATmakeList0();
      if (gsDebug) {
        ATprintf("parsed multi action name set\n  %t\n", $$);
      }
    }
  | LBRACE mult_act_names_cs RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed multi action name set\n  %t\n", $$);
      }
    }
  ;

//one or more multi action names, separated by comma's
mult_act_names_cs:
  mult_act_name
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed multi action names\n  %t\n", $$);
      }
    }
  | mult_act_names_cs COMMA mult_act_name
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed multi action names\n  %t\n", $$);
      }
    }
  ;

//multi action name
mult_act_name:
  ids_bs
    {
      $$ = gsMakeMultActName(ATreverse($1));
      if (gsDebug) {
        ATprintf("parsed multi action name\n  %t\n", $$);
      }
    }
  ;

//one or more id's, separated by bars
ids_bs:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
      }
    }
  | ids_bs BAR ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed id's\n  %t\n", $$);
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
  | LBRACE ren_exprs_cs RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed renaming expression set\n  %t\n", $$);
      }
    }
  ;

//one or more renaming expressions, separated by comma's
ren_exprs_cs:
  ren_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed renaming expressions\n  %t\n", $$);
      }
    }
  | ren_exprs_cs COMMA ren_expr
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
  | LBRACE comm_exprs_cs RBRACE
    {
      $$ = ATreverse($2);
      if (gsDebug) {
        ATprintf("parsed communication expression set\n  %t\n", $$);
      }
    }
  ;

//one or more communication expressions, separated by comma's
comm_exprs_cs:
  comm_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf("parsed communication expressions\n  %t\n", $$);
      }
    }
  | comm_exprs_cs COMMA comm_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      if (gsDebug) {
        ATprintf("parsed communication expressions\n  %t\n", $$);
      }
    }
  ;

//communication expression
comm_expr:
  mult_act_name
    {
      $$ = gsMakeCommExpr($1, gsMakeNil());
      if (gsDebug) {
        ATprintf("parsed communication expression\n  %t\n", $$);
      }
    }
  | mult_act_name ARROW ID
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
