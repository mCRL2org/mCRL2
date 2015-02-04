SimpleSortExpr
  : [se_bool] 'Bool'       
//  | [se_pos] 'Pos'       
  | [se_nat] 'Nat'    
  | [se_int] 'Int'              
  | [se_real] 'Real'            
  | [se_ref] Id  
 // | [se_list] 'List' '(' SortExpr ')'                                   
 // | [se_set] 'Set' '(' SortExpr ')'
 // | [se_struct] 'struct' ConstrDeclList  // structured sort 
  ;
  
SortExpr : [se] SimpleSortExpr ;

IdList: [idlid] Id 
      | [idlids] Id ',' IdList 
	  ;
      
ConstrDeclList: [constdecl] Id 
              | [constdecllist] Id '|' ConstrDeclList
              ;

SortSpec: [sortspec] 'sort' SortDecls;

SortDecls: [sortdecls1] SortDecl 
         | [sortdecls2] SortDecl SortDecls
         ;

SortDecl
  : [sortdecl1] IdList ';\n'
  | [sortdecl2] Id '=' SortExpr ';\n'
  ;

VarDecl: [vdecl] IdList ':' SortExpr ;

VarsDeclList
  : [vsdecl] VarDecl
  | [vsdecls] VarDecl ',' VarsDeclList 
  ; 

Action: [act] Id ;

ActDecls: [actdecl] IdList ';\n' 
      //  | [actspeclist] IdList ActDecls 
        ;
        
ActSpec: [actspec] '\nact' ActDecls ;


//--- mCRL2 specification

//mCRL2Spec: mCRL2SpecElt+ Init mCRL2SpecElt* ;

mCRL2Spec: [mcrl2] SortSpec ActSpec ProcSpec Init;

//mCRL2SpecElt
//  : ( SortSpec
//	| ConsSpec
//	| MapSpec
//	| EqnSpec
//	| GlobVarSpec
//	| ActSpec
//	| ProcSpec 
//  ) ProcSpec
//  ;

ProcDecl: [procdecl] Id '=' ProcExpr ';\n'
        | [procdecllist] Id '(' VarsDeclList ')' '=' ProcExpr ';\n'
        ;
        
ProcSpec: [procspec] 'proc' ProcDecl
        | [procspecs] 'proc' ProcDecl ProcSpec 
        ;

Init: [init] '\ninit' ProcExpr ';' ;

ProcVarInst
  : [pcinst] Id
  | [pcinstlist] Id '(' DataExprListInst ')'
  ;

ProcExpr
  : [pre_act] Action
  | [pre_pv] ProcVarInst
  | [pre_delta] 'delta'
  | [pre_tau] 'tau'
 // | 'block' '(' ActIdSet ',' ProcExpr ')'
 // | 'allow' '(' MultActIdSet ',' ProcExpr ')'
 // | 'hide' '(' ActIdSet ',' ProcExpr ')'
 // | 'rename' '(' RenExprSet ',' ProcExpr ')'
 // | 'comm' '(' CommExprSet ',' ProcExpr ')'
  | [pre_brk] '(' ProcExpr ')'
  | [pre_choice] ProcExpr '+' ProcExpr
  | [pre_sum] 'sum' VarsDeclList '.' ProcExpr
  | [pre_pllel] ProcExpr '||' ProcExpr
//  | [pre_lftmrg] ProcExpr '||_' ProcExpr
  | [pre_cond] DataExpr '->' ProcExpr
//  | (DataExprUnit IfThen $unary_op_right 5) ProcExpr
//  | ProcExpr ('<<' $binary_op_left 6) ProcExpr
  | [pre_seq] ProcExpr '.' ProcExpr
//  | [pre_time] ProcExpr '@' DataExpr
//  | [pre_multact] ProcExpr '|' ProcExpr
  ;
  
//IfThen: '->' ProcExprNoIf '<>' $left 0 ;
