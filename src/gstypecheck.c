#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
}
#endif

/**
@defgroup macros Standardized solution for declaring local variable-sized arrays.
@param type of the elements
@param name of the array variable
@param size of the array expressed in the number of elements
*/
#if defined(__cplusplus) || (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
#define DECLA(type, name, size) type name[size]
#else
#ifdef _MSC_VER
#define DECLA(type, name, size) type *name = (type*) ((size)>0?alloca((size)*sizeof(type)):NULL)
#else
#define DECLA(type, name, size) type *name = (type*) ((size)>0?alloca((size)*sizeof(type)):NULL)
#endif
#endif
/** @} */

#include "gstypecheck.h"
#include "gsfunc.h"

// Static data 
// the static context of the spec will be chacked and used, not transformed
typedef struct { 
  ATermIndexedSet basic_sorts;	
  ATermTable defined_sorts;	//name -> sort expression
  ATermTable constants;		//name -> Set(sort expression)
  ATermTable functions;		//name -> Set(sort expression)
  ATermTable actions;	        //name -> Set(List(sort expression)) because of action polymorphism
  //ATermIndexedSet typedactions;	//name#type
  ATermTable processes;	        //name -> Set(List(sort expression)) bacause of process polymorphism
  //ATermIndexedSet typedprocesses: use keys of body.proc_pars
} Context;
static Context context;

// the body may be transformed
typedef struct { 
  ATermList equations;	
  ATermTable proc_pars;	        //name#type -> List(Vars)
  ATermTable proc_bodies;	//name#type -> rhs 
  //ATermAppl init;             //in the hash tables proc_pars and proc_bodies with key "init#[]" (beware when writing)
} Body;
static Body body;

// Static function declarations
void gstcDataInit(void);
void gstcDataDestroy(void);
static ATbool gstcReadInSorts (ATermList);
static ATbool gstcReadInFuncs(ATermList);
static ATbool gstcReadInActs (ATermList);
static ATbool gstcReadInProcsAndInit (ATermList, ATermAppl);
static ATermList gstcWriteProcs(void);

static ATbool gstcCheckNamesData(void);
static ATbool gstcCheckNamesProc(void);
 
static ATbool gstcTransformActProc(void);
static ATbool gstcTransformVarConst(void);

static inline ATbool gstcInTypesA(ATermAppl, ATermList);
static ATbool gstcEqTypesA(ATermAppl, ATermAppl);
static inline ATbool gstcInTypesL(ATermList, ATermList);
static ATbool gstcEqTypesL(ATermList, ATermList);

static ATbool gstcCheckNamesP(ATermTable, ATermAppl);
static ATbool gstcCheckNamesD(ATermTable, ATermAppl);

static ATermTable gstcMakeVarsTable(ATermList);
static ATermAppl gstcRewrActProc(ATermAppl);
static ATermAppl gstcTraverseActProc(ATermAppl);

#define INIT_KEY gsMakeProcVarId(ATmakeAppl0(ATmakeAFun("init",0,ATtrue)),ATmakeList0())

// Main function
ATermAppl gsTypeCheck (ATermAppl input){	
  ATermAppl Result=NULL;
  gsVerboseMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking read-in phase started\n");
  if(!gstcReadInSorts(ATLgetArgument(ATAgetArgument(input,0),0))) {throw;}
  // Check soorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(!gstcReadInFuncs(ATconcat(ATLgetArgument(ATAgetArgument(input,1),0),
			       ATLgetArgument(ATAgetArgument(input,2),0)))) {throw;}
  body.equations=ATLgetArgument(ATAgetArgument(input,3),0);
  if(!gstcReadInActs(ATLgetArgument(ATAgetArgument(input,4),0))) {throw;}
  if(!gstcReadInProcsAndInit(ATLgetArgument(ATAgetArgument(input,5),0),ATAgetArgument(ATAgetArgument(input,6),0))) {throw;}
  gsDebugMsg ("type checking read-in phase finished\n");
  
  gsDebugMsg ("type checking transform ActProc+VarConst phase started\n");
  if(!gstcTransformActProc()){throw;}
  if(!gstcTransformVarConst()){throw;}
  gsDebugMsg ("type checking transform ActProc+VarConst phase finished\n");

  if(!gstcCheckNamesData()) {throw;} //names and # of arguments
  if(!gstcCheckNamesProc()) {throw;} //names and # of arguments

  Result=ATsetArgument(input,(ATerm)gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcEqnSpec(gstcWriteProcs()),5);
  Result=ATsetArgument(Result,(ATerm)gsMakeInit(ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY)),6);

  gsVerboseMsg ("type checking phase finished\n");
 finally:
  if (Result != NULL) {
    gsDebugMsg("return %t\n", Result);
  } 
  else {
    gsDebugMsg("return NULL\n");
  }
  gstcDataDestroy();
  return Result;
}

