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
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

//Global precondition: the ATerm library has been initialised

//external declarations from mcrl2lexer.l
void mcrl2yyerror(const char *s);
int mcrl2yylex(void);
extern ATermAppl SpecTree;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 20000

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

//set name prefix
%name-prefix="mcrl2yy"

//start token
%start spec

//terminals
//---------

%token <appl> LMERGE ARROW LTE GTE CONS SNOC CONCAT EQ NEQ AND BARS IMP BINIT
%token <appl> ELSE
%token <appl> STAR PLUS MINUS EQUALS DOT COMMA COLON SEMICOLON QMARK EXCLAM AT
%token <appl> HASH BAR
%token <appl> LPAR RPAR PBRACK LBRACK RBRACK LANG RANG PBRACE LBRACE RBRACE
%token <appl> KWSORT KWCONS KWMAP KWVAR KWEQN KWACT KWPROC KWINIT KWSTRUCT
%token <appl> BOOL POS NAT INT LIST SET BAG
%token <appl> TRUE FALSE WHR END LAMBDA FORALL EXISTS DIV MOD IN
%token <appl> DELTA TAU SUM BLOCK ALLOW HIDE RENAME COMM
%token <appl> ID NUMBER

//non-terminals
//-------------

%type <appl> spec spec_elt sort_spec cons_spec map_spec eqn_spec
%type <appl> eqn_decl act_spec proc_spec proc_decl initialisation sort_expr
%type <appl> sort_expr_arrow domain_no_arrow_elt sort_expr_struct
%type <appl> struct_constructor recogniser struct_projection sort_expr_primary
%type <appl> sort_constant sort_constructor data_expr data_expr_whr whr_decl
%type <appl> data_expr_quant data_expr_imp data_expr_imp_rhs data_expr_and
%type <appl> data_expr_and_rhs data_expr_eq data_expr_eq_rhs data_expr_rel
%type <appl> data_expr_cons data_expr_snoc data_expr_concat data_expr_add
%type <appl> data_expr_div data_expr_mult data_expr_prefix
%type <appl> data_expr_quant_prefix data_expr_postfix data_expr_primary
%type <appl> data_constant data_enumeration bag_enum_elt data_comprehension
%type <appl> data_var_decl
%type <appl> proc_expr proc_expr_choice proc_expr_sum proc_expr_merge
%type <appl> proc_expr_merge_rhs proc_expr_binit proc_expr_binit_rhs
%type <appl> proc_expr_cond proc_expr_cond_la proc_expr_seq
%type <appl> proc_expr_seq_wo_cond proc_expr_seq_rhs proc_expr_seq_rhs_wo_cond
%type <appl> proc_expr_at proc_expr_at_wo_cond proc_expr_sync
%type <appl> proc_expr_sync_wo_cond proc_expr_sync_rhs
%type <appl> proc_expr_sync_rhs_wo_cond proc_expr_primary proc_constant
%type <appl> act_proc_ref proc_quant mult_act_name ren_expr comm_expr

%type <list> spec_elts sorts_decls_scs sorts_decl ids_cs domain ops_decls_scs
%type <list> ops_decl eqn_sect eqn_decls_scs data_vars_decls_scs data_vars_decl
%type <list> acts_decls_scs acts_decl proc_decls_scs data_vars_decls_cs
%type <list> domain_no_arrow domain_no_arrow_elts_hs struct_constructors_bs
%type <list> struct_projections_cs whr_decls_cs data_exprs_cs bag_enum_elts_cs
%type <list> act_names_set ren_expr_set ren_exprs_cs comm_expr_set comm_exprs_cs
%type <list> mult_act_names_set mult_act_names_cs ids_bs

%%

//specification
spec:
  spec_elts
    {
      $$ = gsSpecEltsToSpec(ATreverse($1));
      gsDebugMsg("parsed specification\n  %T\n", $$);
      SpecTree = $$;
    }
  ;

//specification elements
spec_elts:
  spec_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed specification elements\n  %T\n", $$);
    }
   | spec_elts spec_elt
    {
      $$ = ATinsert($1, (ATerm) $2);
      gsDebugMsg("parsed specification elements\n  %T\n", $$);
    }
   ;

