//--- BES

BesSpec: [bes] BesEqnSpec BesInit ;

BesEqnSpec: [eqndecls] 'pbes' BesEqnDecls ;

BesEqnDecls
  : [eq] BesEqnDecl 
  | [eqs] BesEqnDecl BesEqnDecls 
  ; 

BesEqnDecl: [eqdecl] FixedPointOperator BesVar '=' BesExpr ';' ;

BesExpr
  : [be_true] 'true'
  | [be_false] 'false'
  | [be_impl] BesExpr '=>' BesExpr
  | [be_disj] BesExpr '||' BesExpr
  | [be_conj] BesExpr '&&' BesExpr
  | [be_neg] '!' BesExpr
  | [be_brk] '(' BesExpr ')'
  | [be_var] BesVar
  ;

BesInit: [init] 'init' BesVar ';' ;

FixedPointOperator
  : [fp_mu] 'mu'
  | [fp_nu] 'nu'
  ;