// ============ Static functions
// ========= main processing functions
void gstcDataInit(void){
  ATprotect((ATerm* )&context.basic_sorts);
  ATprotect((ATerm* )&context.defined_sorts);	
  ATprotect((ATerm* )&context.constants);
  ATprotect((ATerm* )&context.functions);
  ATprotect((ATerm* )&context.actions);	
  ATprotect((ATerm* )&context.processes);
  ATprotect((ATerm* )&body.equations);
  ATprotect((ATerm* )&body.proc_pars);
  ATprotect((ATerm* )&body.proc_bodies);

  context.basic_sorts=ATindexedSetCreate(63,50);
  context.defined_sorts=ATtableCreate(63,50);
  context.constants=ATtableCreate(63,50);
  context.functions=ATtableCreate(63,50);
  context.actions=ATtableCreate(63,50);
  context.processes=ATtableCreate(63,50);
  body.proc_pars=ATtableCreate(63,50);
  body.proc_bodies=ATtableCreate(63,50);
}

void gstcDataDestroy(void){
  ATindexedSetDestroy(context.basic_sorts);
  ATtableDestroy(context.defined_sorts);
  ATtableDestroy(context.constants);
  ATtableDestroy(context.functions);
  ATtableDestroy(context.actions);
  ATtableDestroy(context.processes);
  ATtableDestroy(body.proc_pars);
  ATtableDestroy(body.proc_bodies);
}
 
static ATbool gstcReadInSorts (ATermList Sorts){
  ATbool new;
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl Sort=ATAgetFirst(Sorts);
    ATermAppl SortName=ATAgetArgument(Sort,0);
    if(ATindexedSetGetIndex(context.basic_sorts, (ATerm)SortName)>=0 
       || ATAtableGet(context.defined_sorts, (ATerm)SortName)){
      ThrowMF("Double declaration of sort %t\n", SortName);
    }				
    if(gsIsSortId(Sort)) ATindexedSetPut(context.basic_sorts, (ATerm)SortName, &new);
    else
      if(gsIsSortRef(Sort)) ATtablePut(context.defined_sorts, (ATerm)SortName, (ATerm)ATAgetArgument(Sort,1));
      else assert(0);
  }
 finally:
  return Result;
}  

static ATbool gstcReadInFuncs(ATermList Funcs){
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);
    bool is_constant=!gsIsSortArrowProd(FuncType);
    
    if(ATAtableGet(context.constants, (ATerm)FuncName)){
      ThrowMF("Double declaration of constant %t\n", FuncName);
    }
    ATermList Types=ATLtableGet(context.functions, (ATerm)FuncName);
    if(is_constant && Types){
      ThrowMF("Double declaration of constant %t\n", FuncName);
    }
    if(is_constant){
      ATtablePut(context.constants, (ATerm)FuncName, (ATerm)FuncType);
    }
    else{
      // the table context.functions contains a list of types for each
      // function name. We need to check if there is already such a type 
      // in the list. If so -- error, otherwise -- add
      if (Types && gstcInTypesA(FuncType, Types)){
	ThrowMF("Double declaration of constant %t\n", FuncName);
      }
      else{
	if (!Types) Types=ATmakeList0();
	Types=ATinsert(Types,(ATerm)FuncType);
	ATtablePut(context.functions,(ATerm)FuncName,(ATerm)Types);
      }
    }	
  }
 finally:
  return Result;
} 

