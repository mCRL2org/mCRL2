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
%token ID NUMBER

//non-terminals
//-------------


%type <appl> spec spec_elt sort_spec sort_decl struct_decl constr_decl
%type <appl> proj_decl domain op_spec op_decl eqn_spec eqn_var_decl eqn_decl
%type <appl> act_spec act_decl proc_spec proc_decl proc_decl_var_decl
%type <appl> initialisation sort_expr sort_expr_arrow sort_expr_primary
%type <appl> sort_constant sort_constructor data_expr data_expr_whr
%type <appl> data_expr_lambda id_decl data_expr_imp data_expr_and data_op_and
%type <appl> data_expr_eq data_op_eq data_expr_rel data_op_rel data_expr_snoc
%type <appl> data_expr_concat data_expr_add data_expr_mult data_expr_quant
%type <appl> data_op_quant data_expr_prefix data_expr_postfix data_expr_primary
%type <appl> data_constant data_enumeration bag_enum_elt data_comprehension
%type <appl> proc_expr proc_expr_choice proc_expr_merge proc_expr_binit
%type <appl> proc_expr_cond proc_expr_seq proc_expr_at proc_expr_sync
%type <appl> proc_expr_primary ma_ids_set ma_id ren_expr_set ren_expr
%type <appl> comm_expr_set comm_expr

%type <list> spec_elts sort_decls ids proj_decls op_decls id_decls
%type <list> eqn_var_decls eqn_decls act_decls proc_decls proc_decl_lhs
%type <list> proc_decl_var_decls sort_expr_arrow_lhs data_exprs data_expr_cons
%type <list> bag_enum_elts ma_ids ren_exprs comm_exprs
%%

//specification
spec:
  spec_elts
    {
      $$ = gsMakeSpec($1);
      if (gsDebug) {
        ATprintf( "parsed specification\n  %t\n", $$);
      }
    }
  ;

//specification elements
spec_elts:
  spec_elt
    {
      $$ = ATmakeList1((ATerm) $1);
      if (gsDebug) {
        ATprintf( "parsed specification elements\n  %t\n", $$);
      }
    }
   | spec_elts spec_elt
    {
      $$ = ATinsert($1, (ATerm) $2);
      if (gsDebug) {
        ATprintf( "parsed specification elements\n  %t\n", $$);
      }
    }
   ;

//specification element
spec_elt:
  sort_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    } 
  | op_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    }
  | eqn_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    }
  | act_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    }
  | proc_spec
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    }
  | initialisation
    {
      $$ = $1;
      if (gsDebug) {
        ATprintf( "parsed specification element\n  %t\n", $$);
      }
    }
  ;

//sort specification
sort_spec:
  SORT sort_decls
    {
      $$ = gsMakeSortSpec($2);
      if (gsDebug) {
        ATprintf( "parsed sort specification\n  %t\n", $$);
      }
    }
  ;

//sort declarations
sort_decls:
  sort_decl
  | sort_decls COMMA sort_decl
  ;

//sort declaration
sort_decl:
  ids SEMICOLON                                     //standard sort
  | ids EQUALS sort_expr SEMICOLON                  //sort expression
  | ID EQUALS STRUCT struct_decl SEMICOLON          //structured sort
  ;

//comma-separated list of identifiers
ids:
  ID
  | ids COMMA ID
  ;

//declaration of a structured sort
struct_decl:
  constr_decl
  | struct_decl BAR constr_decl
  ;

//constructor declaration
constr_decl:
  ID
  | ID LPAR proj_decls RPAR
  | ID LPAR proj_decls RPAR QMARK ID
  | ID QMARK ID
  ;

//comma-separated list of projection declarations
proj_decls:
  proj_decl
  | proj_decls COMMA proj_decl
  ;

//projection declaration
proj_decl:
  domain
  | ID COLON domain
  ;

//domain
domain:
  sort_expr_arrow_lhs
  | sort_expr_arrow_lhs ARROW sort_expr
  ;

//operation specification
op_spec:
  CONS op_decls
  | MAP op_decls
  ;

//operation declarations
op_decls:
  op_decl
  | op_decls op_decl
  ;

//operation declaration
op_decl:
  id_decls SEMICOLON
  ;

//declaration of 1 or more identifiers of the same sort
id_decls:
  ids COLON sort_expr
  ;

//equation specification
eqn_spec:
  EQN eqn_decls
  | VAR eqn_var_decls EQN eqn_decls
  ;

//variable declarations of an equation specification
eqn_var_decls:
  eqn_var_decl
  | eqn_var_decls eqn_var_decl
  ;

