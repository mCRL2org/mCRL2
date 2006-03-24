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

//external declarations from mcrl2delexer.l
void mcrl2deyyerror(const char *s);
int mcrl2deyylex(void);
extern ATermAppl DETree;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 20000

%}

%union {
  ATermAppl appl;
  ATermList list;
}

//generate a GLR parser
%glr-parser

//set name prefix
%name-prefix="mcrl2deyy"

//start token
%start data_expr_start

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

%type <appl> sort_expr
%type <appl> sort_expr_arrow domain_no_arrow_elt sort_expr_struct
%type <appl> struct_constructor recogniser struct_projection sort_expr_primary
%type <appl> sort_constant sort_constructor 
%type <appl> data_expr_start data_expr data_expr_whr whr_decl
%type <appl> data_expr_quant data_expr_imp data_expr_imp_rhs data_expr_and
%type <appl> data_expr_and_rhs data_expr_eq data_expr_eq_rhs data_expr_rel
%type <appl> data_expr_cons data_expr_snoc data_expr_concat data_expr_add
%type <appl> data_expr_div data_expr_mult data_expr_prefix
%type <appl> data_expr_quant_prefix data_expr_postfix data_expr_primary
%type <appl> data_constant data_enumeration bag_enum_elt data_comprehension
%type <appl> data_var_decl

%type <list> ids_cs data_vars_decl data_vars_decls_cs
%type <list> domain_no_arrow domain_no_arrow_elts_hs struct_constructors_bs
%type <list> struct_projections_cs whr_decls_cs data_exprs_cs bag_enum_elts_cs

%%

//data expression start
data_expr_start:
  data_expr
    {
      DETree = $1;
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

%% 