static ATbool gstcReadInActs (ATermList Acts){
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Acts);Acts=ATgetNext(Acts)){
    ATermAppl Act=ATAgetFirst(Acts);
    ATermAppl ActName=ATAgetArgument(Act,0);
    ATermList ActType=ATLgetArgument(Act,1);
    
    ATermList Types=ATLtableGet(context.actions, (ATerm)ActName);
    if(!Types){
      Types=ATmakeList1((ATerm)ActType);
    }
    else{
      // the table context.actions contains a list of types for each
      // action name. We need to check if there is already such a type 
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ActType, Types)){
	ThrowMF("Double declaration of action %t\n", ActName);
      }
      else{
	Types=ATinsert(Types,(ATerm)ActType);
      }
    }
    ATtablePut(context.actions,(ATerm)ActName,(ATerm)Types);
  }
 finally:
  return Result;
}

static ATbool gstcReadInProcsAndInit (ATermList Procs, ATermAppl Init){
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Procs);Procs=ATgetNext(Procs)){
    ATermAppl Proc=ATAgetFirst(Procs);
    ATermAppl ProcName=ATAgetArgument(ATAgetArgument(Proc,0),0);
    
    if(ATLtableGet(context.actions, (ATerm)ProcName)){
      ThrowMF("Declaration of both process and action %t\n", ProcName);
    }	

    ATermList ProcType=ATLgetArgument(ATAgetArgument(Proc,0),1);
    ATermList Types=ATLtableGet(context.processes,(ATerm)ProcName);
    if(!Types){
      Types=ATmakeList1((ATerm)ProcType);
    }
    else{
      // the table context.processes contains a list of types for each
      // process name. We need to check if there is already such a type 
      // in the list. If so -- error, otherwise -- add
      if (gstcInTypesL(ProcType, Types)){
	ThrowMF("Double declaration of process %t\n", ProcName);
      }
      else{
	Types=ATinsert(Types,(ATerm)ProcType);
      }
    }
    ATtablePut(context.processes,(ATerm)ProcName,(ATerm)Types);
    ATtablePut(body.proc_pars,(ATerm)ATAgetArgument(Proc,0),(ATerm)ATLgetArgument(Proc,1));
    ATtablePut(body.proc_bodies,(ATerm)ATAgetArgument(Proc,0),(ATerm)ATAgetArgument(Proc,2));
  }
  ATtablePut(body.proc_pars,(ATerm)INIT_KEY,(ATerm)ATmakeList0());
  ATtablePut(body.proc_bodies,(ATerm)INIT_KEY,(ATerm)Init);
 finally:
  return Result;
} 

static ATermList gstcWriteProcs(void){
  ATermList Result=ATmakeList0();
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    if(ProcVar==INIT_KEY) continue;
    Result=ATinsert(Result,(ATerm)gsMakeProcEqn(ProcVar,
						ATLtableGet(body.proc_pars,(ATerm)ProcVar),
						ATAtableGet(body.proc_bodies,(ATerm)ProcVar)
						)
		    );
  }
  return Result;
}

static ATbool gstcCheckNamesData(void){
  ATbool Result=ATtrue;
  for(ATermList Eqns=body.equations;!ATisEmpty(Eqns);Eqns=ATgetNext(Eqns)){
    ATermAppl Eqn=ATAgetFirst(Eqns);
    ATermTable Vars=gstcMakeVarsTable(ATLgetArgument(Eqn,0));
    if(!Vars){ThrowF;}
    ATermAppl Cond=ATAgetArgument(Eqn,1);
    if(!gsIsNil(Cond) && !gstcCheckNamesD(Vars,Cond)){ThrowF;}
    if(!gstcCheckNamesD(Vars,ATAgetArgument(Eqn,2))){ThrowF;}
    if(!gstcCheckNamesD(Vars,ATAgetArgument(Eqn,3))){ThrowF;}
    ATtableDestroy(Vars);
  }
 finally:
  return Result;
}

static ATbool gstcCheckNamesProc(void){
  ATbool Result=ATtrue;
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    ATermTable Vars=gstcMakeVarsTable(ATLtableGet(body.proc_pars,(ATerm)ProcVar));
    if(!Vars){ThrowF;}
    if(!gstcCheckNamesP(Vars,ATAtableGet(body.proc_bodies,(ATerm)ProcVar))){ThrowF;}
    ATtableDestroy(Vars);
  } 
 finally:
  return Result;
}

