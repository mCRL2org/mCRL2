//--- BES

BesSpec: [bes] BesEqnSpec BesInit ;

BesEqnSpec: [eqndecls] 'pbes' BesEqnDecls ;

BesEqnDecls
  : [eq] BesEqnDecl 
  | [eqs] BesEqnDecl BesEqnDecls 
  ; 

BesEqnDecl: [eqdecl] FixedPointOperator BesVar '=' BesExpr ';' ;

BesExpr
  : [be_impl] BesTerm '=>' BesExpr
  | [be_term] BesTerm
  ;

BesTerm
  : [be_disj] BesFactor '||' BesTerm
  | [be_conj] BesFactor '&&' BesTerm
  | [be_fac] BesFactor
  ;
  
BesFactor
  : [be_true] 'true'
  | [be_false] 'false'
  | [be_var] BesVar
  | [be_neg] '!' BesFactor
  | [be_brk] '(' BesExpr ')'
  ;

BesInit: [init] 'init' BesVar ';' ;

FixedPointOperator
  : [fp_mu] 'mu'
  | [fp_nu] 'nu'
  ;