//specification element
spec_elt:
  sort_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    } 
  | cons_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | map_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | eqn_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | act_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | proc_spec
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | initialisation
    {
      $$ = $1;
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  ;

//sort specification
sort_spec:
  KWSORT sorts_decls_scs
    {
      $$ = gsMakeSortSpec($2);
      gsDebugMsg("parsed sort specification\n  %T\n", $$);
    }
  ;

//declaration of one or more sorts, separated by semicolons
sorts_decls_scs:
  sorts_decl SEMICOLON
    {
      $$ = $1;
      gsDebugMsg("parsed sort declarations\n  %T\n", $$);
    }
  | sorts_decls_scs sorts_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      gsDebugMsg("parsed sort declarations\n  %T\n", $$);
    }
  ;

//declaration of one or more sorts
sorts_decl:
  ids_cs
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeSortId(ATAelementAt($1, i)));
      }
      gsDebugMsg("parsed standard sort declarations\n  %T\n", $$);
    }
  | ID EQUALS sort_expr
    {
      $$ = ATmakeList1((ATerm) gsMakeSortRef($1, $3));
      gsDebugMsg("parsed reference sort declarations\n  %T\n", $$);
    }
  ;

//one or more identifiers, separated by comma's
ids_cs:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  | ids_cs COMMA ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  ;

//domain
domain:
  domain_no_arrow
    {
      $$ = $1;
      gsDebugMsg("parsed domain\n  %T\n", $$);
    }
  | domain_no_arrow ARROW sort_expr
    {
      $$ = ATmakeList1((ATerm) gsMakeSortArrowProd($1, $3));
      gsDebugMsg("parsed domain\n  %T\n", $$);
    }
  ;

//constructor operation specification
cons_spec:
  KWCONS ops_decls_scs
    {
      $$ = gsMakeConsSpec($2);
      gsDebugMsg("parsed constructor operation specification\n  %T\n", $$);
    }
  ;

//operation specification
map_spec:
  KWMAP ops_decls_scs
    {
      $$ = gsMakeMapSpec($2);
      gsDebugMsg("parsed operation specification\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more operations of the same sort,
//separated by semicolons
ops_decls_scs:
  ops_decl SEMICOLON
    {
      $$ = $1;
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  | ops_decls_scs ops_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  ;

//declaration of one or more operations of the same sort
ops_decl:
  ids_cs COLON sort_expr
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeOpId(ATAelementAt($1, i), $3));
      }
      gsDebugMsg("parsed operation declarations\n  %T\n", $$);
    }
  ;

//equation specification
eqn_spec:
  eqn_sect    
    {
      $$ = gsMakeDataEqnSpec($1);
      gsDebugMsg("parsed equation specification\n  %T\n", $$);
    }
  ;

//equation section
eqn_sect:
  KWEQN eqn_decls_scs
    {
      $$ = ATreverse($2);
      gsDebugMsg("parsed equation section\n  %T\n", $$);
    }
  | KWVAR data_vars_decls_scs KWEQN eqn_decls_scs
    {
      $$ = ATmakeList0();
      int n = ATgetLength($4);
      for (int i = 0; i < n; i++) {
        ATermAppl DataEqn = ATAelementAt($4, i);
        $$ = ATinsert($$, (ATerm) gsMakeDataEqn($2, ATAgetArgument(DataEqn, 1),
          ATAgetArgument(DataEqn, 2), ATAgetArgument(DataEqn, 3)));
      }
      gsDebugMsg("parsed equation section\n  %T\n", $$);
    }
  ;

//declaration of one or more equations, separated by semicolons
eqn_decls_scs:
  eqn_decl SEMICOLON
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed equation declarations\n  %T\n", $$);
    }
  | eqn_decls_scs eqn_decl SEMICOLON
    {
      $$ = ATinsert($1, (ATerm) $2);
      gsDebugMsg("parsed equation declarations\n  %T\n", $$);
    }
  ;

