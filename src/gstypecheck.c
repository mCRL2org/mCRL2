#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

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
#include "gslowlevel.h"

#define ThrowF            {ThrowV(ATfalse);}
//store ATfalse in result and throw an exception

#define ThrowMF(...)      {ThrowVM(ATfalse, __VA_ARGS__);}
//print error message supplied by the first parameter with the remaining
//store ATfalse in result and throw an exception


// Static data 
// system constants and functions 
typedef struct { 
  ATermTable constants;		//name -> Set(sort expression)
  ATermTable functions;		//name -> Set(sort expression)
} gsSystem;
static gsSystem gssystem;

// the static context of the spec will be checked and used, not transformed
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
static void gstcDataInit(void);
static void gstcDataDestroy(void);
static ATbool gstcReadInSorts (ATermList);
static ATbool gstcReadInConstructors();
static ATbool gstcReadInFuncs(ATermList);
static ATbool gstcReadInActs (ATermList);
static ATbool gstcReadInProcsAndInit (ATermList, ATermAppl);

static ATbool gstcTransformVarConsTypeData(void);
static ATbool gstcTransformActProcVarConst(void);

static ATbool gstcInferTypesData(void);
static ATbool gstcInferTypesProc(void);

static ATermList gstcWriteProcs(void);

static ATbool gstcInTypesA(ATermAppl, ATermList);
static ATbool gstcEqTypesA(ATermAppl, ATermAppl);
static ATbool gstcInTypesL(ATermList, ATermList);
static ATbool gstcEqTypesL(ATermList, ATermList);

static ATbool gstcReadInSortStruct(ATermAppl);
static ATbool gstcAddConstant(ATermAppl, ATermAppl, const char*);
static ATbool gstcAddFunction(ATermAppl, ATermAppl, const char*);
static void gstcAddSystemConstant(ATermAppl);
static void gstcAddSystemFunctionProd(ATermAppl, ATermAppl);

static ATermTable gstcAddVars2Table(ATermTable,ATermList);
static ATermAppl gstcRewrActProc(ATermTable, ATermAppl);
static inline ATermAppl gstcMakeActionOrProc(ATbool, ATermAppl, ATermList, ATermList);
static ATermAppl gstcTraverseActProcVarConstP(ATermTable, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeD(ATermTable, ATermAppl *, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeDN(int, ATermTable, ATermAppl* , ATermAppl);
static ATermList gstcTraverseVarConstL(ATermTable, ATermList);

static ATbool gstcInferTypesP(ATermTable, ATermAppl);
static ATbool gstcInferTypesD(ATermTable, ATermAppl);

static ATermList gstcGetNotInferredList(ATermList TypeListList);
static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type);

static inline ATbool gstcIsPos(ATermAppl Number) {char c=ATgetName(ATgetAFun(Number))[0]; return (isdigit(c) && c>'0');}
static inline ATbool gstcIsNat(ATermAppl Number) {return isdigit(ATgetName(ATgetAFun(Number))[0]);}

static inline ATermAppl gstcMakeSortArrowProd1(ATermAppl Source, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList1((ATerm)Source),Target);
}

static inline ATermAppl gstcMakeSortArrowProd2(ATermAppl Source1, ATermAppl Source2, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList2((ATerm)Source1,(ATerm)Source2),Target);
}

static inline ATermAppl gstcMakeSortArrowProd3(ATermAppl Source1, ATermAppl Source2,ATermAppl Source3, ATermAppl Target){
  return gsMakeSortArrowProd(ATmakeList3((ATerm)Source1,(ATerm)Source2,(ATerm)Source3),Target);
}

static inline ATermAppl gstcMakeOpIdPos2Nat(void){
  return ATsetArgument(gsMakeOpIdPos2Nat(),
		       (ATerm)gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()),
		       1);
}

static inline ATermAppl gstcMakeOpIdNat2Int(void){
  return ATsetArgument(gsMakeOpIdNat2Int(),
		       (ATerm)gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()),
		       1);
}

static inline ATermAppl gstcMakeOpIdSet2Bag(ATermAppl Type){
  return ATsetArgument(gsMakeOpIdNameSet2Bag(Type,Type),
		       (ATerm)gstcMakeSortArrowProd1(gsMakeSortSet(Type),gsMakeSortBag(Type)),
		       1);
}

static inline ATermAppl INIT_KEY(void){return gsMakeProcVarId(ATmakeAppl0(ATmakeAFun("init",0,ATtrue)),ATmakeList0());}
static inline ATermAppl gstcMakeNotInferred(ATermList PossibleTypes){
  assert(PossibleTypes);
  return ATmakeAppl1(ATmakeAFun("notInferred",1,ATtrue),(ATerm)PossibleTypes);
}
static inline bool gstcIsNotInferred(ATermAppl SortTerm){
  return (ATgetAFun(SortTerm)==ATmakeAFun("notInferred",1,ATtrue));
}
static ATermList gstcGetNotInferredList(ATermList TypeListList);
static ATermList gstcAdjustNotInferredList(ATermList TypeList, ATermList TypeListList);
static ATbool gstcIsNotInferredL(ATermList TypeListList);
static ATbool gstcIsTypeAllowed(ATermList PosTypeList, ATermList TypeList);
static ATermAppl gstcUnwindType(ATermAppl Type);
static ATermAppl gstcUnSetBag(ATermAppl PosType);
static ATermAppl gstcUnDataAppl(ATermAppl PosType);
static ATermAppl gstcMakeNotInferredSetBag(ATermAppl Type);
static ATermAppl gstcAdjustPosTypesA(ATermAppl NewType, ATermAppl PosType);
static ATermList gstcTypesIntersect(ATermList TypeList1, ATermList TypeList2);