//variable declaration of an equation specification
eqn_var_decl:
  id_decls SEMICOLON
  ;

//equation declarations
eqn_decls:
  eqn_decl
  | eqn_decls eqn_decl
  ;

//equation declaration
eqn_decl:
  data_expr EQUALS data_expr SEMICOLON
  ;

//action specification
act_spec:
  ACT act_decls
  ;

//action declarations
act_decls:
  act_decl
  | act_decls act_decl
  ;

//action declaration
act_decl:
  ids SEMICOLON
  | ids COLON domain SEMICOLON
  ;

//process specification
proc_spec:
  PROC proc_decls
  ;

//process declarations
proc_decls:
  proc_decl
  | proc_decls proc_decl
  ;

//process declaration
proc_decl:
  proc_decl_lhs EQUALS proc_expr SEMICOLON
  ;

//lhs of a process declaration
proc_decl_lhs:
  ID
  | ID LPAR proc_decl_var_decls RPAR
  ;

//variable declarations of a process declaration
proc_decl_var_decls:
  proc_decl_var_decl
  | proc_decl_var_decls COMMA proc_decl_var_decl
  ;

//variable declaration of a process declaration
proc_decl_var_decl:
  id_decls
  ;

//initialisation
initialisation:
  INIT proc_expr SEMICOLON
  ;

//sort expression
sort_expr:
  sort_expr_arrow
  ;

//arrow sort expression
sort_expr_arrow:
  sort_expr_primary
  | sort_expr_arrow_lhs ARROW sort_expr_arrow
  ;

//lhs of an arrow expression
sort_expr_arrow_lhs:
  sort_expr_primary
  | sort_expr_arrow_lhs HASH sort_expr_primary
  ;

//primary sort expression
sort_expr_primary:
  ID
  | sort_constant
  | sort_constructor LPAR sort_expr RPAR
  | LPAR sort_expr RPAR
  ;

//sort constant
sort_constant:
  BOOL
  | POS
  | NAT
  | INT
  ;

//sort constructor
sort_constructor:
  LIST
  | SET
  | BAG
  ;

//data expression
data_expr:
  data_expr_whr
  ;

//where clause
data_expr_whr:
  data_expr_lambda
  | data_expr_whr WHR data_exprs END
  ;

//comma-separated list of data expressions
data_exprs:
  data_expr
  | data_exprs COMMA data_expr
  ;

//lambda abstraction
data_expr_lambda:
  data_expr_imp
  | LAMBDA id_decl DOT data_expr_lambda
  ;

//identifier declaration
id_decl:
  ID COLON sort_expr
  ;

//implication
data_expr_imp:
  data_expr_and
  | data_expr_and IMP data_expr_imp
  ;

//conjunction and disjunction
data_expr_and:
  data_expr_eq
  | data_expr_and data_op_and data_expr_eq
  ;

//operators for conjunction and disjunction
data_op_and:
  AND
  | BARS 
  ;

//equality
data_expr_eq:
  data_expr_rel
  | data_expr_eq data_op_eq data_expr_rel
  ;

//operators for equality
data_op_eq:
  EQ
  | NEQ
  ;

//relations
data_expr_rel:
  data_expr_cons
  | data_expr_cons data_op_rel data_expr_cons
  ;

//relational operators
data_op_rel:
  GTE                      //greater than or equal
  | LTE                    //lower than or equal
  | RANG                   //greater
  | LANG                   //lower
  | IN
  ;

//cons
data_expr_cons:
  data_expr_snoc
  | data_expr_add CONS data_expr_cons
  ;

//snoc
data_expr_snoc:
  data_expr_concat
  | data_expr_snoc SNOC data_expr_add
  ; 

//concatenation
data_expr_concat:
  data_expr_add
  | data_expr_concat CONCAT data_expr_add
  ;

//addition and subtraction
data_expr_add:
  data_expr_mult
  | data_expr_add PLUS data_expr_mult
  | data_expr_add MINUS data_expr_mult
  ;

//multiplication and division
data_expr_mult:
  data_expr_quant
  | data_expr_mult STAR data_expr_quant
  | data_expr_mult DIV data_expr_quant
  | data_expr_mult MOD data_expr_quant
  | data_expr_mult DOT data_expr_quant
  ;

//universal and existential quantification
data_expr_quant:
  data_expr_prefix
  | data_op_quant id_decl DOT data_expr_quant
  ;

//universal and existential quantification operators
data_op_quant:
  FORALL
  | EXISTS
  ;