//equation declaration
eqn_decl:
  data_expr EQUALS data_expr
    {
      $$ = gsMakeDataEqn(ATmakeList0(), gsMakeNil(), $1, $3);
      gsDebugMsg("parsed equation declaration\n  %T\n", $$);
    }
  | data_expr ARROW data_expr EQUALS data_expr
    {
      $$ = gsMakeDataEqn(ATmakeList0(), $1, $3, $5);
      gsDebugMsg("parsed equation declaration\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more data variables,
//separated by semicolons
data_vars_decls_scs:
  data_vars_decl SEMICOLON
    {
      $$ = $1;
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  | data_vars_decls_scs data_vars_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//declaration of one or more data variables
data_vars_decl:
  ids_cs COLON sort_expr
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeDataVarId(ATAelementAt($1, i), $3));
      }
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//action specification
act_spec:
  KWACT acts_decls_scs
    {
      $$ = gsMakeActSpec($2);
      gsDebugMsg("parsed action specification\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more actions, separated by semicolons
acts_decls_scs:
  acts_decl SEMICOLON
    {
      $$ = $1;
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  | acts_decls_scs acts_decl SEMICOLON
    {
      $$ = ATconcat($1, $2);
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
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
          (ATerm) gsMakeActId(ATAelementAt($1, i), ATmakeList0()));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  | ids_cs COLON domain
    {
      $$ = ATmakeList0();
      int n = ATgetLength($1);
      for (int i = 0; i < n; i++) {
        $$ = ATinsert($$, (ATerm) gsMakeActId(ATAelementAt($1, i), $3));
      }
      gsDebugMsg("parsed action declarations\n  %T\n", $$);
    }
  ;

//process specification
proc_spec:
  KWPROC proc_decls_scs
    {
      $$ = gsMakeProcEqnSpec(ATreverse($2));
      gsDebugMsg("parsed process specification\n  %T\n", $$);
    }
  ;

//one or more process declarations, separated by semicolons
proc_decls_scs:
  proc_decl SEMICOLON
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed process declarations\n  %T\n", $$);
    }
  | proc_decls_scs proc_decl SEMICOLON
    {
      $$ = ATinsert($1, (ATerm) $2);
      gsDebugMsg("parsed process declarations\n  %T\n", $$);
    }
  ;

//process declaration
proc_decl:
  ID EQUALS proc_expr
    {
      $$ = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId($1, ATmakeList0()), ATmakeList0(), $3);
      gsDebugMsg("parsed process declaration\n  %T\n", $$);
    }
  | ID LPAR data_vars_decls_cs RPAR EQUALS proc_expr
    {
      ATermList SortExprs = ATmakeList0();
      int n = ATgetLength($3);
      for (int i = 0; i < n; i++) {
        SortExprs = ATinsert(SortExprs, ATgetArgument(ATAelementAt($3, i), 1));
      }      
      $$ = gsMakeProcEqn(
        ATmakeList0(), gsMakeProcVarId($1, ATreverse(SortExprs)), $3, $6);
      gsDebugMsg("parsed process declaration\n  %T\n", $$);
    }
  ;

//one or more declarations of one or more data variables, separated by
//comma's
data_vars_decls_cs:
  data_vars_decl
    {
      $$ = $1;
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  | data_vars_decls_cs COMMA data_vars_decl
    {
      $$ = ATconcat($1, $3);
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//initialisation
initialisation:
  KWINIT proc_expr SEMICOLON
    {
      $$ = gsMakeInit(ATmakeList0(), $2);
      gsDebugMsg("parsed initialisation\n  %T\n", $$);
    }
  ;

//sort expression
sort_expr:
  sort_expr_arrow
    {
      $$ = $1;
      gsDebugMsg("parsed sort expression\n  %T\n", $$);
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
      $$ = gsMakeSortArrowProd($1, $3);
      gsDebugMsg("parsed arrow sort\n  %T\n", $$);
    }
  ;

//domain
domain_no_arrow:
  domain_no_arrow_elts_hs
    {
      $$ = ATreverse($1);
      gsDebugMsg("parsed non-arrow domain\n  %T\n", $$);
    }
  ;

//one or more domain elements, separated by hashes
domain_no_arrow_elts_hs:
  domain_no_arrow_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", $$);
    }
  | domain_no_arrow_elts_hs HASH domain_no_arrow_elt
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed non-arrow domain elements\n  %T\n", $$);
    }
  ;

//domain element
domain_no_arrow_elt:
  sort_expr_struct
    {
      $$ = $1;
      gsDebugMsg("parsed non-arrow domain element\n  %T\n", $$);
    }
  ;

//structured sort
sort_expr_struct:
  sort_expr_primary
    {
      $$ = $1;
    }
  | KWSTRUCT struct_constructors_bs
    {
      $$ = gsMakeSortStruct(ATreverse($2));
      gsDebugMsg("parsed structured sort\n  %T\n", $$);
    }
  ;

//one ore more structured sort constructors, separated by bars
struct_constructors_bs:
  struct_constructor
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed structured sort constructors\n  %T\n", $$);
    }
  | struct_constructors_bs BAR struct_constructor
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed structured sort constructors\n  %T\n", $$);
    }
  ;

