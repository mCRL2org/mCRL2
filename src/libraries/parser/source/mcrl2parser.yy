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
extern ATermAppl spec_tree;
extern ATermIndexedSet parser_protect_table;

#ifdef _MSC_VER
#define yyfalse 0
#define yytrue 1
#endif

#define YYMAXDEPTH 160000

//local declarations
ATermAppl gsSpecEltsToSpec(ATermList SpecElts);
//Pre: SpecElts contains one initialisation and zero or more occurrences of
//     sort, constructor, operation, equation, action and process
//     specifications.
//Ret: specification containing one sort, constructor, operation, equation,
//     action and process specification, and one initialisation, in that order.

#define safe_assign(lhs, rhs) { ATbool b; ATindexedSetPut(parser_protect_table, (ATerm) rhs, &b); lhs = rhs; }
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
%start start

//terminals
//---------

%token <appl> TAG_SORT_EXPR TAG_DATA_EXPR TAG_MULT_ACT TAG_PROC_EXPR TAG_SPEC
%token <appl> TAG_STATE_FRM
%token <appl> LMERGE ARROW LTE GTE CONS SNOC CONCAT EQ NEQ AND BARS IMP BINIT
%token <appl> ELSE
%token <appl> STAR PLUS MINUS EQUALS DOT COMMA COLON SEMICOLON QMARK EXCLAM AT
%token <appl> HASH BAR
%token <appl> LPAR RPAR PBRACK LBRACK RBRACK LANG RANG PBRACE LBRACE RBRACE
%token <appl> KWSORT KWCONS KWMAP KWVAR KWEQN KWACT KWPROC KWINIT KWSTRUCT
%token <appl> BOOL POS NAT INT REAL LIST SET BAG
%token <appl> CTRUE CFALSE DIV MOD IN LAMBDA FORALL EXISTS WHR END 
%token <appl> DELTA TAU SUM BLOCK ALLOW HIDE RENAME COMM
%token <appl> VAL MU NU DELAY YALED NIL
%token <appl> ID NUMBER

//non-terminals
//-------------

//start
%type <appl> start
//sort expressions
%type <appl> sort_expr sort_expr_arrow domain_no_arrow_elt sort_expr_struct
%type <appl> struct_constructor recogniser struct_projection sort_expr_primary
%type <appl> sort_constant sort_constructor
//data expressions
%type <appl> data_expr data_expr_whr whr_decl data_expr_quant data_expr_imp
%type <appl> data_expr_imp_rhs data_expr_and data_expr_and_rhs data_expr_eq
%type <appl> data_expr_eq_rhs data_expr_rel data_expr_cons data_expr_snoc
%type <appl> data_expr_concat data_expr_add data_expr_div data_expr_mult
%type <appl> data_expr_prefix data_expr_quant_prefix data_expr_postfix
%type <appl> data_expr_primary data_constant data_enumeration
%type <appl> bag_enum_elt data_comprehension data_var_decl
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
%type <appl> proc_quant mult_act_name ren_expr comm_expr
//specifications
%type <appl> spec spec_elt sort_spec cons_spec map_spec eqn_spec
%type <appl> eqn_decl act_spec proc_spec proc_decl initialisation
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

//sort expressions
%type <list> domain_no_arrow domain_no_arrow_elts_hs struct_constructors_bs
%type <list> struct_projections_cs ids_cs 
//data expressions
%type <list> whr_decls_cs data_exprs_cs bag_enum_elts_cs
//process expressions
%type <list> act_names_set ren_expr_set ren_exprs_cs comm_expr_set comm_exprs_cs
%type <list> mult_act_names_set mult_act_names_cs ids_bs
//multi-actions
%type <list> param_ids_bs
//specifications
%type <list> spec_elts sorts_decls_scs sorts_decl domain ops_decls_scs
%type <list> ops_decl eqn_sect eqn_decls_scs data_vars_decls_scs data_vars_decl
%type <list> acts_decls_scs acts_decl proc_decls_scs data_vars_decls_cs
//state formulas
%type <list> fixpoint_params data_var_decl_inits_cs

%%


//Start
//-----