// Main function
ATermAppl gsTypeCheck (ATermAppl input){	
  ATermAppl Result=NULL;
  gsVerboseMsg ("type checking phase started\n");
  gstcDataInit();

  gsDebugMsg ("type checking read-in phase started\n");
  if(!gstcReadInSorts(ATLgetArgument(ATAgetArgument(input,0),0))) {throw;}
  // Check soorts for loops
  // Unwind sorts to enable equiv and subtype relations
  if(!gstcReadInConstructors()) {throw;}
  if(!gstcReadInFuncs(ATconcat(ATLgetArgument(ATAgetArgument(input,1),0),
			       ATLgetArgument(ATAgetArgument(input,2),0)))) {throw;}
  body.equations=ATLgetArgument(ATAgetArgument(input,3),0);
  if(!gstcReadInActs(ATLgetArgument(ATAgetArgument(input,4),0))) {throw;}
  if(!gstcReadInProcsAndInit(ATLgetArgument(ATAgetArgument(input,5),0),
			     ATAgetArgument(ATAgetArgument(input,6),0))) {throw;}
  gsDebugMsg ("type checking read-in phase finished\n");
  
  gsDebugMsg ("type checking transform ActProc+VarConst phase started\n");
  if(!gstcTransformVarConsTypeData()){throw;}
  if(!gstcTransformActProcVarConst()){throw;}
  gsDebugMsg ("type checking transform ActProc+VarConst phase finished\n");

  if(!gstcInferTypesData()) {throw;} //names and # of arguments
  if(!gstcInferTypesProc()) {throw;} //names and # of arguments

  Result=ATsetArgument(input,(ATerm)gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcEqnSpec(gstcWriteProcs()),5);
  Result=ATsetArgument(Result,(ATerm)gsMakeInit(ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY())),6);

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
  gssystem.constants=ATtableCreate(63,50);
  gssystem.functions=ATtableCreate(63,50);
  context.basic_sorts=ATindexedSetCreate(63,50);
  context.defined_sorts=ATtableCreate(63,50);
  context.constants=ATtableCreate(63,50);
  context.functions=ATtableCreate(63,50);
  context.actions=ATtableCreate(63,50);
  context.processes=ATtableCreate(63,50);
  body.proc_pars=ATtableCreate(63,50);
  body.proc_bodies=ATtableCreate(63,50);

  //Creation of operation identifiers for system defined operations.
  //Bool
  gstcAddSystemConstant(gsMakeOpIdTrue());
  gstcAddSystemConstant(gsMakeOpIdFalse());
  gstcAddSystemFunctionProd(gsMakeOpIdNot(),
			    gstcMakeSortArrowProd1(gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdAnd(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdOr(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdImp(),
			    gstcMakeSortArrowProd2(gsMakeSortIdBool(),gsMakeSortIdBool(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdEq(gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeUnknown(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNeq(gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeUnknown(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdIf(gsMakeUnknown()),
			    gstcMakeSortArrowProd3(gsMakeSortIdBool(),gsMakeUnknown(),gsMakeUnknown(),gsMakeUnknown()));
  //Numbers
  gstcAddSystemFunctionProd(gsMakeOpIdPos2Nat(),
			gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdPos2Int(),
			gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNat2Pos(),
			gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdNat2Int(),
			gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdInt2Pos(),
			gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdInt2Nat(),
			gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdLTE(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdLTE(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdLTE(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdLT(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdLT(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdLT(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdGTE(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGTE(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGTE(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdMax(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdInt(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdMax(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdMax(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdInt(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdMax(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdMax(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdMin(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdMin(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdMin(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdAbs(),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdNeg(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNeg(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdNeg(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdSucc(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdSucc(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdSucc(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdPred(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdPred(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdPred(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdAdd(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdAdd(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdAdd(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdAdd(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdSubt(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubt(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubt(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdMult(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdMult(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdMult(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdInt()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdDiv(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdDiv(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdInt()));
  gstcAddSystemFunctionProd(gsMakeOpIdMod(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdMod(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdPos(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdExp(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdNat(),gsMakeSortIdPos()));
  gstcAddSystemFunctionProd(gsMakeOpIdExp(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdExp(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdNat(),gsMakeSortIdInt()));
//Lists
  gstcAddSystemConstant(gsMakeOpIdNameEmptyList(gsMakeSortList(gsMakeUnknown()))),
  gstcAddSystemFunctionProd(gsMakeOpIdNameListSize(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameCons(gsMakeUnknown(), gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSnoc(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown(),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameConcat(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameEltAt(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat(),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameLTail(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameRHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameRTail(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
	    
//Sets
  gstcAddSystemFunctionProd(gsMakeOpIdNameSet2Bag(gsMakeSortSet(gsMakeUnknown()), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdNameEmptySet(gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetSize(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetIn(gsMakeUnknown(), gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubSetEq(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubSet(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetUnion(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetDiff(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetIntersect(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSetCompl(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));


//Bags
  gstcAddSystemFunctionProd(gsMakeOpIdNameBag2Set(gsMakeSortBag(gsMakeUnknown()), gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortBag(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdNameEmptyBag(gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagSize(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagIn(gsMakeUnknown(), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameCount(gsMakeUnknown(), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubBagEq(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameSubBag(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagUnion(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagDiff(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdNameBagIntersect(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
}

void gstcDataDestroy(void){
  ATtableDestroy(gssystem.constants);
  ATtableDestroy(gssystem.functions);
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
      if(gsIsSortRef(Sort)){
	ATtablePut(context.defined_sorts, (ATerm)SortName, (ATerm)ATAgetArgument(Sort,1));
	gsDebugMsg("recognized %t %t\n",SortName,(ATerm)ATAgetArgument(Sort,1));    	
      }
      else assert(0);
  }
 finally:
  return Result;
}  

static ATbool gstcReadInConstructors(){
  for(ATermList Sorts=ATtableKeys(context.defined_sorts);!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts))
    if(!gstcReadInSortStruct(ATAtableGet(context.defined_sorts,ATgetFirst(Sorts)))) return ATfalse;
  return ATtrue;
} 

static ATbool gstcReadInFuncs(ATermList Funcs){
  gsDebugMsg("Star Read-in Func\n");    
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);
    
    if(gsIsSortArrowProd(FuncType)){
      if(!gstcAddFunction(FuncName,FuncType,"function")) {ThrowF;}
    }
    else{
      if(!gstcAddConstant(FuncName,FuncType,"constant")) {ThrowMF("Could not add constant\n");}
    }
    gsDebugMsg("Read-in Func %t, Types %t\n",FuncName,FuncType);    
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
	Types=ATappend(Types,(ATerm)ActType);
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
	Types=ATappend(Types,(ATerm)ProcType);
      }
    }
    ATtablePut(context.processes,(ATerm)ProcName,(ATerm)Types);
    ATtablePut(body.proc_pars,(ATerm)ATAgetArgument(Proc,0),(ATerm)ATLgetArgument(Proc,1));
    ATtablePut(body.proc_bodies,(ATerm)ATAgetArgument(Proc,0),(ATerm)ATAgetArgument(Proc,2));
    gsDebugMsg("Read-in Proc Name %t, Types %t\n",ProcName,Types);    
  }
  ATtablePut(body.proc_pars,(ATerm)INIT_KEY(),(ATerm)ATmakeList0());
  ATtablePut(body.proc_bodies,(ATerm)INIT_KEY(),(ATerm)Init);
 finally:
  return Result;
} 

static ATermList gstcWriteProcs(void){
  ATermList Result=ATmakeList0();
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    if(ProcVar==INIT_KEY()) continue;
    Result=ATinsert(Result,(ATerm)gsMakeProcEqn(ProcVar,
						ATLtableGet(body.proc_pars,(ATerm)ProcVar),
						ATAtableGet(body.proc_bodies,(ATerm)ProcVar)
						)
		    );
  }
  return Result;
}

static ATbool gstcInferTypesData(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);
  for(ATermList Eqns=body.equations;!ATisEmpty(Eqns);Eqns=ATgetNext(Eqns)){
    ATermAppl Eqn=ATAgetFirst(Eqns);
    Vars=gstcAddVars2Table(Vars,ATLgetArgument(Eqn,0));
    if(!Vars){ThrowF;}
    ATermAppl Cond=ATAgetArgument(Eqn,1);
    if(!gsIsNil(Cond) && !gstcInferTypesD(Vars,Cond)){ThrowF;}
    if(!gstcInferTypesD(Vars,ATAgetArgument(Eqn,2))){ThrowF;}
    if(!gstcInferTypesD(Vars,ATAgetArgument(Eqn,3))){ThrowF;}
  }
 finally:
  ATtableDestroy(Vars);
  return Result;
}

static ATbool gstcInferTypesProc(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    Vars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,(ATerm)ProcVar));
    if(!Vars){ThrowF;}
    if(!gstcInferTypesP(Vars,ATAtableGet(body.proc_bodies,(ATerm)ProcVar))){ATtableDestroy(Vars);ThrowF;}
  } 
 finally:
  ATtableDestroy(Vars);
  return Result;
}

static ATbool gstcTransformVarConsTypeData(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);
  
  //data terms in equations
  ATermList NewEqns=ATmakeList0();
  for(ATermList Eqns=body.equations;!ATisEmpty(Eqns);Eqns=ATgetNext(Eqns)){
    ATermAppl Eqn=ATAgetFirst(Eqns);
    ATermList VarList=ATLgetArgument(Eqn,0);
      Vars=gstcAddVars2Table(Vars,VarList);
      if(!Vars){ThrowF;}
      ATermAppl Cond=ATAgetArgument(Eqn,1);
      if(!gsIsNil(Cond) && !(gstcTraverseVarConsTypeD(Vars,&Cond,gsMakeSortIdBool()))){ThrowF;}
      ATermAppl Left=ATAgetArgument(Eqn,2);
      ATermAppl LeftType;
      if(!(LeftType=gstcTraverseVarConsTypeD(Vars,&Left,gsMakeUnknown()))){ThrowF;}
      ATermAppl Right=ATAgetArgument(Eqn,3);
      if(!(gstcTraverseVarConsTypeD(Vars,&Right,LeftType))){ThrowF;}
      NewEqns=ATinsert(NewEqns,(ATerm)gsMakeDataEqn(VarList,Cond,Left,Right));
  }
  body.equations=ATreverse(NewEqns);
  
 finally:
  ATtableDestroy(Vars);
  return Result;
} 
 
static ATbool gstcTransformActProcVarConst(void){
  ATbool Result=ATtrue;
  ATermTable Vars=ATtableCreate(63,50);

  //process and data terms in processes and init
  for(ATermList ProcVars=ATtableKeys(body.proc_pars);!ATisEmpty(ProcVars);ProcVars=ATgetNext(ProcVars)){
    ATermAppl ProcVar=ATAgetFirst(ProcVars);
    ATtableReset(Vars);
    Vars=gstcAddVars2Table(Vars,ATLtableGet(body.proc_pars,(ATerm)ProcVar));
    if(!Vars){ThrowF;}
    ATermAppl NewProcTerm=gstcTraverseActProcVarConstP(Vars,ATAtableGet(body.proc_bodies,(ATerm)ProcVar));
    if(!NewProcTerm){ThrowF;}
    ATtablePut(body.proc_bodies,(ATerm)ProcVar,(ATerm)NewProcTerm);
  } 

 finally:
  ATtableDestroy(Vars);
  return Result;
}

// ======== Auxiliary functions
static ATbool gstcInTypesA(ATermAppl Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesA(Type,ATAgetFirst(Types))) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesA(ATermAppl Type1, ATermAppl Type2){
  if(ATisEqual(Type1, Type2)) return ATtrue;

  if(!Type1 || !Type2) return ATfalse;

  return ATisEqual(gstcUnwindType(Type1),gstcUnwindType(Type2));
}

static ATbool gstcInTypesL(ATermList Type, ATermList Types){
  for(;!ATisEmpty(Types);Types=ATgetNext(Types))
    if(gstcEqTypesL(Type,ATLgetFirst(Types))) return ATtrue;
  return ATfalse;
}

static ATbool gstcEqTypesL(ATermList Type1, ATermList Type2){
  if(ATisEqual(Type1, Type2)) return ATtrue;
  if(!Type1 || !Type2) return ATfalse;
  if(ATgetLength(Type1)!=ATgetLength(Type2)) return ATfalse;
  for(;!ATisEmpty(Type1);Type1=ATgetNext(Type1),Type2=ATgetNext(Type2))
    if(!gstcEqTypesA(ATAgetFirst(Type1),ATAgetFirst(Type2))) return ATfalse;
  return ATtrue;
}

static ATbool gstcReadInSortStruct(ATermAppl SortExpr){
  ATbool Result=ATtrue;

  if(gsIsSortId(SortExpr)) return ATtrue;

  if(gsIsSortList(SortExpr) || gsIsSortSet(SortExpr) || gsIsSortBag(SortExpr))
    return gstcReadInSortStruct(ATAgetArgument(SortExpr,0));

  if(gsIsSortArrowProd(SortExpr)){
    if(!gstcReadInSortStruct(ATAgetArgument(SortExpr,1))) return ATfalse;
    for(ATermList Sorts=ATLgetArgument(SortExpr,0);!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
      if(!gstcReadInSortStruct(ATAgetFirst(Sorts))) return ATfalse;
    }
    return ATtrue;
  }
  
  if(gsIsSortStruct(SortExpr)){
    for(ATermList Constrs=ATLgetArgument(SortExpr,0);!ATisEmpty(Constrs);Constrs=ATgetNext(Constrs)){
      ATermAppl Constr=ATAgetFirst(Constrs);
    
      // recognizer -- if present -- a function from SortExpr to Bool
      ATermAppl Name=ATAgetArgument(Constr,2);
      if(!gsIsNil(Name) && 
	 !gstcAddFunction(Name,gsMakeSortArrowProd(ATmakeList1((ATerm)SortExpr),gsMakeSortExprBool()),"recognizer")) {ThrowF;}
      
      // constructor type and projections
      ATermList Projs=ATLgetArgument(Constr,1);
      Name=ATAgetArgument(Constr,0);
      if(ATisEmpty(Projs)){
	if(!gstcAddConstant(Name,SortExpr,"constructor constant")){ThrowF;}
	else continue;
      }
      
      ATermList ConstructorType=ATmakeList0();
      for(;!ATisEmpty(Projs);Projs=ATgetNext(Projs)){
	ATermAppl Proj=ATAgetFirst(Projs);
	ATermAppl ProjSort=ATAgetArgument(Proj,1);
	
	// not to forget, recursive call for ProjSort ;-)
	if(!gstcReadInSortStruct(ProjSort)) {ThrowF;}

	ATermAppl ProjName=ATAgetArgument(Proj,0);
	if(!gsIsNil(ProjName) &&
	   !gstcAddFunction(ProjName,gsMakeSortArrowProd(ATmakeList1((ATerm)SortExpr),ProjSort),"projection")) {ThrowF;}
	ConstructorType=ATinsert(ConstructorType,(ATerm)ProjSort);
      }
      if(!gstcAddFunction(Name,gsMakeSortArrowProd(ATreverse(ConstructorType),SortExpr),"constructor")) {ThrowF;}
    }
    return ATtrue;
  }
  
  assert(0);
 finally:
  return Result;
}

static ATbool gstcAddConstant(ATermAppl Name, ATermAppl Sort, const char* msg){
  ATbool Result=ATtrue;

  if(ATAtableGet(context.constants, (ATerm)Name) || ATLtableGet(context.functions, (ATerm)Name)){
    ThrowMF("Double declaration of %s %t\n", msg, Name);
  }

  if(ATAtableGet(gssystem.constants, (ATerm)Name) || ATLtableGet(gssystem.functions, (ATerm)Name)){
    ThrowMF("Attempt to redeclare the system identifier with %s %t\n", msg, Name);
  }
  
  ATtablePut(context.constants, (ATerm)Name, (ATerm)Sort);
 finally:
  return Result;
}

static ATbool gstcAddFunction(ATermAppl Name, ATermAppl Sort, const char *msg){
  ATbool Result=ATtrue;

  if(ATAtableGet(context.constants, (ATerm)Name)){
    ThrowMF("Double declaration of constant and %s %t\n", msg, Name);
  }

  if(ATAtableGet(gssystem.constants, (ATerm)Name) || ATLtableGet(gssystem.functions, (ATerm)Name)){
    ThrowMF("Attempt to redeclare the system identifier with %s %t\n", msg, Name);
  }

  ATermList Types=ATLtableGet(context.functions, (ATerm)Name);
  // the table context.functions contains a list of types for each
  // function name. We need to check if there is already such a type 
  // in the list. If so -- error, otherwise -- add
  if (Types && gstcInTypesA(Sort, Types)){
    ThrowMF("Double declaration of %s %t\n", msg, Name);
  }
  else{
    if (!Types) Types=ATmakeList0();
    Types=ATappend(Types,(ATerm)Sort);
    ATtablePut(context.functions,(ATerm)Name,(ATerm)Types);
  }
  gsDebugMsg("Read-in %s %t Type %t\n",msg,Name,Types);    
 finally:
  return Result;
}

static void gstcAddSystemConstant(ATermAppl OpId){
  ATermAppl Name=ATAgetArgument(OpId,0);
  ATermList Types=ATLtableGet(gssystem.constants, (ATerm)Name);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)ATAgetArgument(OpId,1));
  ATtablePut(gssystem.constants,(ATerm)Name,(ATerm)Types);
}

static void gstcAddSystemFunctionProd(ATermAppl OpId, ATermAppl Type){
  // Replace type in OpId with Type and add
  ATermAppl Name=ATAgetArgument(OpId,0);
  ATermList Types=ATLtableGet(gssystem.functions, (ATerm)Name);

  if (!Types) Types=ATmakeList0();
  Types=ATappend(Types,(ATerm)Type);
  ATtablePut(gssystem.functions,(ATerm)Name,(ATerm)Types);
}

static ATermTable gstcAddVars2Table(ATermTable Vars, ATermList VarDecls){
  ATbool Result=ATtrue;

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

static ATermAppl gstcRewrActProc(ATermTable Vars, ATermAppl ProcTerm){
  ATermAppl Result=NULL;
  ATermAppl Name=ATAgetArgument(ProcTerm,0);
  ATermList ParList;

  ATbool action;

  if((ParList=ATLtableGet(context.actions,(ATerm)Name))){
    action=ATtrue;
  }
  else{
    if((ParList=ATLtableGet(context.processes,(ATerm)Name))){
      action=ATfalse;
    }
    else{
      ThrowM("Action or process %t not declared\n", Name);
    }
  }
  assert(!ATisEmpty(ParList));
  
  int nFactPars=ATgetLength(ATLgetArgument(ProcTerm,1));
  char *msg=(action)?"action":"process";
  
  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    ATermList NewParList=ATmakeList0();
    for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
      ATermList Par=ATLgetFirst(ParList);
      if(ATgetLength(Par)==nFactPars) NewParList=ATinsert(NewParList,(ATerm)Par);
    }
    ParList=ATreverse(NewParList);
  }
 
  if(ATisEmpty(ParList)) {
    ThrowM("No %s %t with %d parameters is declared (while typechecking %t)\n", msg, Name, nFactPars, ProcTerm);     
  }

  if(ATgetLength(ParList)==1){
    Result=gstcMakeActionOrProc(action,Name,ATLgetFirst(ParList),ATLgetArgument(ProcTerm,1));
  }
  else{
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=gstcMakeActionOrProc(action,Name,gstcGetNotInferredList(ParList),ATLgetArgument(ProcTerm,1));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  ATermList PosTypeList=ATLgetArgument(ATAgetArgument(Result,0),1);
  
  ATermList NewPars=ATmakeList0();
  ATermList NewPosTypeList=ATmakeList0();
  for(ATermList Pars=ATLgetArgument(ProcTerm,1);!ATisEmpty(Pars);Pars=ATgetNext(Pars),
	PosTypeList=ATgetNext(PosTypeList)){
    ATermAppl Par=ATAgetFirst(Pars);
    ATermAppl NewPosType=gstcTraverseVarConsTypeD(Vars,&Par,ATAgetFirst(PosTypeList));
    if(!NewPosType) {throw;}
    NewPars=ATinsert(NewPars,(ATerm)Par);
    NewPosTypeList=ATinsert(NewPosTypeList,(ATerm)NewPosType);
  }
  NewPars=ATreverse(NewPars);
  NewPosTypeList=ATreverse(NewPosTypeList);

  PosTypeList=gstcAdjustNotInferredList(NewPosTypeList,ParList);
  if(!PosTypeList) {Result=NULL; ThrowM("No %s %t with type %t is declared (while typechecking %t)",msg,Name,NewPosTypeList,ProcTerm);}
  
  if(gstcIsNotInferredL(PosTypeList)){
    gsWarningMsg("Ambiguous %s %t\n",msg,Name);
  }
    
  Result=gstcMakeActionOrProc(action,Name,PosTypeList,NewPars);

  gsDebugMsg("recognized %s %t\n",msg,Result);    
 finally:
  return Result;
}

static inline ATermAppl gstcMakeActionOrProc(ATbool action, ATermAppl Name, 
					     ATermList FormParList, ATermList FactParList){
  return (action)?gsMakeAction(gsMakeActId(Name,FormParList),FactParList)
    :gsMakeProcess(gsMakeProcVarId(Name,FormParList),FactParList);
}

static ATermAppl gstcTraverseActProcVarConstP(ATermTable Vars, ATermAppl ProcTerm){
  ATermAppl Result=NULL;
  AFun ProcSymbol=ATgetAFun(ProcTerm);
  int n = ATgetArity(ProcSymbol);
  if(n==0) return ProcTerm;

  if(gsIsActionProcess(ProcTerm)){
    return gstcRewrActProc(Vars,ProcTerm);
  }

  if(gsIsRestrict(ProcTerm) || gsIsHide(ProcTerm) || 
     gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm)){
    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewProc) {throw;}
    return ATsetArgument(ProcTerm,(ATerm)NewProc,1);
  }

  if(gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
     gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm)){
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));
 
   if(!NewLeft) {throw;}
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewRight) {throw;}
    return ATsetArgument(ATsetArgument(ProcTerm,(ATerm)NewLeft,0),(ATerm)NewRight,1);
  }

  if(gsIsAtTime(ProcTerm)){
    ATermAppl NewProc=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,0));
    if(!NewProc) {throw;}
    ATermAppl Time=ATAgetArgument(ProcTerm,1);
    ATermAppl NewTime=gstcTraverseVarConsTypeD(Vars,&Time,gsMakeSortIdNat());
    if(!NewTime) {throw;}
    return gsMakeAtTime(NewProc,NewTime);
  }

  if(gsIsCond(ProcTerm)){
    ATermAppl Cond=ATAgetArgument(ProcTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,&Cond,gsMakeSortIdBool());
    if(!NewType) {throw;}
    ATermAppl NewLeft=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,1));
    if(!NewLeft) {throw;}
    ATermAppl NewRight=gstcTraverseActProcVarConstP(Vars,ATAgetArgument(ProcTerm,2));
    if(!NewRight) {throw;}
    return gsMakeCond(Cond,NewLeft,NewRight);
  }

  if(gsIsSum(ProcTerm)){
    ATermTable NewVars=gstcAddVars2Table(Vars,ATLgetArgument(ProcTerm,0));
    if(!NewVars) {throw;}
    ATermAppl NewProc=gstcTraverseActProcVarConstP(NewVars,ATAgetArgument(ProcTerm,1));
    if(!NewProc) {throw;}
    return ATsetArgument(ProcTerm,(ATerm)NewProc,1);
  }
  
  assert(0);
 finally:
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeD(ATermTable Vars, ATermAppl *DataTerm, ATermAppl PosType){
  ATermAppl Result=NULL;
 
  if(gsIsNumber(*DataTerm)){
    ATermAppl Number=ATAgetArgument(*DataTerm,0);
    ATermAppl Sort;
    if(gstcIsPos(Number)) Sort=gsMakeSortIdPos();
    else if(gstcIsNat(Number)) Sort=gsMakeSortIdNat(); 
    else Sort=gsMakeSortIdInt(); 
    
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Sort,1);
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && Sort==gsMakeSortIdPos()){
      Sort=gsMakeSortIdNat();
      *DataTerm=gsMakeDataApplProd(gstcMakeOpIdPos2Nat(),ATmakeList1((ATerm)*DataTerm));
    }
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && Sort==gsMakeSortIdNat()){
      Sort=gsMakeSortIdInt();
      *DataTerm=gsMakeDataApplProd(gstcMakeOpIdNat2Int(),ATmakeList1((ATerm)*DataTerm));
    }
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && Sort==gsMakeSortIdInt()){
      ThrowM("A number type is not in this list of alowed types: %t (while typechecking %t)",PosType,*DataTerm);
    }
    return Sort;
  }

  if(gsIsSetBagComp(*DataTerm)){
    ATermTable NewVars=gstcAddVars2Table(Vars,ATmakeList1((ATerm)ATAgetArgument(*DataTerm,0)));
    if(!NewVars) {throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    ATermAppl NewType=gstcUnSetBag(PosType);
    if(!NewType) {throw;}
    NewType=gstcTraverseVarConsTypeD(NewVars,&Data,NewType);
    if(!NewType) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    NewType=gstcMakeNotInferredSetBag(NewType);
    NewType=gstcAdjustPosTypesA(NewType,PosType);
    return NewType;
  }

  if(gsIsForall(*DataTerm) || gsIsExists(*DataTerm)){
    ATermTable NewVars=gstcAddVars2Table(Vars,ATLgetArgument(*DataTerm,0));
    if(!NewVars) {throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    if(!gstcAdjustPosTypesA(gsMakeSortIdBool(),PosType)) {throw;}
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewVars,&Data,gsMakeSortIdBool());
    if(!NewType) {throw;}
    if(!gstcAdjustPosTypesA(gsMakeSortIdBool(),NewType)) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return gsMakeSortIdBool();
  }

  if(gsIsLambda(*DataTerm)){
    ATermTable NewVars=gstcAddVars2Table(Vars,ATLgetArgument(*DataTerm,0));
    if(!NewVars) {throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    ATermAppl NewType=gstcUnDataAppl(PosType);
    NewType=gstcTraverseVarConsTypeD(NewVars,&Data,NewType);
    if(!NewType) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return NewType;
  }
  
  if(gsIsWhr(*DataTerm)){
    ATermList WhereVarList=ATmakeList0();
    for(ATermList WhereList=ATLgetArgument(*DataTerm,1);!ATisEmpty(WhereList);WhereList=ATgetNext(WhereList)){
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=ATAgetArgument(WhereElem,1);
      ATermAppl WhereType=gstcTraverseVarConsTypeD(Vars,&WhereTerm,gsMakeUnknown());
      if(!WhereType) {throw;}
      WhereVarList=ATinsert(WhereVarList, (ATerm)gsMakeDataVarId(ATAgetArgument(WhereElem,0),WhereType));
      
    }
    ATermTable NewVars=gstcAddVars2Table(Vars,ATreverse(WhereVarList));
    if(!NewVars) {throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewVars,&Data,PosType);
    if(!NewType) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,0);
    return NewType;
  }

  if(gsIsListEnum(*DataTerm) || gsIsSetEnum(*DataTerm)){
    ATermList NewData=gstcTraverseVarConstL(Vars,ATLgetArgument(*DataTerm,0));
    if(!NewData) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)NewData,0);
    //here
    return gsMakeUnknown();
  }  

  if(gsIsBagEnum(*DataTerm)){
    ATermList DataTermList2=ATLgetArgument(*DataTerm,0);
    ATermAppl Type;
    ATermList NewDataTermList2=ATmakeList0();
    for(;!ATisEmpty(DataTermList2);DataTermList2=ATgetNext(DataTermList2)){
      ATermAppl DataTerm2=ATAgetFirst(DataTermList2);
      ATermAppl DataTerm0=ATAgetArgument(DataTerm2,0);
      ATermAppl Type0=gstcTraverseVarConsTypeD(Vars,&DataTerm0,gsMakeUnknown());
      if(!Type0) {throw;}
      ATermAppl DataTerm1=ATAgetArgument(DataTerm2,1);
      ATermAppl Type1=gstcTraverseVarConsTypeD(Vars,&DataTerm1,gsMakeSortIdNat());
      if(!Type1) {throw;}
      NewDataTermList2=ATinsert(NewDataTermList2,(ATerm)gsMakeBagEnumElt(DataTerm0,DataTerm1));
      Type=Type0;
    }
    DataTermList2=ATreverse(NewDataTermList2);

    Type=gsMakeSortBag(Type);

    *DataTerm=gsMakeBagEnum(DataTermList2,Type);
    return Type;
  }

  if(gsIsDataApplProd(*DataTerm)){
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeDN(ATgetLength(ATLgetArgument(*DataTerm,1)),
						Vars,&Data,PosType);
    if(!NewType) {throw;}

    ATermList NewDatas=gstcTraverseVarConstL(Vars,ATLgetArgument(*DataTerm,1));
    
    if(!NewDatas) {throw;}
    *DataTerm=gsMakeDataApplProd(Data,NewDatas);
    
    if(gsIsSortArrowProd(gstcUnwindType(NewType))){
      return ATAgetArgument(gstcUnwindType(NewType),1);
    }

    return NewType;
  }  

  if(gsIsDataVarIdOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    ATermAppl Type=ATAtableGet(Vars,(ATerm)Name);
    if(Type){
      *DataTerm=gsMakeDataVarId(Name,Type);
      return Type;
    }
    ATermList ParList;

    if((Type=ATAtableGet(context.constants,(ATerm)Name))){
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    if((ParList=ATLtableGet(gssystem.constants,(ATerm)Name))){
      if(ATgetLength(ParList)==1){
	Type=ATAgetFirst(ParList);
	*DataTerm=gsMakeOpId(Name,Type);
	return Type;
      }
      else{
	gsWarningMsg("Ambiguous system constant %t\n",Name);    
	*DataTerm=gsMakeOpId(Name,gsMakeUnknown());
	return gsMakeUnknown();
      }
    }

    ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
    ParList=ATLtableGet(context.functions,(ATerm)Name);
    if(!ParList) ParList=ParListS;
    else if(ParListS) ParList=ATconcat(ParListS,ParList);

    if(!ParList){
      gsErrorMsg("Unknown Op %t\n",Name);
      return NULL;
    }
    
    if(ATgetLength(ParList)==1){ 
      ATermAppl Type=ATAgetFirst(ParList);
      *DataTerm=gsMakeDataVarId(Name,Type);
      return Type;
    }
    else{
      gsWarningMsg("Unknown Op %t\n",Name);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }  
  assert(0);
 finally:
  return Result;
}
    
static ATermAppl gstcTraverseVarConsTypeDN(int nFactPars, ATermTable Vars, ATermAppl *DataTerm, ATermAppl PosType){
  if(gsIsDataVarIdOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    ATermAppl Type=ATAtableGet(Vars,(ATerm)Name);
    if(Type){
      *DataTerm=gsMakeDataVarId(Name,Type);
      return Type;
    }
    ATermList ParList;
    
    if(!nFactPars){
      if((Type=ATAtableGet(context.constants,(ATerm)Name))) return gsMakeOpId(Name,Type);
      else{
	if((ParList=ATLtableGet(gssystem.constants,(ATerm)Name))){
	  if(ATgetLength(ParList)==1){
	    ATermAppl Type=ATAgetFirst(ParList);
	    *DataTerm=gsMakeOpId(Name,Type);
	    return Type;
	  }
	  else{
	    gsWarningMsg("Ambiguous system constant %t\n",Name);    
	    *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
	    return Type;
	  }
	}
	else{
	  gsErrorMsg("Unknown Op %t\n",Name);
	  return NULL;
	}
      }
    }
    
    ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
    ParList=ATLtableGet(context.functions,(ATerm)Name);
    if(!ParList) ParList=ParListS;
    else if(ParListS) ParList=ATconcat(ParListS,ParList);

    if(!ParList) {
      gsErrorMsg("Unknown Op %t\n",Name);
      return NULL;
    }

    // filter ParList keeping only functions A_0#...#A_nFactPars->A
    {
      ATermList NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if(gsIsSortArrowProd(Par) && ATgetLength(ATLgetArgument(Par,0))==nFactPars) 
	  NewParList=ATinsert(NewParList,(ATerm)Par);
     }
      ParList=ATreverse(NewParList);
    }

    if(ATisEmpty(ParList)) {
      gsErrorMsg("Unknown Op %t with %d arguments\n",Name,nFactPars);    
      return NULL;
    }
    
    if(ATgetLength(ParList)==1){
      ATermAppl Type=ATAgetFirst(ParList);
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    else{
      gsWarningMsg("Ambiguous Op %t\n",Name);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }
  else return gstcTraverseVarConsTypeD(Vars,DataTerm,PosType);
}


static ATermList gstcTraverseVarConstL(ATermTable Vars, ATermList DataTermList){
  ATermList Result=NULL;
  ATermList NewDataTermList=ATmakeList0();

  for(;!ATisEmpty(DataTermList);DataTermList=ATgetNext(DataTermList)){
    ATermAppl DataTerm=ATAgetFirst(DataTermList);
    ATermAppl Type=gstcTraverseVarConsTypeD(Vars,&DataTerm,gsMakeUnknown());
    if(!Type) {throw;}
    NewDataTermList=ATinsert(NewDataTermList,(ATerm)DataTerm);
  }
  return ATreverse(NewDataTermList);
 finally:
  return Result;
}

// ================================================================================
// Phase 2 -- type inference
// ================================================================================
static ATbool gstcInferTypesP(ATermTable Vars, ATermAppl ProcTerm){
  ATbool Result=ATtrue;

 finally:
  return Result;
}

static ATbool gstcInferTypesD(ATermTable Vars, ATermAppl DataTerm){
  ATbool Result=ATtrue;
 finally:
  return Result;
}

static ATermList gstcGetNotInferredList(ATermList TypeListList){
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1
  
  ATermList Result=ATmakeList0();
  int nFormPars=ATgetLength(ATgetFirst(TypeListList));
  DECLA(ATermList,Pars,nFormPars);
  for(int i=0;i<nFormPars;i++){
    Pars[i]=ATmakeList0();
  }

  for(;!ATisEmpty(TypeListList);TypeListList=ATgetNext(TypeListList)){
    ATermList TypeList=ATLgetFirst(TypeListList);
    for(int i=0;i<nFormPars;TypeList=ATgetNext(TypeList),i++){
      Pars[i]=gstcInsertType(Pars[i],ATAgetFirst(TypeList));
    }
  }
  
  for(int i=nFormPars-1;i>=0;i--) Result=ATinsert(Result,(ATerm)gstcMakeNotInferred(ATreverse(Pars[i])));
  return Result;
}

static ATermList gstcInsertType(ATermList TypeList, ATermAppl Type){
  for(ATermList OldTypeList=TypeList;!ATisEmpty(OldTypeList);OldTypeList=ATgetNext(OldTypeList)){
    if(gstcEqTypesA(ATAgetFirst(OldTypeList),Type)) return TypeList;
  }
  return ATinsert(TypeList,(ATerm)Type);
}

static ATermAppl gstcAdjustPosTypesA(ATermAppl NewType, ATermAppl PosType){
  //PosType: possible types (normally obtained from above)
  //NewType: possible types (normally obtained from below)
  //return: Part of PosType that 'conforms' NewType.

  if(gsIsUnknown(NewType) || gstcEqTypesA(NewType,PosType)) return PosType;

  if(gsIsUnknown(PosType)) return NewType;

  if(!gstcIsNotInferred(NewType)){
    if(!gstcIsNotInferred(PosType)) return NULL; 
    else if(gstcInTypesA(NewType,ATLgetArgument(PosType,0))) return NewType;
    else return NULL; 
  }
  
  if(!gstcIsNotInferred(PosType)){
    if(gstcInTypesA(PosType,ATLgetArgument(NewType,0))) return PosType;
    else return NULL; 
  }

  ATermList Types=gstcTypesIntersect(ATLgetArgument(NewType,0),ATLgetArgument(PosType,0));
  if(!Types || ATisEmpty(Types)) return NULL;
  if(ATgetLength(Types)==1) return ATAgetFirst(Types);
  else return gstcMakeNotInferred(Types);
}

static ATermList gstcTypesIntersect(ATermList TypeList1, ATermList TypeList2){
  // returns the intersection of the 2 type lists 
  ATermList Result=ATmakeList0();

  for(;!ATisEmpty(TypeList2);TypeList2=ATgetNext(TypeList2)){
    ATermAppl Type2=ATAgetFirst(TypeList2);
    if(gstcInTypesA(Type2,TypeList1)) Result=ATinsert(Result,(ATerm)Type2);
  }
  return ATreverse(Result);
}

static ATermList gstcAdjustNotInferredList(ATermList PosTypeList, ATermList TypeListList){
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  gsDebugMsg("gstcAdjustNotInferredList: PosTypeList %t;    TypeListList:%t \n",PosTypeList,TypeListList);

  //if PosTypeList has only normal types -- check if it is in TypeListList, 
  //if so return PosTypeList, otherwise return NULL
  if(!gstcIsNotInferredL(PosTypeList)){
    if(gstcInTypesL(PosTypeList,TypeListList)) return PosTypeList;
    else return NULL;
  }
 
  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  ATermList NewTypeListList=ATmakeList0();
  for(;!ATisEmpty(TypeListList);TypeListList=ATgetNext(TypeListList)){
    ATermList TypeList=ATLgetFirst(TypeListList);
    if(gstcIsTypeAllowed(PosTypeList,TypeList))
      NewTypeListList=ATinsert(NewTypeListList,(ATerm)TypeList);
  }
  if(ATisEmpty(NewTypeListList)) return NULL;
  if(ATgetLength(NewTypeListList)==1) return ATLgetFirst(NewTypeListList);

  // otherwise return not inferred.
  return gstcGetNotInferredList(ATreverse(NewTypeListList));
}
  
static ATbool gstcIsTypeAllowed(ATermList PosTypeList, ATermList TypeList){
  gsDebugMsg("gstcIsTypeAllowed: PosTypeList %t;    TypeList:%t \n",PosTypeList,TypeList);
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList)){
    ATermAppl PosType=ATAgetFirst(PosTypeList);
    if(gsIsUnknown(PosType)) continue;
    if(gstcIsNotInferred(PosType)){
      if(!gstcInTypesA(ATAgetFirst(TypeList),ATLgetArgument(PosType,0))) return ATfalse;
      else continue;
    }
    //PosType is a notmal type
    if(!gstcEqTypesA(PosType,ATAgetFirst(TypeList))) return ATfalse;
  }
  gsDebugMsg("gstcIsTypeAllowed: True:  PosTypeList %t;    TypeList:%t \n",PosTypeList,TypeList);
  return ATtrue;
}

static ATbool gstcIsNotInferredL(ATermList TypeList){
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList)){
    ATermAppl Type=ATAgetFirst(TypeList);
    if(gstcIsNotInferred(Type)||gsIsUnknown(Type)) return ATtrue;
  }
  return ATfalse;
}

static ATermAppl gstcUnwindType(ATermAppl Type){
  if(!gsIsSortId(Type)) return Type;
  ATermAppl Value=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(Type,0));
  if(!Value) Value=Type;
  return Value;
}

static ATermAppl gstcUnSetBag(ATermAppl PosType){
  //select Set(Type), and Bag(Type), elements, return their list of arguments.
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortSet(PosType) || gsIsSortSet(PosType)) return ATAgetArgument(PosType,0);
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortSet(NewPosType) || gsIsSortSet(NewPosType)) NewPosType=ATAgetArgument(NewPosType,0);
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermAppl gstcMakeNotInferredSetBag(ATermAppl Type){
  //Type: any type (possibly Unknown or NotInferred)
  //Returns: Not inferred type with Set(Type), Bag(Type)...

  ATermList Types=ATmakeList0();
  if(!gstcIsNotInferred(Type)){
    Types=ATmakeList2((ATerm)gsMakeSortSet(Type),(ATerm)gsMakeSortBag(Type));
  }
  else{
    for(ATermList PosTypes=ATLgetArgument(Type,0);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl PosType=ATAgetFirst(PosTypes);
      Types=ATconcat(Types,ATmakeList2((ATerm)gsMakeSortSet(PosType),(ATerm)gsMakeSortBag(PosType)));
    }
  }

  return gstcMakeNotInferred(Types);
}

static ATermAppl gstcUnDataAppl(ATermAppl PosType){

}