//structured sort constructor
struct_constructor:
  ID recogniser
    {
      $$ = gsMakeStructCons($1, ATmakeList0(), $2);
      gsDebugMsg("parsed structured sort constructor\n  %T\n", $$);
    }
  | ID LPAR struct_projections_cs RPAR recogniser
    {
      $$ = gsMakeStructCons($1, ATreverse($3), $5);
      gsDebugMsg("parsed structured sort constructor\n  %T\n", $$);
    }
  ;

//recogniser
recogniser:
  /* empty */ 
    {
      $$ = gsMakeNil();
      gsDebugMsg("parsed recogniser\n  %T\n", $$);
    }
  | QMARK ID
    {
      $$ = $2;
      gsDebugMsg("parsed recogniser id\n  %T\n", $$);
    }
  ;

//one or more structured sort projections, separated by comma's
struct_projections_cs:
  struct_projection
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed structured sort projections\n  %T\n", $$);
    }
  | struct_projections_cs COMMA struct_projection
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed structured sort projections\n  %T\n", $$);
    }
  ;

//structured sort projection
struct_projection:
  sort_expr
    {
      $$ = gsMakeStructProj(gsMakeNil(), $1);
      gsDebugMsg("parsed structured sort projection\n  %T\n", $$);
    }
  | ID COLON sort_expr
    {
      $$ = gsMakeStructProj($1, $3);
      gsDebugMsg("parsed structured sort projection\n  %T\n", $$);
    }
  ;

//primary sort expression
sort_expr_primary:
  ID
    {
      $$ = gsMakeSortId($1);
      gsDebugMsg("parsed primary sort\n  %T\n", $$);
    }
  | sort_constant
    {
      $$ = $1;
      gsDebugMsg("parsed sort constant\n  %T\n", $$);
    }
  | sort_constructor
    {
      $$ = $1;
      gsDebugMsg("parsed sort constructor\n  %T\n", $$);
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
      $$ = gsMakeSortId($1);
    }
  | POS
    {
      $$ = gsMakeSortId($1);
    }
  | NAT
    {
      $$ = gsMakeSortId($1);
    }
  | INT
    {
      $$ = gsMakeSortId($1);
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
      gsDebugMsg("parsed data expression\n  %T\n", $$);
    }
  ;

//where clause
data_expr_whr:
  data_expr_quant
    {
      $$ = $1;
    }
  | data_expr_whr WHR whr_decls_cs END
    {
      $$ = gsMakeWhr($1, ATreverse($3));
      gsDebugMsg("parsed where clause\n  %T\n", $$);
    }
  ;

//declaration of one or more where clauses, separated by comma's
whr_decls_cs:
  whr_decl
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed where clause declarations\n  %T\n", $$);
    }
  | whr_decls_cs COMMA whr_decl
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed where clause declarations\n  %T\n", $$);
    }
  ;

//where clause declaration
whr_decl:
  ID EQUALS data_expr
    {
      $$ = gsMakeWhrDecl($1, $3);
      gsDebugMsg("parsed where clause declaration\n  %T\n", $$);
    }
  ;