//start
start:
  TAG_SORT_EXPR sort_expr
    {
      safe_assign($$, $2);
      spec_tree = $$;
    }
  | TAG_DATA_EXPR data_expr
    {
      safe_assign($$, $2);
      spec_tree = $$;
    }
  | TAG_MULT_ACT mult_act
    {
      safe_assign($$, $2);
      spec_tree = $$;
    }
  | TAG_PROC_EXPR proc_expr
    {
      safe_assign($$, $2);
      spec_tree = $$;
    }
  | TAG_SPEC spec
    {
      safe_assign($$, $2);
      spec_tree = $$;
    }
  | TAG_STATE_FRM state_frm
    {
      safe_assign($$, $2);
      spec_tree = $$;
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
      safe_assign($$, gsMakeSortArrowProd($1, $3));
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
      safe_assign($$, gsMakeSortExprList($3));
    }
  | SET LPAR sort_expr RPAR
    {
      safe_assign($$, gsMakeSortExprSet($3));
    }
  | BAG LPAR sort_expr RPAR
    {
      safe_assign($$, gsMakeSortExprBag($3));
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
  | data_expr_whr WHR whr_decls_cs END
    {
      safe_assign($$, gsMakeWhr($1, ATreverse($3)));
      gsDebugMsg("parsed where clause\n  %T\n", $$);
    }
  ;

//declaration of one or more where clauses, separated by comma's
whr_decls_cs:
  whr_decl
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed where clause declarations\n  %T\n", $$);
    }
  | whr_decls_cs COMMA whr_decl
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed where clause declarations\n  %T\n", $$);
    }
  ;

//where clause declaration
whr_decl:
  ID EQUALS data_expr
    {
      safe_assign($$, gsMakeWhrDecl($1, $3));
      gsDebugMsg("parsed where clause declaration\n  %T\n", $$);
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

//implication (right associative)
data_expr_imp:
  data_expr_and
    {
      safe_assign($$, $1);
    }
  | data_expr_and IMP data_expr_imp_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed conjunction\n  %T\n", $$);
    }
  | data_expr_eq BARS data_expr_and_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
    }
  | data_expr_rel NEQ data_expr_eq_rhs
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed equality expression\n  %T\n", $$);
    }
  ;

