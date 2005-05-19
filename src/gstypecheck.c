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

static ATermList gstcWriteProcs(void);

static ATbool gstcInTypesA(ATermAppl, ATermList);
static ATbool gstcEqTypesA(ATermAppl, ATermAppl);
static ATbool gstcInTypesL(ATermList, ATermList);
static ATbool gstcEqTypesL(ATermList, ATermList);

static ATbool gstcIsSortDeclared(ATermAppl SortName);
static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr);
static ATbool gstcIsSortExprListDeclared(ATermList SortExprList);
static ATbool gstcReadInSortStruct(ATermAppl);
static ATbool gstcAddConstant(ATermAppl, ATermAppl, const char*);
static ATbool gstcAddFunction(ATermAppl, ATermAppl, const char*);
static void gstcAddSystemConstant(ATermAppl);
static void gstcAddSystemFunctionProd(ATermAppl, ATermAppl);

static void gstcATermTableCopy(ATermTable Vars, ATermTable CopyVars);

static ATermTable gstcAddVars2Table(ATermTable,ATermList);
static ATermAppl gstcRewrActProc(ATermTable, ATermAppl);
static inline ATermAppl gstcMakeActionOrProc(ATbool, ATermAppl, ATermList, ATermList);
static ATermAppl gstcTraverseActProcVarConstP(ATermTable, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeD(ATermTable, ATermAppl *, ATermAppl);
static ATermAppl gstcTraverseVarConsTypeDN(int, ATermTable, ATermAppl* , ATermAppl);

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

static inline ATermAppl gstcMakeOpIdPos2Int(void){
  return ATsetArgument(gsMakeOpIdPos2Int(),
		       (ATerm)gstcMakeSortArrowProd1(gsMakeSortIdPos(),gsMakeSortIdInt()),
		       1);
}

static inline ATermAppl gstcMakeOpIdNat2Int(void){
  return ATsetArgument(gsMakeOpIdNat2Int(),
		       (ATerm)gstcMakeSortArrowProd1(gsMakeSortIdNat(),gsMakeSortIdInt()),
		       1);
}

static inline ATermAppl gstcMakeOpIdSet2Bag(ATermAppl Type){
  return ATsetArgument(gsMakeOpIdSet2Bag(Type,Type),
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
static ATbool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType);
static ATbool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList);
static ATermAppl gstcUnwindType(ATermAppl Type);
static ATermAppl gstcUnSetBag(ATermAppl PosType);
static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType);
static ATermAppl gstcMakeNotInferredSetBag(ATermAppl Type);
static ATermAppl gstcAdjustPosTypesA(ATermAppl NewType, ATermAppl PosType);
static ATermList gstcTypesIntersect(ATermList TypeList1, ATermList TypeList2);
static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2);
static ATermList gstcGetVarTypes(ATermList VarDecls);
static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType);
static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList);
static ATbool gstcHasUnknown(ATermAppl Type);
static ATermAppl gstcExpandPosTypes(ATermAppl Type);
static ATermAppl gstcMinType(ATermList TypeList);
static ATbool gstcMActIn(ATermList MAct, ATermList MActs);
static ATbool gstcMActInSubEq(ATermList MAct, ATermList MActs);
static ATbool gstcMActEq(ATermList MAct1, ATermList MAct2);
static ATbool gstcMActSubEq(ATermList MAct1, ATermList MAct2);
static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2);
static ATermAppl gstcMatchIf(ATermAppl Type);
static ATermAppl gstcMatchEqNeq(ATermAppl Type);
static ATermAppl gstcMatchListOpCons(ATermAppl Type);
static ATermAppl gstcMatchListOpSnoc(ATermAppl Type);
static ATermAppl gstcMatchListOpConcat(ATermAppl Type);
static ATermAppl gstcMatchListOpEltAt(ATermAppl Type);
static ATermAppl gstcMatchListOpHead(ATermAppl Type);
static ATermAppl gstcMatchListOpTail(ATermAppl Type);


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
			     ATAgetArgument(ATAgetArgument(input,6),1))) {throw;}
  gsDebugMsg ("type checking read-in phase finished\n");
  
  gsDebugMsg ("type checking transform ActProc+VarConst phase started\n");
  if(!gstcTransformVarConsTypeData()){throw;}
  if(!gstcTransformActProcVarConst()){throw;}
  gsDebugMsg ("type checking transform ActProc+VarConst phase finished\n");

  //if(!gstcInferTypesData()) {throw;} //names and # of arguments
  //if(!gstcInferTypesProc()) {throw;} //names and # of arguments

  Result=ATsetArgument(input,(ATerm)gsMakeDataEqnSpec(body.equations),3);
  Result=ATsetArgument(Result,(ATerm)gsMakeProcEqnSpec(gstcWriteProcs()),5);
  Result=ATsetArgument(Result,(ATerm)gsMakeInit(ATmakeList0(),
    ATAtableGet(body.proc_bodies,(ATerm)INIT_KEY())),6);

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
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGTE(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
  //more
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdPos()),
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdNat()),
			    gstcMakeSortArrowProd2(gsMakeSortIdNat(),gsMakeSortIdNat(),gsMakeSortIdBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdGT(gsMakeSortIdInt()),
			    gstcMakeSortArrowProd2(gsMakeSortIdInt(),gsMakeSortIdInt(),gsMakeSortIdBool()));
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
			    gstcMakeSortArrowProd1(gsMakeSortIdInt(),gsMakeSortIdNat()));
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
			    gstcMakeSortArrowProd2(gsMakeSortIdPos(),gsMakeSortIdPos(),gsMakeSortIdPos()));
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
  gstcAddSystemConstant(gsMakeOpIdEmptyList(gsMakeSortList(gsMakeUnknown()))),
  gstcAddSystemFunctionProd(gsMakeOpIdListSize(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdCons(gsMakeUnknown(), gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdSnoc(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown(),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdConcat(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdEltAt(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd2(gsMakeSortList(gsMakeUnknown()),gsMakeSortExprNat(),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdLHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdLTail(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdRHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeUnknown()));
  gstcAddSystemFunctionProd(gsMakeOpIdRTail(gsMakeSortList(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortList(gsMakeUnknown()),gsMakeSortList(gsMakeUnknown())));
	    
//Sets
  gstcAddSystemFunctionProd(gsMakeOpIdSet2Bag(gsMakeSortSet(gsMakeUnknown()), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdEmptySet(gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdSetSize(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdSetIn(gsMakeUnknown(), gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubSetEq(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubSet(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdSetUnion(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdSetDiff(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdSetIntersect(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdSetCompl(gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortSet(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));


//Bags
  gstcAddSystemFunctionProd(gsMakeOpIdBag2Set(gsMakeSortBag(gsMakeUnknown()), gsMakeSortSet(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortBag(gsMakeUnknown()),gsMakeSortSet(gsMakeUnknown())));
  gstcAddSystemConstant(gsMakeOpIdEmptyBag(gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdBagSize(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd1(gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdBagIn(gsMakeUnknown(), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdCount(gsMakeUnknown(), gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeUnknown(),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprNat()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubBagEq(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdSubBag(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortExprBool()));
  gstcAddSystemFunctionProd(gsMakeOpIdBagUnion(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdBagDiff(gsMakeSortBag(gsMakeUnknown())),
			    gstcMakeSortArrowProd2(gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown()),gsMakeSortBag(gsMakeUnknown())));
  gstcAddSystemFunctionProd(gsMakeOpIdBagIntersect(gsMakeSortBag(gsMakeUnknown())),
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
    if(ATisEqual(gsMakeSortIdBool(),gsMakeSortId(SortName))){
      ThrowMF("Attempt to redeclare sort Bool\n");
    }				
    if(ATisEqual(gsMakeSortIdPos(),gsMakeSortId(SortName))){
      ThrowMF("Attempt to redeclare sort Pos\n");
    }				
    if(ATisEqual(gsMakeSortIdNat(),gsMakeSortId(SortName))){
      ThrowMF("Attempt to redeclare sort Nat\n");
    }				
    if(ATisEqual(gsMakeSortIdInt(),gsMakeSortId(SortName))){
      ThrowMF("Attempt to redeclare sort Int\n");
    }				
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
  for(ATermList Sorts=ATtableKeys(context.defined_sorts);!ATisEmpty(Sorts);Sorts=ATgetNext(Sorts)){
    ATermAppl SortExpr=ATAtableGet(context.defined_sorts,ATgetFirst(Sorts));
    if(!gstcIsSortExprDeclared(SortExpr)) return ATfalse;
    if(!gstcReadInSortStruct(SortExpr)) return ATfalse;
  }
  return ATtrue;
} 

static ATbool gstcReadInFuncs(ATermList Funcs){
  gsDebugMsg("Start Read-in Func\n");    
  ATbool Result=ATtrue;
  for(;!ATisEmpty(Funcs);Funcs=ATgetNext(Funcs)){
    ATermAppl Func=ATAgetFirst(Funcs);
    ATermAppl FuncName=ATAgetArgument(Func,0);
    ATermAppl FuncType=ATAgetArgument(Func,1);
    
    if(!gstcIsSortExprDeclared(FuncType)) {ThrowF;}
    //if FuncType is a defined function sort, unwind it
    { ATermAppl NewFuncType;
      if(gsIsSortId(FuncType) 
	 && (NewFuncType=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(FuncType,0))) 
	 && gsIsSortArrowProd(NewFuncType))
	FuncType=NewFuncType;
    }
    
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
    
    if(!gstcIsSortExprListDeclared(ActType)) {ThrowF;}

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
    ATermAppl ProcName=ATAgetArgument(ATAgetArgument(Proc,1),0);
    
    if(ATLtableGet(context.actions, (ATerm)ProcName)){
      ThrowMF("Declaration of both process and action %t\n", ProcName);
    }	

    ATermList ProcType=ATLgetArgument(ATAgetArgument(Proc,1),1);

    if(!gstcIsSortExprListDeclared(ProcType)) {ThrowF;}

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
    ATtablePut(body.proc_pars,(ATerm)ATAgetArgument(Proc,1),(ATerm)ATLgetArgument(Proc,2));
    ATtablePut(body.proc_bodies,(ATerm)ATAgetArgument(Proc,1),(ATerm)ATAgetArgument(Proc,3));
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
    Result=ATinsert(Result,(ATerm)gsMakeProcEqn(ATmakeList0(),
                                                ProcVar,
						ATLtableGet(body.proc_pars,(ATerm)ProcVar),
						ATAtableGet(body.proc_bodies,(ATerm)ProcVar)
						)
		    );
  }
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
    ATermAppl LeftType=gstcTraverseVarConsTypeD(Vars,&Left,gsMakeUnknown());
    if(!LeftType){ThrowF;}
    ATermAppl Right=ATAgetArgument(Eqn,3);
    ATermAppl RightType=gstcTraverseVarConsTypeD(Vars,&Right,LeftType);
    if(!RightType){ThrowF;}

    //If the types are not uniquly the same now: do once more:
    if(!gstcEqTypesA(LeftType,RightType)){
      gsDebugMsg("Doing again for the equation %t, LeftType: %t, RightType: %t\n",Eqn,LeftType,RightType);
      ATermAppl Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){ThrowMF("Types of the left- (%t) and right-hand-sides (%t) of the equation %t do not match\n",LeftType,RightType,Eqn);}
      
      Left=ATAgetArgument(Eqn,2);
      LeftType=gstcTraverseVarConsTypeD(Vars,&Left,Type);
      if(!LeftType){ThrowF;}
    
      Right=ATAgetArgument(Eqn,3);
      RightType=gstcTraverseVarConsTypeD(Vars,&Right,LeftType);
      if(!RightType){ThrowF;}
      
      Type=gstcTypeMatchA(LeftType,RightType);
      if(!Type){ThrowMF("Types of the left- and right-hand-sides of the equation %t do not match",Eqn);}
      if(gstcHasUnknown(Type)){ThrowMF("Types of the left- and right-hand-sides of the equation %t cannot be uniquily determined",Eqn);}
    }
    ATtableReset(Vars);
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

static ATbool gstcIsSortDeclared(ATermAppl SortName){

  gsDebugMsg("gstcIsSortDeclared: SortName %t\n",SortName);    

  if(ATisEqual(gsMakeSortIdBool(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdPos(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdNat(),gsMakeSortId(SortName))) return ATtrue;
  if(ATisEqual(gsMakeSortIdInt(),gsMakeSortId(SortName))) return ATtrue;
  if(ATindexedSetGetIndex(context.basic_sorts, (ATerm)SortName)>=0) return ATtrue;
  if(ATAtableGet(context.defined_sorts,(ATerm)SortName)) return ATtrue;
  return ATfalse;
}

static ATbool gstcIsSortExprDeclared(ATermAppl SortExpr){
  ATbool Result=ATtrue;

  if(gsIsSortId(SortExpr)){ 
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if(!gstcIsSortDeclared(SortName))
      {ThrowMF("Basic or defined sort %t is not declared\n",SortName);}
    return ATtrue;
  }

  if(gsIsSortList(SortExpr) || gsIsSortSet(SortExpr) || gsIsSortBag(SortExpr))
    return gstcIsSortExprDeclared(ATAgetArgument(SortExpr,0));
  
  if(gsIsSortArrowProd(SortExpr)){
    if(!gstcIsSortExprDeclared(ATAgetArgument(SortExpr,1))) return ATfalse;
    if(!gstcIsSortExprListDeclared(ATLgetArgument(SortExpr,0))) return ATfalse;
    return ATtrue;
  }
  
  if(gsIsSortStruct(SortExpr)){
    for(ATermList Constrs=ATLgetArgument(SortExpr,0);!ATisEmpty(Constrs);Constrs=ATgetNext(Constrs)){
      ATermAppl Constr=ATAgetFirst(Constrs);
    
      ATermList Projs=ATLgetArgument(Constr,1);
      for(;!ATisEmpty(Projs);Projs=ATgetNext(Projs)){
	ATermAppl Proj=ATAgetFirst(Projs);
	ATermAppl ProjSort=ATAgetArgument(Proj,1);
	
	// not to forget, recursive call for ProjSort ;-)
	if(!gstcIsSortExprDeclared(ProjSort)) {ThrowF;}
      }
    }
    return ATtrue;
  }
  
  assert(0);
 finally:
  return Result;
}

static ATbool gstcIsSortExprListDeclared(ATermList SortExprList){
  for(;!ATisEmpty(SortExprList);SortExprList=ATgetNext(SortExprList))
    if(!gstcIsSortExprDeclared(ATAgetFirst(SortExprList))) return ATfalse; 
  return ATtrue;
}


static ATbool gstcReadInSortStruct(ATermAppl SortExpr){
  ATbool Result=ATtrue;

  if(gsIsSortId(SortExpr)){ 
    ATermAppl SortName=ATAgetArgument(SortExpr,0);
    if(!gstcIsSortDeclared(SortName))
      {ThrowMF("Basic or defined sort %t is not declared\n",SortName);}
    return ATtrue;
  }

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

  //constants and functions can have the same names
  //  if(ATAtableGet(context.constants, (ATerm)Name)){
  //    ThrowMF("Double declaration of constant and %s %t\n", msg, Name);
  //  }

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

static void gstcATermTableCopy(ATermTable Orig, ATermTable Copy){
  for(ATermList Keys=ATtableKeys(Orig);!ATisEmpty(Keys);Keys=ATgetNext(Keys)){
    ATerm Key=ATgetFirst(Keys);
    ATtablePut(Copy,Key,ATtableGet(Orig,Key));
  }
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

  goto finally;
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
    if(!NewPosType) {Result=NULL; throw;}
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

    //restrict & hide
    if(gsIsRestrict(ProcTerm) || gsIsHide(ProcTerm)){
      char *msg=gsIsRestrict(ProcTerm)?"Restricting":"Hiding";
      ATermList ActList=ATLgetArgument(ProcTerm,0);
      if(ATisEmpty(ActList)) gsWarningMsg("%s empty set of actions (typechecking %t)\n",msg,ProcTerm);

      ATermIndexedSet Acts=ATindexedSetCreate(63,50);
      for(;!ATisEmpty(ActList);ActList=ATgetNext(ActList)){
	ATermAppl Act=ATAgetFirst(ActList);
	
	//Actions must be declared
	if(!ATtableGet(context.actions,(ATerm)Act)) {ThrowM("%s an undefined action %t (typechecking %t)\n",msg,Act,ProcTerm);}
	ATbool new;
	ATindexedSetPut(Acts,(ATerm)Act,&new);
	if(!new) gsWarningMsg("%s action %t twice (typechecking %t)\n",msg,Act,ProcTerm);
      }
      ATindexedSetDestroy(Acts);
    }

    //rename
    if(gsIsRename(ProcTerm)){
      ATermList RenList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(RenList)) gsWarningMsg("Renaming empty set of actions (typechecking %t)\n",ProcTerm);

      ATermIndexedSet ActsFrom=ATindexedSetCreate(63,50);

      for(;!ATisEmpty(RenList);RenList=ATgetNext(RenList)){
	ATermAppl Ren=ATAgetFirst(RenList);
	ATermAppl ActFrom=ATAgetArgument(Ren,0);
	ATermAppl ActTo=ATAgetArgument(Ren,1);
	
	if(ATisEqual(ActFrom,ActTo)) gsWarningMsg("Renaming action %t into itself (typechecking %t)\n",ActFrom,ProcTerm);
	
	//Actions must be declared and of the same types
	ATermList TypesFrom,TypesTo;
	if(!(TypesFrom=ATLtableGet(context.actions,(ATerm)ActFrom)))
	  {ThrowM("Renaming an undefined action %t (typechecking %t)\n",ActFrom,ProcTerm);}
	if(!(TypesTo=ATLtableGet(context.actions,(ATerm)ActTo)))
	  {ThrowM("Renaming into an undefined action %t (typechecking %t)\n",ActTo,ProcTerm);}

	TypesTo=gstcTypeListsIntersect(TypesFrom,TypesTo);
	if(!TypesTo || ATisEmpty(TypesTo))
	  {ThrowM("Renaming action %t into action %t: these two have no common type (typechecking %t)\n",ActTo,ActFrom,ProcTerm);}
 
	ATbool new;
	ATindexedSetPut(ActsFrom,(ATerm)ActFrom,&new);
	if(!new) {ThrowM("Renaming action %t twice (typechecking %t)\n",ActFrom,ProcTerm);}
     }
      ATindexedSetDestroy(ActsFrom);
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if(gsIsComm(ProcTerm)){
      ATermList CommList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(CommList)) gsWarningMsg("Synchronizing empty set of (multi)actions (typechecking %t)\n",ProcTerm);
      else{
	ATermList MActsFrom=ATmakeList0();

	for(;!ATisEmpty(CommList);CommList=ATgetNext(CommList)){
	  ATermAppl Comm=ATAgetFirst(CommList);
	  ATermList MActFrom=ATLgetArgument(ATAgetArgument(Comm,0),0);
	  ATermList BackupMActFrom=MActFrom;
	  assert(!ATisEmpty(MActFrom));
	  ATermAppl ActTo=ATAgetArgument(Comm,1);
	  
	  if(ATgetLength(MActFrom)==1)
	    gsWarningMsg("Using Syncronization as Renaming (hiding) of action %t into %t (typechecking %t)\n",
			 ATgetFirst(MActFrom),ActTo,ProcTerm);
	  
	  //Actions must be declared
	  ATermList ResTypes=NULL;

	  if(!gsIsNil(ActTo)){
	    ResTypes=ATLtableGet(context.actions,(ATerm)ActTo);
	    if(!ResTypes) 
	      {ThrowM("Synchronizing to an undefined action %t (typechecking %t)\n",ActTo,ProcTerm);}
	  }

	  for(;!ATisEmpty(MActFrom);MActFrom=ATgetNext(MActFrom)){
	    ATermAppl Act=ATAgetFirst(MActFrom);
	    ATermList Types=ATLtableGet(context.actions,(ATerm)Act);
	    if(!Types)
	      {ThrowM("Synchronizing an undefined action %t in (multi)action %t (typechecking %t)\n",Act,MActFrom,ProcTerm);}
	    ResTypes=(ResTypes)?gstcTypeListsIntersect(ResTypes,Types):Types;
	    if(!Types || ATisEmpty(Types))
	      {ThrowM("Synchronizing action %t from (multi)action into action %t: these have no common type (typechecking %t)\n",
		      Act,BackupMActFrom,ActTo,ProcTerm);}
	  }
	  MActFrom=BackupMActFrom;

	  if(gstcMActInSubEq(MActFrom,MActsFrom))
	    {ThrowM("Synchronizing (multi)action %t twice (typechecking %t)\n",MActFrom,ProcTerm);}
	  else MActsFrom=ATinsert(MActsFrom,(ATerm)MActFrom);
	}
      }
    }
    
    //allow
    if(gsIsAllow(ProcTerm)){
      ATermList MActList=ATLgetArgument(ProcTerm,0);

      if(ATisEmpty(MActList)) gsWarningMsg("Allowing empty set of (multi) actions (typechecking %t)\n",ProcTerm);
      else{
	ATermList MActs=ATmakeList0();
	
	for(;!ATisEmpty(MActList);MActList=ATgetNext(MActList)){
	  ATermList MAct=ATLgetArgument(ATAgetFirst(MActList),0);

	  //Actions must be declared
	  for(;!ATisEmpty(MAct);MAct=ATgetNext(MAct)){
	    ATermAppl Act=ATAgetFirst(MAct);
	    if(!ATLtableGet(context.actions,(ATerm)Act))
	      {ThrowM("Allowing an undefined action %t in (multi)action %t (typechecking %t)\n",Act,MAct,ProcTerm);}
	  }	

	  MAct=ATLgetArgument(ATAgetFirst(MActList),0);
	  if(gstcMActIn(MAct,MActs))
	    gsWarningMsg("Allowing (multi)action %t twice (typechecking %t)\n",MAct,ProcTerm);
	  else MActs=ATinsert(MActs,(ATerm)MAct);
	}
      }
    }

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
    ATermAppl NewType=gstcTraverseVarConsTypeD(Vars,&Time,gsMakeSortIdNat());
    if(!NewType) {throw;}
    return gsMakeAtTime(NewProc,Time);
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
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATLgetArgument(ProcTerm,0));
    if(!NewVars) {ATtableDestroy(CopyVars); throw;}
    ATermAppl NewProc=gstcTraverseActProcVarConstP(NewVars,ATAgetArgument(ProcTerm,1));
    ATtableDestroy(CopyVars);
    if(!NewProc) {ThrowMF("while typechecking %t\n",ProcTerm);}
    return ATsetArgument(ProcTerm,(ATerm)NewProc,1);
  }
  
  assert(0);
 finally:
  return Result;
}

static ATermAppl gstcTraverseVarConsTypeD(ATermTable Vars, ATermAppl *DataTerm, ATermAppl PosType){
  ATermAppl Result=NULL;
 
  gsDebugMsg("gstcTraverseVarConsTypeD: DataTerm %t with PosType %t\n",*DataTerm,PosType);    

  if(gsIsNumber(*DataTerm)){
    ATermAppl Number=ATAgetArgument(*DataTerm,0);
    ATermAppl Sort;
    if(gstcIsPos(Number)) Sort=gsMakeSortIdPos();
    else if(gstcIsNat(Number)) Sort=gsMakeSortIdNat(); 
    else Sort=gsMakeSortIdInt(); 
    
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Sort,1);
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && ATisEqual(Sort,gsMakeSortIdPos())){
      Sort=gsMakeSortIdNat();
      *DataTerm=gsMakeDataApplProd(gstcMakeOpIdPos2Nat(),ATmakeList1((ATerm)*DataTerm));
    }
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && ATisEqual(Sort,gsMakeSortIdNat())){
      Sort=gsMakeSortIdInt();
      *DataTerm=gsMakeDataApplProd(gstcMakeOpIdNat2Int(),ATmakeList1((ATerm)*DataTerm));
    }
    
    if(!gstcAdjustPosTypesA(Sort,PosType) && ATisEqual(Sort,gsMakeSortIdInt())){
      ThrowM("A number type is not in this list of allowed types: %t (while typechecking %t)",PosType,*DataTerm);
    }
    return Sort;
  }

  if(gsIsSetBagComp(*DataTerm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATmakeList1((ATerm)ATAgetArgument(*DataTerm,0)));
    if(!NewVars) {ATtableDestroy(CopyVars); throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    ATermAppl NewType=gstcUnSetBag(PosType);
    if(!NewType) {ATtableDestroy(CopyVars); throw;}
    NewType=gstcTraverseVarConsTypeD(NewVars,&Data,NewType);
    ATtableDestroy(CopyVars); 
    if(!NewType) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    NewType=gstcMakeNotInferredSetBag(NewType);
    NewType=gstcAdjustPosTypesA(NewType,PosType);
    return NewType;
  }

  if(gsIsForall(*DataTerm) || gsIsExists(*DataTerm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATLgetArgument(*DataTerm,0));
    if(!NewVars) {ATtableDestroy(CopyVars); throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    if(!gstcAdjustPosTypesA(gsMakeSortIdBool(),PosType)) {ATtableDestroy(CopyVars); throw;}
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewVars,&Data,gsMakeSortIdBool());
    ATtableDestroy(CopyVars); 
    if(!NewType) {throw;}
    if(!gstcAdjustPosTypesA(gsMakeSortIdBool(),NewType)) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return gsMakeSortIdBool();
  }

  if(gsIsLambda(*DataTerm)){
    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermList VarList=ATLgetArgument(*DataTerm,0);
    ATermTable NewVars=gstcAddVars2Table(CopyVars,VarList);
    if(!NewVars) {ATtableDestroy(CopyVars); throw;}
    ATermList ArgTypes=gstcGetVarTypes(VarList);
    ATermAppl NewType=gstcUnArrowProd(ArgTypes,PosType);
    if(!NewType) {ATtableDestroy(CopyVars); ThrowM("No functions with arguments %t among %t (while typechecking %t)\n", ArgTypes,PosType,*DataTerm);}
    ATermAppl Data=ATAgetArgument(*DataTerm,1);
    NewType=gstcTraverseVarConsTypeD(NewVars,&Data,NewType);
    ATtableDestroy(CopyVars); 
    if(!NewType) {throw;}
    *DataTerm=ATsetArgument(*DataTerm,(ATerm)Data,1);
    return gsMakeSortArrowProd(ArgTypes,NewType);
  }
  
  if(gsIsWhr(*DataTerm)){
    ATermList WhereVarList=ATmakeList0();
    ATermList NewWhereList=ATmakeList0();
    for(ATermList WhereList=ATLgetArgument(*DataTerm,1);!ATisEmpty(WhereList);WhereList=ATgetNext(WhereList)){
      ATermAppl WhereElem=ATAgetFirst(WhereList);
      ATermAppl WhereTerm=ATAgetArgument(WhereElem,1);
      ATermAppl WhereType=gstcTraverseVarConsTypeD(Vars,&WhereTerm,gsMakeUnknown());
      if(!WhereType) {throw;}
      WhereVarList=ATinsert(WhereVarList,(ATerm)gsMakeDataVarId(ATAgetArgument(WhereElem,0),WhereType));
      NewWhereList=ATinsert(NewWhereList,(ATerm)ATsetArgument(WhereElem,(ATerm)WhereTerm,1));
    }
    NewWhereList=ATreverse(NewWhereList);

    ATermTable CopyVars=ATtableCreate(63,50);
    gstcATermTableCopy(Vars,CopyVars);

    ATermTable NewVars=gstcAddVars2Table(CopyVars,ATreverse(WhereVarList));
    if(!NewVars) {ATtableDestroy(CopyVars); throw;}
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeD(NewVars,&Data,PosType);
    ATtableDestroy(CopyVars); 
    if(!NewType) {throw;}
    *DataTerm=gsMakeWhr(Data,NewWhereList);
    return NewType;
  }

  if(gsIsListEnum(*DataTerm) || gsIsSetEnum(*DataTerm)){
    ATermList DataTermList=ATLgetArgument(*DataTerm,0);
    ATermAppl Type=gsMakeUnknown();
    ATermList NewDataTermList=ATmakeList0();
    for(;!ATisEmpty(DataTermList);DataTermList=ATgetNext(DataTermList)){
      ATermAppl DataTerm=ATAgetFirst(DataTermList);
      ATermAppl Type0=gstcTraverseVarConsTypeD(Vars,&DataTerm,Type);
      if(!Type0) {throw;}
      NewDataTermList=ATinsert(NewDataTermList,(ATerm)DataTerm);
      Type=Type0;
    }
    DataTermList=ATreverse(NewDataTermList);

    Type=(gsIsListEnum(*DataTerm))?gsMakeSortList(Type):gsMakeSortSet(Type);
    
    *DataTerm=(gsIsListEnum(*DataTerm))?gsMakeListEnum(DataTermList,Type):gsMakeSetEnum(DataTermList,Type);
    return Type;
  }  

  if(gsIsBagEnum(*DataTerm)){
    ATermList DataTermList2=ATLgetArgument(*DataTerm,0);
    ATermAppl Type=gsMakeUnknown();
    ATermList NewDataTermList2=ATmakeList0();
    for(;!ATisEmpty(DataTermList2);DataTermList2=ATgetNext(DataTermList2)){
      ATermAppl DataTerm2=ATAgetFirst(DataTermList2);
      ATermAppl DataTerm0=ATAgetArgument(DataTerm2,0);
      ATermAppl Type0=gstcTraverseVarConsTypeD(Vars,&DataTerm0,Type);
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
    //arguments
    ATermList Arguments=ATLgetArgument(*DataTerm,1);
    int nArguments=ATgetLength(Arguments);

    ATermList NewArgumentTypes=ATmakeList0();
    ATermList NewArguments=ATmakeList0();
    for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments)){
      ATermAppl Arg=ATAgetFirst(Arguments);
      ATermAppl Type=gstcTraverseVarConsTypeD(Vars,&Arg,gsMakeUnknown());
      if(!Type) {throw;}
      NewArguments=ATinsert(NewArguments,(ATerm)Arg);
      NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
    }
    Arguments=ATreverse(NewArguments);
    ATermList ArgumentTypes=ATreverse(NewArgumentTypes);

    //function
    ATermAppl Data=ATAgetArgument(*DataTerm,0);
    ATermAppl NewType=gstcTraverseVarConsTypeDN(nArguments,Vars,
						&Data,gsMakeSortArrowProd(ArgumentTypes,PosType));
    if(!NewType) {ThrowM("(The type error above occurred in DataTerm %t while trying to cast it to type %t)\n",gsMakeDataApplProd(Data,Arguments),PosType);}
    
    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.

    if(gsIsSortArrowProd(gstcUnwindType(NewType))){
      ATermList NeededArgumentTypes=ATLgetArgument(gstcUnwindType(NewType),0);
     
      //arguments again
      ATermList NewArgumentTypes=ATmakeList0();
      ATermList NewArguments=ATmakeList0();
      for(;!ATisEmpty(Arguments);Arguments=ATgetNext(Arguments),
	    ArgumentTypes=ATgetNext(ArgumentTypes),NeededArgumentTypes=ATgetNext(NeededArgumentTypes)){
	ATermAppl Arg=ATAgetFirst(Arguments);
	ATermAppl NeededType=ATAgetFirst(NeededArgumentTypes);
	ATermAppl Type=ATAgetFirst(ArgumentTypes);
	if(!gstcEqTypesA(NeededType,Type)){
	  if(gstcTypeMatchA(Type,gsMakeSortIdPos()) && gstcTypeMatchA(NeededType,gsMakeSortIdNat())){
	    Type=gsMakeSortIdNat();
	    Arg=gsMakeDataApplProd(gstcMakeOpIdPos2Nat(),ATmakeList1((ATerm)Arg));
	  }
	  else if(gstcTypeMatchA(Type,gsMakeSortIdPos()) && gstcTypeMatchA(NeededType,gsMakeSortIdInt())){
	    Type=gsMakeSortIdInt();
	    Arg=gsMakeDataApplProd(gstcMakeOpIdPos2Int(),ATmakeList1((ATerm)Arg));
	  }
	  else if(gstcTypeMatchA(Type,gsMakeSortIdNat()) && gstcTypeMatchA(NeededType,gsMakeSortIdInt())){
	    Type=gsMakeSortIdInt();
	    Arg=gsMakeDataApplProd(gstcMakeOpIdNat2Int(),ATmakeList1((ATerm)Arg));
	  }
	  else{
	    gsDebugMsg("Doing again on %t, Type: %t, Needed type: %t\n",Arg,Type,NeededType);
	    ATermAppl NewType=gstcTypeMatchA(NeededType,Type);
	    if(!NewType){ThrowMF("Needed type %t does not match possible type %t (while typechecking %t in %t)\n",NeededType,Type,Arg,*DataTerm);}
	    Type=NewType;
	    Type=gstcTraverseVarConsTypeD(Vars,&Arg,Type);
	    if(!Type) {throw;}
	  }
	}
	NewArguments=ATinsert(NewArguments,(ATerm)Arg);
	NewArgumentTypes=ATinsert(NewArgumentTypes,(ATerm)Type);
      }
      Arguments=ATreverse(NewArguments);
      ArgumentTypes=ATreverse(NewArgumentTypes);     
    }
    
    *DataTerm=gsMakeDataApplProd(Data,Arguments);
    
    if(gsIsSortArrowProd(gstcUnwindType(NewType))){
      return ATAgetArgument(gstcUnwindType(NewType),1);
    }
  
    return gstcUnArrowProd(ArgumentTypes,NewType);
  }  

  if(gsIsDataVarIdOpId(*DataTerm)||gsIsOpId(*DataTerm)){
    ATermAppl Name=ATAgetArgument(*DataTerm,0);
    ATermAppl Type=ATAtableGet(Vars,(ATerm)Name);
    if(Type){
      gsDebugMsg("Recognised variable %t, Type: %t\n",Name,Type);
      *DataTerm=gsMakeDataVarId(Name,Type);
 
      ATermAppl NewType=gstcTypeMatchA(Type,PosType);
      if(NewType) Type=NewType;
      else {
	if(gstcTypeMatchA(Type,gsMakeSortIdPos())){
	  if(gstcTypeMatchA(PosType,gsMakeSortIdNat())){
	    Type=gsMakeSortIdNat();
	    *DataTerm=gsMakeDataApplProd(gstcMakeOpIdPos2Nat(),ATmakeList1((ATerm)*DataTerm));
	  }
	  else 
	    if(gstcTypeMatchA(PosType,gsMakeSortIdInt())){
	      Type=gsMakeSortIdInt();
	      *DataTerm=gsMakeDataApplProd(gstcMakeOpIdPos2Int(),ATmakeList1((ATerm)*DataTerm));
	    }
	}
	else 
	  if(gstcTypeMatchA(Type,gsMakeSortIdNat()) && gstcTypeMatchA(PosType,gsMakeSortIdInt())){
	    Type=gsMakeSortIdInt();
	    *DataTerm=gsMakeDataApplProd(gstcMakeOpIdNat2Int(),ATmakeList1((ATerm)*DataTerm));
	  }
	  else{ThrowM("No variable %t with type %t",*DataTerm,PosType);}
      }
      
      return Type;
    }
    ATermList ParList;

    if((Type=ATAtableGet(context.constants,(ATerm)Name))){
      if(!(Type=gstcTypeMatchA(Type,PosType))) {ThrowM("No constant %t with type %t",*DataTerm,PosType);}
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    
    if((ParList=ATLtableGet(gssystem.constants,(ATerm)Name))){
      ATermList NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if(!(Par=gstcTypeMatchA(Par,PosType))) continue;
	NewParList=ATinsert(NewParList,(ATerm)Par);
      }
      ParList=ATreverse(NewParList);
      if(ATisEmpty(ParList)) {ThrowM("No system constant %t with type %t",*DataTerm,PosType);}

      if(ATgetLength(ParList)==1){
	Type=ATAgetFirst(ParList);
	*DataTerm=gsMakeOpId(Name,Type);
	return Type;
      }
      else{
	//gsWarningMsg("Ambiguous system constant %t\n",Name);    
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
      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    else{
      gsWarningMsg("Unknown Op %t\n",Name);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }

  if(gsIsDataVarId(*DataTerm)){
    return ATAgetArgument(*DataTerm,1);
  }

  assert(0);
 finally:
  return Result;
}
    
static ATermAppl gstcTraverseVarConsTypeDN(int nFactPars, ATermTable Vars, ATermAppl *DataTerm, ATermAppl PosType){
  gsDebugMsg("gstcTraverseVarConsTypeDN: DataTerm %t with PosType %t, nFactPars %d\n",*DataTerm,PosType,nFactPars);    
  if(gsIsDataVarIdOpId(*DataTerm)||gsIsOpId(*DataTerm)){
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
	  gsErrorMsg("Unknown constant %t\n",Name);
	  return NULL;
	}
      }
    }
    
    ATermList ParListS=ATLtableGet(gssystem.functions,(ATerm)Name);
    ParList=ATLtableGet(context.functions,(ATerm)Name);
    if(!ParList) ParList=ParListS;
    else if(ParListS) ParList=ATconcat(ParListS,ParList);

    if(!ParList) {
      gsErrorMsg("Unknown Op %t with %d parameters\n",Name,nFactPars);
      return NULL;
    }
    gsDebugMsg("Possible types for Op %t with %d arguments are (ParList: %t; PosType: %t)\n",Name,nFactPars,ParList,PosType);

    { // filter ParList keeping only functions A_0#...#A_nFactPars->A
      ATermList NewParList=ATmakeList0(); 
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if(!gsIsSortArrowProd(Par)) continue;
	if(!(ATgetLength(ATLgetArgument(Par,0))==nFactPars)) continue; 
	NewParList=ATinsert(NewParList,(ATerm)Par);
      }
      ParList=ATreverse(NewParList);

      // filter ParList keeping only functions of the right type
      ATermList BackupParList=ParList;
      NewParList=ATmakeList0();
      for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	ATermAppl Par=ATAgetFirst(ParList);
	if(!(Par=gstcTypeMatchA(Par,PosType))) continue;
	NewParList=ATinsert(NewParList,(ATerm)Par);
      }

      if(ATisEmpty(NewParList)){
	//Ok, this looks like a type error. We are not that strict. 
	//Pos can be Nat, or even Int...
	//So lets make PosType more liberal
	//We change every Pos to NotInferred(Pos,Nat,Int)...
	//and get the list. Then we take the min of the list.
	
	ParList=BackupParList;
	gsDebugMsg("Trying casting for Op %t with %d arguments (ParList: %t; PosType: %t)\n",Name,nFactPars,ParList,PosType);
	PosType=gstcExpandPosTypes(PosType);
	for(;!ATisEmpty(ParList);ParList=ATgetNext(ParList)){
	  ATermAppl Par=ATAgetFirst(ParList);
	  if(!(Par=gstcTypeMatchA(Par,PosType))) continue;
	  NewParList=ATinsert(NewParList,(ATerm)Par);
	}
	NewParList=ATreverse(NewParList);
	gsDebugMsg("The result of casting is %t\n",NewParList);
	if(ATgetLength(NewParList)>1) ParList=ATmakeList1((ATerm)gstcMinType(NewParList));
	else ParList=NewParList;
      }
      else ParList=ATreverse(NewParList);
    }
	
    if(ATisEmpty(ParList)) {
      gsErrorMsg("Unknown Op %t with %d arguments that matches %t\n",Name,nFactPars,PosType);    
      return NULL;
    }
    
    if(ATgetLength(ParList)==1){
      ATermAppl Type=ATAgetFirst(ParList);
      if(gstcHasUnknown(Type)){
	Type=gstcTypeMatchA(PosType,Type);
      }
      if(gstcHasUnknown(Type) && gsIsOpId(*DataTerm)){
	//gsWarningMsg("Here..................... Type %t, DataTerm1: %t, PosType %t\n",Type,ATAgetArgument(*DataTerm,1),PosType);    
	Type=gstcTypeMatchA(Type,ATAgetArgument(*DataTerm,1));
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdIf(gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing if matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchIf(Type);
	if(!NewType){
	  gsErrorMsg("The function if has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdEq(gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(ATAgetArgument(gsMakeOpIdNeq(gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing == or != matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchEqNeq(Type);
	if(!NewType){
	  gsErrorMsg("The function == or != has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdCons(gsMakeUnknown(),gsMakeSortList(gsMakeUnknown())),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpCons(Type);
	if(!NewType){
	  gsErrorMsg("The function |> has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdSnoc(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpSnoc(Type);
	if(!NewType){
	  gsErrorMsg("The function <| has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdConcat(gsMakeSortList(gsMakeUnknown())),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpConcat(Type);
	if(!NewType){
	  gsErrorMsg("The function <| has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdEltAt(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpEltAt(Type);
	if(!NewType){
	  gsErrorMsg("The function <| has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }
      
      if(ATisEqual(ATAgetArgument(gsMakeOpIdLHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(ATAgetArgument(gsMakeOpIdRHead(gsMakeSortList(gsMakeUnknown()), gsMakeUnknown()),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpHead(Type);
	if(!NewType){
	  gsErrorMsg("The function <| has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      if(ATisEqual(ATAgetArgument(gsMakeOpIdLTail(gsMakeSortList(gsMakeUnknown())),0),ATAgetArgument(*DataTerm,0))||
	 ATisEqual(ATAgetArgument(gsMakeOpIdRTail(gsMakeSortList(gsMakeUnknown())),0),ATAgetArgument(*DataTerm,0))){
	gsDebugMsg("Doing List insertion matching Type %t, PosType %t\n",Type,PosType);    
	ATermAppl NewType=gstcMatchListOpTail(Type);
	if(!NewType){
	  gsErrorMsg("The function <| has incompartible argument types %t (while typechecking %t)\n",Type,*DataTerm);
	  return NULL;
	}
	Type=NewType;
      }

      *DataTerm=gsMakeOpId(Name,Type);
      return Type;
    }
    else{
      gsWarningMsg("Ambiguous Op %t with %d parameters\n",Name,nFactPars);    
      *DataTerm=gsMakeOpId(Name,gsMakeUnknown());
      return gsMakeUnknown();
    }
  }
  else return gstcTraverseVarConsTypeD(Vars,DataTerm,PosType);
}

// ================================================================================
// Phase 2 -- type inference
// ================================================================================
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
  
  gsDebugMsg("gstcTypesIntersect:  TypeList1 %t;    TypeList2: %t\n",TypeList1,TypeList2);

  ATermList Result=ATmakeList0();

  for(;!ATisEmpty(TypeList2);TypeList2=ATgetNext(TypeList2)){
    ATermAppl Type2=ATAgetFirst(TypeList2);
    if(gstcInTypesA(Type2,TypeList1)) Result=ATinsert(Result,(ATerm)Type2);
  }
  return ATreverse(Result);
}

static ATermList gstcTypeListsIntersect(ATermList TypeListList1, ATermList TypeListList2){
  // returns the intersection of the 2 type list lists
  
  gsDebugMsg("gstcTypesIntersect:  TypeListList1 %t;    TypeListList2: %t\n",TypeListList1,TypeListList2);

  ATermList Result=ATmakeList0();

  for(;!ATisEmpty(TypeListList2);TypeListList2=ATgetNext(TypeListList2)){
    ATermList TypeList2=ATLgetFirst(TypeListList2);
    if(gstcInTypesL(TypeList2,TypeListList1)) Result=ATinsert(Result,(ATerm)TypeList2);
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
    if(gstcIsTypeAllowedL(TypeList,PosTypeList))
      NewTypeListList=ATinsert(NewTypeListList,(ATerm)TypeList);
  }
  if(ATisEmpty(NewTypeListList)) return NULL;
  if(ATgetLength(NewTypeListList)==1) return ATLgetFirst(NewTypeListList);

  // otherwise return not inferred.
  return gstcGetNotInferredList(ATreverse(NewTypeListList));
}
  
static ATbool gstcIsTypeAllowedL(ATermList TypeList, ATermList PosTypeList){
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(ATgetLength(TypeList)==ATgetLength(PosTypeList));
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList))
    if(!gstcIsTypeAllowedA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList))) return ATfalse;
  return ATtrue;
}

static ATbool gstcIsTypeAllowedA(ATermAppl Type, ATermAppl PosType){
  //Checks if Type is alowed by PosType
  if(gsIsUnknown(PosType)) return ATtrue;
  if(gstcIsNotInferred(PosType))
    return gstcInTypesA(Type,ATLgetArgument(PosType,0));

  //PosType is a normal type
  return gstcEqTypesA(Type,PosType);
}

static ATermAppl gstcTypeMatchA(ATermAppl Type, ATermAppl PosType){
  //Checks if Type is allowed by PosType and returns the matching subtype of Type
  //we assume that Type has no NotInferred
  
  gsDebugMsg("gstcTypeMatchA Type: %t;    PosType: %t \n",Type,PosType);

  if(gsIsUnknown(Type)) return PosType;
  if(gsIsUnknown(PosType) || gstcEqTypesA(Type,PosType)) return Type;
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypeList=ATLgetArgument(PosType,0);!ATisEmpty(PosTypeList);PosTypeList=ATgetNext(PosTypeList)){
      ATermAppl NewPosType=ATAgetFirst(PosTypeList);
      if((NewPosType=gstcTypeMatchA(Type,NewPosType))) return NewPosType;
    }
    gsDebugMsg("No match gstcTypeMatchA Type: %t;    PosType: %t \n",Type,PosType);
    return NULL;
  }
    
  //PosType is a normal type
  //if(!gstcHasUnknown(Type)) return NULL;

  if(gsIsSortId(Type)) Type=gstcUnwindType(Type);
  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);

  if(gsIsSortList(Type)){
    if(!gsIsSortList(PosType)) return NULL;
    ATermAppl NewType=gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0));
    if(!NewType) return NULL;
    return gsMakeSortList(gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0)));
  }

  if(gsIsSortSet(Type)){
    if(!gsIsSortSet(PosType)) return NULL;
    else return gsMakeSortBag(gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0)));
  }

  if(gsIsSortBag(Type)){
    if(!gsIsSortBag(PosType)) return NULL;
    else return gsMakeSortBag(gstcTypeMatchA(ATAgetArgument(Type,0),ATAgetArgument(PosType,0)));
  }

  if(gsIsSortArrowProd(Type)){
    if(!gsIsSortArrowProd(PosType)) return NULL;
    else{
      ATermList ArgTypes=gstcTypeMatchL(ATLgetArgument(Type,0),ATLgetArgument(PosType,0));
      if(!ArgTypes) return NULL;
      ATermAppl ResType=gstcTypeMatchA(ATAgetArgument(Type,1),ATAgetArgument(PosType,1));
      if(!ResType) return NULL;
      Type=gsMakeSortArrowProd(ArgTypes,ResType);
      gsDebugMsg("gstcTypeMatchA Done: Type: %t;    PosType: %t \n",Type,PosType);
      return Type;
    }
  }

  return NULL;
}

static ATermList gstcTypeMatchL(ATermList TypeList, ATermList PosTypeList){
  gsDebugMsg("gstcTypeMatchL TypeList: %t;    PosTypeList: %t \n",TypeList,PosTypeList);

  if(ATgetLength(TypeList)!=ATgetLength(PosTypeList)) return NULL;

  ATermList Result=ATmakeList0();
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList),PosTypeList=ATgetNext(PosTypeList)){
    ATermAppl Type=gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList));
    if(!Type) return NULL;
    Result=ATinsert(Result,(ATerm)gstcTypeMatchA(ATAgetFirst(TypeList),ATAgetFirst(PosTypeList)));
  }
  return ATreverse(Result);
}

static ATbool gstcIsNotInferredL(ATermList TypeList){
  for(;!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList)){
    ATermAppl Type=ATAgetFirst(TypeList);
    if(gstcIsNotInferred(Type)||gsIsUnknown(Type)) return ATtrue;
  }
  return ATfalse;
}

static ATermAppl gstcUnwindType(ATermAppl Type){
  //gsDebugMsg("gstcUnwindType Type: %t\n",Type);

  if(gsIsSortList(Type)) return gsMakeSortList(gstcUnwindType(ATAgetArgument(Type,0)));
  if(gsIsSortSet(Type)) return gsMakeSortSet(gstcUnwindType(ATAgetArgument(Type,0)));
  if(gsIsSortBag(Type)) return gsMakeSortBag(gstcUnwindType(ATAgetArgument(Type,0)));
  
  if(gsIsSortId(Type)){
    ATermAppl Value=ATAtableGet(context.defined_sorts,(ATerm)ATAgetArgument(Type,0));
    if(!Value) return Type;
    return gstcUnwindType(Value);
  } 
  
  return Type;
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

static ATermAppl gstcUnArrowProd(ATermList ArgTypes, ATermAppl PosType){
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //return TypeX if unique, the set of TypeX as NotInferred if many, NULL otherwise

  gsDebugMsg("gstcUnArrowProd: ArgTypes %t with PosType %t\n",ArgTypes,PosType);    

  if(gsIsSortId(PosType)) PosType=gstcUnwindType(PosType);
  if(gsIsSortArrowProd(PosType)){
    ATermList PosArgTypes=ATLgetArgument(PosType,0);
    if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) return NULL;
    if(gstcEqTypesL(PosArgTypes,ArgTypes)) return ATAgetArgument(PosType,1);
  }
  if(gsIsUnknown(PosType)) return PosType;

  ATermList NewPosTypes=ATmakeList0();
  if(gstcIsNotInferred(PosType)){
    for(ATermList PosTypes=ATLgetArgument(PosType,1);!ATisEmpty(PosTypes);PosTypes=ATgetNext(PosTypes)){
      ATermAppl NewPosType=ATAgetFirst(PosTypes);
      if(gsIsSortId(NewPosType)) NewPosType=gstcUnwindType(NewPosType);
      if(gsIsSortArrowProd(PosType)){
	ATermList PosArgTypes=ATLgetArgument(PosType,0);
	if(ATgetLength(PosArgTypes)!=ATgetLength(ArgTypes)) continue;
	if(gstcEqTypesL(PosArgTypes,ArgTypes)) NewPosType=ATAgetArgument(NewPosType,1);
      }
      else if(!gsIsUnknown(NewPosType)) continue;
      NewPosTypes=ATinsert(NewPosTypes,(ATerm)NewPosType);
    }
    NewPosTypes=ATreverse(NewPosTypes);
    return gstcMakeNotInferred(NewPosTypes);
  }
  return NULL;
}

static ATermList gstcGetVarTypes(ATermList VarDecls){
  ATermList Result=ATmakeList0();
  for(;!ATisEmpty(VarDecls);VarDecls=ATgetNext(VarDecls))
    Result=ATinsert(Result,(ATerm)ATAgetArgument(ATAgetFirst(VarDecls),1));
  return ATreverse(Result);
}

static ATbool gstcHasUnknown(ATermAppl Type){
  if(gsIsUnknown(Type)) return ATtrue;
  if(gsIsSortId(Type)) return ATfalse;
  if(gsIsSortList(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortSet(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortBag(Type)) return gstcHasUnknown(ATAgetArgument(Type,0));
  if(gsIsSortStruct(Type)) return ATfalse;

  if(gsIsSortArrowProd(Type)){
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      if(gstcHasUnknown(ATAgetFirst(TypeList))) return ATtrue;
    return gstcHasUnknown(ATAgetArgument(Type,1));
  } 
  
  return ATtrue;
}

static ATermAppl gstcExpandPosTypes(ATermAppl Type){
  //Expand Pos.. to possible bigger types.
  if(gsIsUnknown(Type)) return Type;
  if(gstcEqTypesA(gsMakeSortIdPos(),Type)) return gstcMakeNotInferred(ATmakeList3((ATerm)gsMakeSortIdPos(),(ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt()));
  if(gstcEqTypesA(gsMakeSortIdNat(),Type)) return gstcMakeNotInferred(ATmakeList2((ATerm)gsMakeSortIdNat(),(ATerm)gsMakeSortIdInt()));
  if(gsIsSortId(Type)) return Type;
  if(gsIsSortList(Type) || gsIsSortSet(Type) || gsIsSortBag(Type)) return ATsetArgument(Type,(ATerm)gstcExpandPosTypes(ATAgetArgument(Type,0)),0);
  if(gsIsSortStruct(Type)) return Type;

  if(gsIsSortArrowProd(Type)){
    //the argument types, and if the resulting type is SortArrowProd -- recursively
    ATermList NewTypeList=ATmakeList0();
    for(ATermList TypeList=ATLgetArgument(Type,0);!ATisEmpty(TypeList);TypeList=ATgetNext(TypeList))
      NewTypeList=ATinsert(NewTypeList,(ATerm)gstcExpandPosTypes(gstcUnwindType(ATAgetFirst(TypeList))));
    ATermAppl ResultType=ATAgetArgument(Type,1);
    if(!gsIsSortArrowProd(ResultType))
      return ATsetArgument(Type,(ATerm)ATreverse(NewTypeList),0);
    else 
      return gsMakeSortArrowProd(ATreverse(NewTypeList),gstcExpandPosTypes(gstcUnwindType(ResultType)));
  } 
  
  return Type;
}

static ATermAppl gstcMinType(ATermList TypeList){
  return ATAgetFirst(TypeList);
}


// =========================== MultiActions
static ATbool gstcMActIn(ATermList MAct, ATermList MActs){
  //returns true if MAct is in MActs
  for(;!ATisEmpty(MActs);MActs=ATgetNext(MActs))
    if(gstcMActEq(MAct,ATLgetFirst(MActs))) return ATtrue;
       
  return ATfalse;
}

static ATbool gstcMActInSubEq(ATermList MAct, ATermList MActs){
  //returns true if a supermultiaction of MAct is in MActs
  for(;!ATisEmpty(MActs);MActs=ATgetNext(MActs))
    if(gstcMActSubEq(MAct,ATLgetFirst(MActs))) return ATtrue;
       
  return ATfalse;
}

static ATbool gstcMActEq(ATermList MAct1, ATermList MAct2){
  //returns true if the two multiactions are equal.
  if(ATgetLength(MAct1)!=ATgetLength(MAct2)) return ATfalse;
  if(ATisEmpty(MAct1)) return ATtrue;
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=ATgetNext(MAct1);

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2=ATmakeList0();
  for(;!ATisEmpty(MAct2);MAct2=ATgetNext(MAct2)){
    ATermAppl Act2=ATAgetFirst(MAct2);
    if(ATisEqual(Act1,Act2)) {
      MAct2=ATconcat(ATreverse(NewMAct2),ATgetNext(MAct2)); goto gstcMActEq_found;
    }
    else{
      NewMAct2=ATinsert(NewMAct2,(ATerm)Act2);
    }
  }
  return ATfalse;
 gstcMActEq_found:
  return gstcMActEq(MAct1,MAct2);
}

static ATbool gstcMActSubEq(ATermList MAct1, ATermList MAct2){
  //returns true if MAct1 is a submultiaction of MAct2.
  if(ATgetLength(MAct1)>ATgetLength(MAct2)) return ATfalse;
  if(ATisEmpty(MAct1)) return ATtrue;
  ATermAppl Act1=ATAgetFirst(MAct1);
  MAct1=ATgetNext(MAct1);

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  ATermList NewMAct2=ATmakeList0();
  for(;!ATisEmpty(MAct2);MAct2=ATgetNext(MAct2)){
    ATermAppl Act2=ATAgetFirst(MAct2);
    if(ATisEqual(Act1,Act2)) {
      MAct2=ATconcat(ATreverse(NewMAct2),ATgetNext(MAct2)); goto gstcMActSubEqMA_found;
    }
    else{
      NewMAct2=ATinsert(NewMAct2,(ATerm)Act2);
    }
  }
  return ATfalse;
 gstcMActSubEqMA_found:
  return gstcMActSubEq(MAct1,MAct2);
}

static ATermAppl gstcUnifyMinType(ATermAppl Type1, ATermAppl Type2){
  //Find the minimal type that Unifies the 2. If not possible, return NULL.
  ATermAppl Res=gstcTypeMatchA(Type1,gstcExpandPosTypes(Type2));
  if(!Res) Res=gstcTypeMatchA(Type2,gstcExpandPosTypes(Type1));
  return Res;
}

static ATermAppl gstcMatchIf(ATermAppl Type){
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting 

  assert(gsIsSortArrowProd(Type));
  ATermList Args=ATLgetArgument(Type,0);
  ATermAppl Res=ATAgetArgument(Type,1);
  assert((ATgetLength(Args)==3));
  //assert(gsIsBool(ATAgetFirst(Args)));
  Args=ATgetNext(Args);

  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;
  Args=ATgetNext(Args);
  if(!(Res=gstcUnifyMinType(Res,ATAgetFirst(Args)))) return NULL;

  return gsMakeSortArrowProd(ATmakeList3((ATerm)gsMakeSortIdBool(),(ATerm)Res,(ATerm)Res),Res);
}

static ATermAppl gstcMatchEqNeq(ATermAppl Type){
  //tries to sort out the types for == or !=.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  //assert(gsIsBool(ATAgetArgument(Type,1)));
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  
  ATermAppl Arg=gstcUnifyMinType(Arg1,Arg2);
  if(!Arg) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Arg,(ATerm)Arg),gsMakeSortIdBool());
}

static ATermAppl gstcMatchListOpCons(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(gstcUnwindType(Res)));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(gstcUnwindType(Arg2)));
  Arg2=ATAgetArgument(Arg2,0);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)Res,(ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpSnoc(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));
  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortList(Arg1));
  Arg1=ATAgetArgument(Arg1,0);

  Args=ATgetNext(Args);
  ATermAppl Arg2=ATAgetFirst(Args);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)Res),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpConcat(ATermAppl Type){
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  ATermAppl Arg1=ATAgetFirst(Args);
  if(gsIsSortId(Arg1)) Arg1=gstcUnwindType(Arg1);
  assert(gsIsSortList(Arg1));
  Arg1=ATAgetArgument(Arg1,0);

  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(Arg2));
  Arg2=ATAgetArgument(Arg2,0);
  
  Res=gstcUnifyMinType(Res,Arg1);
  if(!Res) return NULL;

  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

static ATermAppl gstcMatchListOpEltAt(ATermAppl Type){
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==2));

  //assert((gsIsSortNat(ATAgetFirst(Args)));
  Args=ATgetNext(Args);

  ATermAppl Arg2=ATAgetFirst(Args);
  if(gsIsSortId(Arg2)) Arg2=gstcUnwindType(Arg2);
  assert(gsIsSortList(Arg2));
  Arg2=ATAgetArgument(Arg2,0);
  
  Res=gstcUnifyMinType(Res,Arg2);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList2((ATerm)gsMakeSortList(Res),(ATerm)gsMakeSortIdNat()),Res);
}

static ATermAppl gstcMatchListOpHead(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortList(Arg));
  Arg=ATAgetArgument(Arg,0);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortList(Res)),Res);
}

static ATermAppl gstcMatchListOpTail(ATermAppl Type){
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  assert(gsIsSortArrowProd(Type));
  ATermAppl Res=ATAgetArgument(Type,1);
  if(gsIsSortId(Res)) Res=gstcUnwindType(Res);
  assert(gsIsSortList(Res));
  Res=ATAgetArgument(Res,0);
  ATermList Args=ATLgetArgument(Type,0);
  assert((ATgetLength(Args)==1));
  ATermAppl Arg=ATAgetFirst(Args);
  if(gsIsSortId(Arg)) Arg=gstcUnwindType(Arg);
  assert(gsIsSortList(Arg));
  Arg=ATAgetArgument(Arg,0);

  Res=gstcUnifyMinType(Res,Arg);
  if(!Res) return NULL;

  return gsMakeSortArrowProd(ATmakeList1((ATerm)gsMakeSortList(Res)),gsMakeSortList(Res));
}