//quantifications and lambda abstraction
data_expr_quant:
  data_expr_imp
    {
      $$ = $1;
    }
  | LAMBDA data_vars_decls_cs DOT data_expr_quant
    {
      $$ = gsMakeLambda($2, $4);
      gsDebugMsg("parsed lambda abstraction\n  %T\n", $$);
    }
  | FORALL data_vars_decls_cs DOT data_expr_quant
    {
      $$ = gsMakeForall($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant
    {
      $$ = gsMakeExists($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//implication (right associative)
data_expr_imp:
  data_expr_and
    {
      $$ = $1;
    }
  | data_expr_and IMP data_expr_imp_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed implication\n  %T\n", $$);
    }
  ;

//right argument of implication
data_expr_imp_rhs:
  data_expr_imp
    {
      $$ = $1;
    }
  | FORALL data_vars_decls_cs DOT data_expr_imp_rhs
    {
      $$ = gsMakeForall($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_imp_rhs
    {
      $$ = gsMakeExists($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//conjunction and disjunction (right associative)
data_expr_and:
  data_expr_eq
    {
      $$ = $1;
    }
  | data_expr_eq AND data_expr_and_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | data_expr_eq BARS data_expr_and_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed disjunction\n  %T\n", $$);
    }
  ;

//right argument of conjunction and disjunction
data_expr_and_rhs:
  data_expr_and
    {
      $$ = $1;
    }
  | FORALL data_vars_decls_cs DOT data_expr_and_rhs
    {
      $$ = gsMakeForall($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_and_rhs
    {
      $$ = gsMakeExists($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  ;

//equality (right associative)
data_expr_eq:
  data_expr_rel
    {
      $$ = $1;
    }
  | data_expr_rel EQ data_expr_eq_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
    }
  | data_expr_rel NEQ data_expr_eq_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
    }
  ;

//right argument of equality 
data_expr_eq_rhs:
  data_expr_eq
    {
      $$ = $1;
    }
  | FORALL data_vars_decls_cs DOT data_expr_eq_rhs
    {
      $$ = gsMakeForall($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_eq_rhs
    {
      $$ = gsMakeExists($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LTE data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons RANG data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LANG data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons IN data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  ;

//cons (right associative)
data_expr_cons:
  data_expr_snoc
    {
      $$ = $1;
    }
  | data_expr_add CONS data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed list cons expression\n  %T\n", $$);
    }
  ;

//snoc (left associative)
data_expr_snoc:
  data_expr_concat
    {
      $$ = $1;
    }
  | data_expr_snoc SNOC data_expr_add
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed list snoc expression\n  %T\n", $$);
    }
  ; 

//concatenation (left associative)
data_expr_concat:
  data_expr_add
    {
      $$ = $1;
    }
  | data_expr_concat CONCAT data_expr_add
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed list concat expression\n  %T\n", $$);
    }
  ;

//addition and subtraction (left associative)
data_expr_add:
  data_expr_div
    {
      $$ = $1;
    }
  | data_expr_add PLUS data_expr_div
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed addition or set union\n  %T\n", $$);
    }
  | data_expr_add MINUS data_expr_div
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed subtraction or set difference\n  %T\n", $$);
    }
  ;

//division (left associative)
data_expr_div:
  data_expr_mult
    {
      $$ = $1;
    }
  | data_expr_div DIV data_expr_mult
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed div expression\n  %T\n", $$);
    }
  | data_expr_div MOD data_expr_mult
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed mod expression\n  %T\n", $$);
    }
  ;

//multiplication and list at (left associative)
data_expr_mult:
  data_expr_prefix
    {
      $$ = $1;
    }
  | data_expr_mult STAR data_expr_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", $$);
    }
  | data_expr_mult DOT data_expr_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed list at expression\n  %T\n", $$);
    }
  ;

//prefix data expression
data_expr_prefix:
  data_expr_postfix
    {
      $$ = $1;
    }
  | EXCLAM data_expr_quant_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($1), ATmakeList1((ATerm) $2));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | MINUS data_expr_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($1), ATmakeList1((ATerm) $2));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | HASH data_expr_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeDataVarIdOpId($1), ATmakeList1((ATerm) $2));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  ;

//quantifier or prefix data expression
data_expr_quant_prefix:
  data_expr_prefix
    {
      $$ = $1;
    }
  | FORALL data_vars_decls_cs DOT data_expr_quant_prefix
    {
      $$ = gsMakeForall($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
    }
  | EXISTS data_vars_decls_cs DOT data_expr_quant_prefix
    {
      $$ = gsMakeExists($2, $4);
      gsDebugMsg("parsed quantification\n  %T\n", $$);
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
      $$ = gsMakeDataApplProd($1, ATreverse($3));
      gsDebugMsg("parsed postfix data expression\n  %T\n", $$);
    }
  ;

//one or more data expressions, separated by comma's
data_exprs_cs:
  data_expr
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed data expressions\n  %T\n", $$);
    }
  | data_exprs_cs COMMA data_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed data expressions\n  %T\n", $$);
    }
  ;
  