//right argument of equality 
data_expr_eq_rhs:
  data_expr_eq
    {
      safe_assign($$, $1);
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LTE data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons RANG data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons LANG data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed relational expression\n  %T\n", $$);
    }
  | data_expr_cons IN data_expr_cons
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed addition or set union\n  %T\n", $$);
    }
  | data_expr_add MINUS data_expr_div
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed div expression\n  %T\n", $$);
    }
  | data_expr_div MOD data_expr_mult
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed mod expression\n  %T\n", $$);
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
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
      gsDebugMsg("parsed multiplication or set intersection\n  %T\n", $$);
    }
  | data_expr_mult DOT data_expr_prefix
    {
      $$ = gsMakeDataApplProd(gsMakeId($2),
        ATmakeList2((ATerm) $1, (ATerm) $3));
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
      safe_assign($$, gsMakeDataApplProd(gsMakeId($1), ATmakeList1((ATerm) $2)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | MINUS data_expr_prefix
    {
      safe_assign($$, gsMakeDataApplProd(gsMakeId($1), ATmakeList1((ATerm) $2)));
      gsDebugMsg("parsed prefix data expression\n  %T\n", $$);
    }
  | HASH data_expr_prefix
    {
      safe_assign($$, gsMakeDataApplProd(gsMakeId($1), ATmakeList1((ATerm) $2)));
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
      safe_assign($$, gsMakeDataApplProd($1, ATreverse($3)));
      gsDebugMsg("parsed postfix data expression\n  %T\n", $$);
    }
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
  | NUMBER
    {
      safe_assign($$, gsMakeNumber($1, gsMakeUnknown()));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | PBRACK
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  | PBRACE
    {
      safe_assign($$, gsMakeId($1));
      gsDebugMsg("parsed data constant\n  %T\n", $$);
    }
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs_cs RBRACK
    {
      safe_assign($$, gsMakeListEnum(ATreverse($2), gsMakeUnknown()));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE data_exprs_cs RBRACE
    {
      safe_assign($$, gsMakeSetEnum(ATreverse($2), gsMakeUnknown()));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  | LBRACE bag_enum_elts_cs RBRACE
    {
      safe_assign($$, gsMakeBagEnum(ATreverse($2), gsMakeUnknown()));
      gsDebugMsg("parsed data enumeration\n  %T\n", $$);
    }
  ;

//one or more bag enumeration elements, separated by comma's
bag_enum_elts_cs:
  bag_enum_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  | bag_enum_elts_cs COMMA bag_enum_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $3));
      gsDebugMsg("parsed bag enumeration elements\n  %T\n", $$);
    }
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
    {
      safe_assign($$, gsMakeBagEnumElt($1, $3));
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
  PBRACE
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

//set of renaming expressions
ren_expr_set:
  PBRACE
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
  PBRACE
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
  mult_act_name
    {
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | mult_act_name ARROW TAU
    {      
      safe_assign($$, gsMakeCommExpr($1, gsMakeNil()));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
    }
  | mult_act_name ARROW ID
    {      
      safe_assign($$, gsMakeCommExpr($1, $3));
      gsDebugMsg("parsed communication expression\n  %T\n", $$);
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
  PBRACE
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

//Specifications
//--------------

//specification
spec:
  spec_elts
    {
      safe_assign($$, gsSpecEltsToSpec(ATreverse($1)));
      gsDebugMsg("parsed specification\n  %T\n", $$);
    }
  ;

//specification elements
spec_elts:
  spec_elt
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed specification elements\n  %T\n", $$);
    }
   | spec_elts spec_elt
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed specification elements\n  %T\n", $$);
    }
   ;

//specification element
spec_elt:
  sort_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    } 
  | cons_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | map_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | eqn_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | act_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | proc_spec
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
    }
  | initialisation
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed specification element\n  %T\n", $$);
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

//domain
domain:
  domain_no_arrow
    {
      safe_assign($$, $1);
      gsDebugMsg("parsed domain\n  %T\n", $$);
    }
  | domain_no_arrow ARROW sort_expr
    {
      safe_assign($$, ATmakeList1((ATerm) gsMakeSortArrowProd($1, $3)));
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

//equation specification
eqn_spec:
  eqn_sect    
    {
      safe_assign($$, gsMakeDataEqnSpec($1));
      gsDebugMsg("parsed equation specification\n  %T\n", $$);
    }
  ;

//equation section
eqn_sect:
  KWEQN eqn_decls_scs
    {
      safe_assign($$, ATreverse($2));
      gsDebugMsg("parsed equation section\n  %T\n", $$);
    }
  | KWVAR data_vars_decls_scs KWEQN eqn_decls_scs
    {
      safe_assign($$, ATmakeList0());
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
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed equation declarations\n  %T\n", $$);
    }
  | eqn_decls_scs eqn_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
      gsDebugMsg("parsed equation declarations\n  %T\n", $$);
    }
  ;

//equation declaration
eqn_decl:
  data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), gsMakeNil(), $1, $3));
      gsDebugMsg("parsed equation declaration\n  %T\n", $$);
    }
  | data_expr ARROW data_expr EQUALS data_expr
    {
      safe_assign($$, gsMakeDataEqn(ATmakeList0(), $1, $3, $5));
      gsDebugMsg("parsed equation declaration\n  %T\n", $$);
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
        $$ = ATinsert($$,
          (ATerm) gsMakeActId(ATAelementAt($1, i), ATmakeList0()));
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

//process specification
proc_spec:
  KWPROC proc_decls_scs
    {
      safe_assign($$, gsMakeProcEqnSpec(ATreverse($2)));
      gsDebugMsg("parsed process specification\n  %T\n", $$);
    }
  ;

//one or more process declarations, separated by semicolons
proc_decls_scs:
  proc_decl SEMICOLON
    {
      safe_assign($$, ATmakeList1((ATerm) $1));
      gsDebugMsg("parsed process declarations\n  %T\n", $$);
    }
  | proc_decls_scs proc_decl SEMICOLON
    {
      safe_assign($$, ATinsert($1, (ATerm) $2));
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
      safe_assign($$, $1);
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  | data_vars_decls_cs COMMA data_vars_decl
    {
      safe_assign($$, ATconcat($1, $3));
      gsDebugMsg("parsed data variable declarations\n  %T\n", $$);
    }
  ;

//initialisation
initialisation:
  KWINIT proc_expr SEMICOLON
    {
      safe_assign($$, gsMakeInit(ATmakeList0(), $2));
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
      safe_assign($$, gsMakeDataVarIdInit($1, $3, $5));
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
      gsDebugMsg("parsed postfix state formula\n  %T\n", $$);
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
  | LPAR reg_frm RPAR
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
    gsMakeDataSpec(
      gsMakeSortSpec(SortDecls),
      gsMakeConsSpec(ConsDecls),
      gsMakeMapSpec(MapDecls),
      gsMakeDataEqnSpec(DataEqnDecls)
    ),
    gsMakeActSpec(ActDecls),
    gsMakeProcEqnSpec(ProcEqnDecls),
    Init
  );
  return Result;
}