//prefix data expression
data_expr_prefix:
  data_expr_postfix
  | EXCLAM data_expr_prefix                //logical negation, set complement
  | MINUS data_expr_prefix                 //arithmetic negation
  | HASH data_expr_prefix                  //list/set/bag size
  ;  

//postfix data expression
data_expr_postfix:
  data_expr_primary
  | data_expr_postfix LPAR data_exprs RPAR //function application
  ;
  
//primary data expression
data_expr_primary:
  ID                                       //identifier
  | data_constant                          //constant
  | data_enumeration                       //enumeration
  | data_comprehension                     //comprehension
  | LPAR data_expr RPAR                    //parenthesized data expression
  ;

//constant
data_constant:
  TRUE                                     //true
  | FALSE                                  //false
  | NUMBER                                 //number
  | PBRACK                                 //empty list
  | PBRACE                                 //empty set/bag
  ;

//enumeration
data_enumeration:
  LBRACK data_exprs RBRACK                 //list enumeration
  | LBRACE data_exprs RBRACE               //set enumeration
  | LBRACE bag_enum_elts RBRACE            //bag enumeration 
  ;

//comma-separated list of bag enumeration elements
bag_enum_elts:
  bag_enum_elt
  | bag_enum_elts COMMA bag_enum_elt
  ;

//bag enumeration element
bag_enum_elt:
  data_expr COLON data_expr
  ;

//comprehension
data_comprehension:
  LBRACE id_decl BAR data_expr RBRACE      //set/bag comprehension
  ;

//process expression
proc_expr:
  proc_expr_choice
  ;

//choice (associative)
proc_expr_choice:
  proc_expr_merge
  | proc_expr_choice PLUS proc_expr_merge
  ;

//merge (associative) and left merge (left associative)
proc_expr_merge:
  proc_expr_binit
  | proc_expr_merge BARS proc_expr_binit   //merge
  | proc_expr_merge LMERGE proc_expr_binit //left merge
  ;

//bounded initialisation (left associative)
proc_expr_binit:
  proc_expr_cond
  | proc_expr_binit BINIT proc_expr_cond
  ;

//conditional
proc_expr_cond:
  proc_expr_seq
  | data_expr_prefix ARROW proc_expr_seq
  ;

//sequential (associative)
proc_expr_seq:
  proc_expr_at
  | proc_expr_seq DOT proc_expr_at
  ;

//timed expression
proc_expr_at:
  proc_expr_sync
  | proc_expr_at AT data_expr_prefix
  ;

//synchronisation (associative)
proc_expr_sync:
  proc_expr_primary
  | proc_expr_sync BAR proc_expr_primary
  ;

//primary process expression
proc_expr_primary:
  DELTA                                            //deadlock
  | TAU                                            //internal action
  | ID                                             //action or process reference with 0...
  | ID LPAR data_exprs RPAR                        //and 1 or more arguments
  | SUM LPAR id_decl COMMA proc_expr RPAR          //summation
  | RESTRICT LPAR ma_ids_set COMMA proc_expr RPAR  //restriction AKA encapsulation
  | ALLOW LPAR ma_ids_set COMMA proc_expr RPAR     //allow AKA nabla 
  | HIDE LPAR ma_ids_set COMMA proc_expr RPAR      //hiding
  | RENAME LPAR ren_expr_set COMMA proc_expr RPAR  //renaming
  | COMM LPAR comm_expr_set COMMA proc_expr RPAR   //communication
  | LPAR proc_expr RPAR                            //parenthesized process expression
  ;

//set of multiaction identifiers
ma_ids_set:
  LBRACE RBRACE
  | LBRACE ma_ids RBRACE
  ;

//multiaction identifiers
ma_ids:
  ma_id
  | ma_ids COMMA ma_id
  ;

//multiaction identifier
ma_id:
  ID
  | ma_id BAR ID
  ;

//set of renaming expressions
ren_expr_set:
  LBRACE RBRACE
  | LBRACE ren_exprs RBRACE
  ;

//renaming expressions
ren_exprs:
  ren_expr 
  | ren_exprs COMMA ren_expr
  ;

//renaming expression
ren_expr:
  ID ARROW ID
  ;

//set of communication expressions
comm_expr_set:
  LBRACE RBRACE
  | LBRACE comm_exprs RBRACE
  ;

//communication expressions
comm_exprs:
  comm_expr 
  | comm_exprs COMMA comm_expr
  ;

//communication expression
comm_expr:
  ma_id
  | ma_id ARROW ID
  ;

%% 