static ATbool gstcTransformActProc(void){
  ATbool Result=ATtrue;
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    ATermAppl ProcTerm=ATAtableGet(body.proc_bodies,(ATerm)ProcVar);
    ProcTerm=gstcTraverseActProc(ProcTerm);
    if(!ProcTerm){ThrowF;}
    ATtablePut(body.proc_bodies,(ATerm)ProcVar,(ATerm)ProcTerm);
  }
 finally:
  return Result;
}

static ATbool gstcTransformVarConst(void){
  ATbool Result=ATtrue;
 finally:
  return Result;
}

// ======== Auxiliary functions
static inline ATbool gstcInTypesA(ATermAppl Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesA(ATAgetFirst(Types),Type)) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesA(ATermAppl Type1, ATermAppl Type2){
  return ATisEqual(Type1, Type2);
}

static inline ATbool gstcInTypesL(ATermList Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesL(ATLgetFirst(Types),Type)) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesL(ATermList Type1, ATermList Type2){
  return ATisEqual(Type1, Type2);
}

static ATbool gstcCheckNamesP(ATermTable Vars, ATermAppl ProcTerm){
  ATbool Result=ATtrue;
 finally:
  return Result;
}

static ATbool gstcCheckNamesD(ATermTable Vars, ATermAppl DataTerm){
  ATbool Result=ATtrue;
 finally:
  return Result;
}
    
ATermTable gstcMakeVarsTable(ATermList VarDecls){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);

  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls)){
    ATermAppl VarDecl=ATAgetFirst(VarDecls);
    ATermAppl VarName=ATAgetArgument(VarDecl,0);
    ATermAppl VarType=ATAgetArgument(VarDecl,1);
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    ATtablePut(Vars, (ATerm)VarName, (ATerm)VarType);
  } 
 finally:
  if (!Result) {
    ATtableDestroy(Vars);
    return NULL;
  }
  else
    return Vars;
}

static ATbool gstcMatchRewrActProc(ATermAppl ProcTerm){
  //gsWarningMsg("matching %t, %d\n",ProcTerm, gsIsActionProcess(ProcTerm)); 
  if(gsIsActionProcess(ProcTerm)) return ATtrue;
  else return ATfalse;
}

static ATermAppl gstcRewrActProc(ATermAppl ProcTerm){
  ATermAppl Result=NULL;
  ATermAppl Name=ATAgetArgument(ProcTerm,0);
  if(ATtableGet(context.actions,(ATerm)Name)){
    //gsWarningMsg("recognized action %t\n",Name); 
    Result=gsMakeAction(gsMakeActId(Name,ATmakeList1((ATerm)gsMakeUnknown())),ATLgetArgument(ProcTerm,1));
  }
  else  
    if(ATtableGet(context.processes,(ATerm)Name)){
      //gsWarningMsg("recognized process %t\n",Name); 
      Result=gsMakeProcess(gsMakeProcVarId(Name,ATmakeList1((ATerm)gsMakeUnknown())),ATLgetArgument(ProcTerm,1));
    }
    else{
 	ThrowM("Action or process %t not declared\n", Name);
    }
 finally:
  return Result;
}

ATermAppl gstcTraverseActProc(ATermAppl ProcTerm){
  // if maches -- apply
  if(gstcMatchRewrActProc(ProcTerm))
    return gstcRewrActProc(ProcTerm);
  
  // otherwise apply to args
  AFun ProcSymbol=ATgetAFun(ProcTerm);
  int n = ATgetArity(ProcSymbol);
  DECLA(ATerm,args,n);              // ATerm* args = alloca(n*sizeof(ATerm));
  for (int i=0;i<n;i++)
    args[i] = ATgetArgument(ProcTerm,i);
  
  // For conditions don't apply to the first parameter
  for (int i=gsIsCond(ProcTerm)?1:0;i<n;i++){
    if(ATgetType(args[i])==AT_APPL){
      args[i]=(ATerm)gstcTraverseActProc((ATermAppl)args[i]);
      if(!args[i]) return NULL;
    } 
  }
  return ATmakeApplArray(ProcSymbol,args);
}