//primary data expression
data_expr_primary:
  ID
    {
      $$ = gsMakeDataVarIdOpId($1);
      gsDebugMsg("parsed primary data expression\n  %T\n", $$);
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
      $$ = gsMakeDataVarIdOpId($1);
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | FALSE
    {
      $$ = gsMakeDataVarIdOpId($1);
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | NUMBER
    {
      $$ = gsMakeNumber($1, gsMakeUnknown());
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | PBRACK
    {
      $$ = gsMakeDataVarIdOpId($1);
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | PBRACE
    {
      $$ = gsMakeDataVarIdOpId($1);
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs_cs RBRACK
    {
      $$ = gsMakeListEnum(ATreverse($2), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE data_exprs_cs RBRACE
    {
      $$ = gsMakeSetEnum(ATreverse($2), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE bag_enum_elts_cs RBRACE
    {
      $$ = gsMakeBagEnum(ATreverse($2), gsMakeUnknown());
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  ;

//one or more bag enumeration elements, separated by comma's
bag_enum_elts_cs:
  bag_enum_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  | bag_enum_elts_cs COMMA bag_enum_elt
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
    {
      $$ = gsMakeBagEnumElt($1, $3);
      gsDebugMsg("parsed bag enumeration element\n  %T\n", $$);
    }
  ;

//comprehension
data_comprehension:
  LBRACE data_var_decl BAR data_expr RBRACE
    {
      $$ = gsMakeSetBagComp($2, $4);
      gsDebugMsg("parsed data comprehension\n  %T\n", $$);
    }
  ;

//declaration of a data variable
data_var_decl:
  ID COLON sort_expr
    {
      $$ = gsMakeDataVarId($1, $3);
      gsDebugMsg("parsed data variable declaration\n  %T\n", $$);
    }
  ;

//process expression
proc_expr:
  proc_expr_choice
    {
      $$ = $1;
      gsDebugMsg("parsed process expression\n  %T\n", $$);
    }
  ;

//choice (right associative)
proc_expr_choice:
  proc_expr_sum
    {
      $$ = $1;
    }
  | proc_expr_sum PLUS proc_expr_choice
    {
      $$ = gsMakeChoice($1, $3);
      gsDebugMsg("parsed choice expression\n  %T\n", $$);
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
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  ;

//merge and left merge (right associative)
proc_expr_merge:
  proc_expr_binit
    {
      $$ = $1;
    }
  | proc_expr_binit BARS proc_expr_merge_rhs
    {
      $$ = gsMakeMerge($1, $3);
      gsDebugMsg("parsed merge expression\n  %T\n", $$);
    }
  | proc_expr_binit LMERGE proc_expr_merge_rhs
    {
      $$ = gsMakeLMerge($1, $3);
      gsDebugMsg("parsed left merge expression\n  %T\n", $$);
    }
  ;

//right argument of merge
proc_expr_merge_rhs:
  proc_expr_merge
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_merge_rhs
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  ;

//bounded initialisation (left associative)
proc_expr_binit:
  proc_expr_cond
    {
      $$ = $1;
    }
  | proc_expr_binit BINIT proc_expr_binit_rhs
    {
      $$ = gsMakeBInit($1, $3);
      gsDebugMsg("parsed bounded initialisation expression\n  %T\n", $$);
    }
  ;

//right argument of bounded initialisation
proc_expr_binit_rhs:
  proc_expr_cond
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_binit_rhs
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  ;

//conditional
proc_expr_cond:
  proc_expr_seq
    {
      $$ = $1;
    }
  | data_expr_prefix ARROW proc_expr_cond_la
    {
      $$ = gsMakeCond($1, $3, gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_cond_la
    {
      $$ = gsMakeCond($1, $3, $5);
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  ;

//last argument of conditional
proc_expr_cond_la:
  proc_expr_cond
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_cond_la
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  ;

//sequential composition (right associative)
proc_expr_seq:
  proc_expr_at
    {
      $$ = $1;
    }
  | proc_expr_at DOT proc_expr_seq_rhs
    {
      $$ = gsMakeSeq($1, $3);
      gsDebugMsg("parsed sequential expression\n  %T\n", $$);
    }
  ;

//sequential composition not mentioning conditional
proc_expr_seq_wo_cond:
  proc_expr_at_wo_cond
    {
      $$ = $1;
    }
  | proc_expr_at DOT proc_expr_seq_rhs_wo_cond
    {
      $$ = gsMakeSeq($1, $3);
      gsDebugMsg("parsed sequential expression\n  %T\n", $$);
    }
  ;

//right argument of sequential composition
proc_expr_seq_rhs:
  proc_expr_seq
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs
    {
      $$ = gsMakeCond($1, $3, gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_seq_rhs_wo_cond ELSE proc_expr_seq_rhs
    {
      $$ = gsMakeCond($1, $3, $5);
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  ;

//right argument of sequential composition not mentioning conditional
proc_expr_seq_rhs_wo_cond:
  proc_expr_seq_wo_cond
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_seq_rhs_wo_cond
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed at time expression\n  %T\n", $$);
    }
  ;

//timed expression not mentioning conditional
proc_expr_at_wo_cond:
  proc_expr_sync_wo_cond
    {
      $$ = $1;
    }
  | proc_expr_at_wo_cond AT data_expr_prefix
    {
      $$ = gsMakeAtTime($1, $3);
      gsDebugMsg("parsed at time expression\n  %T\n", $$);
    }
  ;

//synchronisation (right associative)
proc_expr_sync:
  proc_expr_primary
    {
      $$ = $1;
    }
  | proc_expr_primary BAR proc_expr_sync_rhs
    {
      $$ = gsMakeSync($1, $3);
      gsDebugMsg("parsed sync expression\n  %T\n", $$);
    }
  ;

//synchronisation (right associative)
proc_expr_sync_wo_cond:
  proc_expr_primary
    {
      $$ = $1;
    }
  | proc_expr_primary BAR proc_expr_sync_rhs_wo_cond
    {
      $$ = gsMakeSync($1, $3);
      gsDebugMsg("parsed sync expression\n  %T\n", $$);
    }
  ;

//right argument of synchronisation
proc_expr_sync_rhs:
  proc_expr_sync
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs
    {
      $$ = gsMakeCond($1, $3, gsMakeDelta());
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  | data_expr_prefix ARROW proc_expr_sync_rhs_wo_cond ELSE proc_expr_sync_rhs
    {
      $$ = gsMakeCond($1, $3, $5);
      gsDebugMsg("parsed conditional expression\n  %T\n", $$);
    }
  ;

//right argument of synchronisation
proc_expr_sync_rhs_wo_cond:
  proc_expr_sync_wo_cond
    {
      $$ = $1;
    }
  | SUM data_vars_decls_cs DOT proc_expr_sync_rhs_wo_cond
    {
      $$ = gsMakeSum($2, $4);
      gsDebugMsg("parsed summation\n  %T\n", $$);
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
      gsDebugMsg("parsed process constant\n  %T\n", $$);
    }
  | TAU
    {
      $$ = gsMakeTau();
      gsDebugMsg("parsed process constant\n  %T\n", $$);
    }
  ;

//action or process
act_proc_ref:
  ID
    {
      $$ = gsMakeActionProcess($1, ATmakeList0());
      gsDebugMsg("parsed action or process\n  %T\n", $$);
    }
  | ID LPAR data_exprs_cs RPAR
    {
      $$ = gsMakeActionProcess($1, ATreverse($3));
      gsDebugMsg("parsed action or process\n  %T\n", $$);
    }
  ;

//process quantification
proc_quant:
  BLOCK LPAR act_names_set COMMA proc_expr RPAR
    {
      $$ = gsMakeBlock($3, $5);
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | HIDE LPAR act_names_set COMMA proc_expr RPAR
    {
      $$ = gsMakeHide($3, $5);
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR
    {
      $$ = gsMakeRename($3, $5);
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR
    {
      $$ = gsMakeComm($3, $5);
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  | ALLOW LPAR mult_act_names_set COMMA proc_expr RPAR
    {
      $$ = gsMakeAllow($3, $5);
      gsDebugMsg("parsed process quantification\n  %T\n", $$);
    }
  ;

//set of action names
act_names_set:
  PBRACE
    {
      $$ = ATmakeList0();
      gsDebugMsg("parsed action name set\n  %T\n", $$);
    }
  | LBRACE ids_cs RBRACE
    {
      $$ = ATreverse($2);
      gsDebugMsg("parsed action name set\n  %T\n", $$);
    }
  ;

//set of renaming expressions
ren_expr_set:
  PBRACE
    {
      $$ = ATmakeList0();
      gsDebugMsg("parsed renaming expression set\n  %T\n", $$);
    }
  | LBRACE ren_exprs_cs RBRACE
    {
      $$ = ATreverse($2);
      gsDebugMsg("parsed renaming expression set\n  %T\n", $$);
    }
  ;

//one or more renaming expressions, separated by comma's
ren_exprs_cs:
  ren_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed renaming expressions\n  %T\n", $$);
    }
  | ren_exprs_cs COMMA ren_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed renaming expressions\n  %T\n", $$);
    }
  ;

//renaming expression
ren_expr:
  ID ARROW ID
    {
      $$ = gsMakeRenameExpr($1, $3);
      gsDebugMsg("parsed renaming expression\n  %T\n", $$);
    }
  ;

//set of communication expressions
comm_expr_set:
  PBRACE
    {
      $$ = ATmakeList0();
      gsDebugMsg("parsed communication expression set\n  %T\n", $$);
    }
  | LBRACE comm_exprs_cs RBRACE
    {
      $$ = ATreverse($2);
      gsDebugMsg("parsed communication expression set\n  %T\n", $$);
    }
  ;

//one or more communication expressions, separated by comma's
comm_exprs_cs:
  comm_expr 
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed communication expressions\n  %T\n", $$);
    }
  | comm_exprs_cs COMMA comm_expr
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed communication expressions\n  %T\n", $$);
    }
  ;

//communication expression
comm_expr:
  mult_act_name
    {
      $$ = gsMakeCommExpr($1, gsMakeNil());
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | mult_act_name ARROW TAU
    {      
      $$ = gsMakeCommExpr($1, gsMakeNil());
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | mult_act_name ARROW ID
    {      
      $$ = gsMakeCommExpr($1, $3);
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  ;

//multi action name
mult_act_name:
  ids_bs
    {
      $$ = gsMakeMultActName(ATreverse($1));
      gsDebugMsg("parsed multi action name\n  %T\n", $$);
    }
  ;

//one or more id's, separated by bars
ids_bs:
  ID
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  | ids_bs BAR ID
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed id's\n  %T\n", $$);
    }
  ;

//set of multi action names
mult_act_names_set:
  PBRACE
    {
      $$ = ATmakeList0();
      gsDebugMsg("parsed multi action name set\n  %T\n", $$);
    }
  | LBRACE mult_act_names_cs RBRACE
    {
      $$ = ATreverse($2);
      gsDebugMsg("parsed multi action name set\n  %T\n", $$);
    }
  ;

//one or more multi action names, separated by comma's
mult_act_names_cs:
  mult_act_name
    {
      $$ = ATmakeList1((ATerm) $1);
      gsDebugMsg("parsed multi action names\n  %T\n", $$);
    }
  | mult_act_names_cs COMMA mult_act_name
    {
      $$ = ATinsert($1, (ATerm) $3);
      gsDebugMsg("parsed multi action names\n  %T\n", $$);
    }
  ;

%% 

ATermAppl gsSpecEltsToSpec(ATermList SpecElts)
{
  ATermAppl Result = NULL;
  ATermList SortDecls = ATmakeList0();
  ATermList ConsDecls = ATmakeList0();
  ATermList MapDecls = ATmakeList0();
  ATermList DataEqnDecls = ATmakeList0();
  ATermList ActDecls = ATmakeList0();
  ATermList ProcEqnDecls = ATmakeList0();
  ATermAppl Init = NULL;
  int n = ATgetLength(SpecElts);
  for (int i = 0; i < n; i++) {
    ATermAppl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsInit(SpecElt)) {
      if (Init == NULL) {
        Init = SpecElt;
      } else {
        //Init != NULL
        gsErrorMsg("parse error: multiple initialisations\n");
        return NULL;
      }
    } else {
      ATermList SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsSortSpec(SpecElt)) {
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
  if (Init == NULL) {
    gsErrorMsg("parse error: missing initialisation\n");
    return NULL;
  }
  Result = gsMakeSpecV1(
    gsMakeSortSpec(SortDecls),
    gsMakeConsSpec(ConsDecls),
    gsMakeMapSpec(MapDecls),
    gsMakeDataEqnSpec(DataEqnDecls),
    gsMakeActSpec(ActDecls),
    gsMakeProcEqnSpec(ProcEqnDecls),
    Init
  );
  return Result;
}
