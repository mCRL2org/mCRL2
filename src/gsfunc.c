#include "gsfunc.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

//strdup implementation
#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
char *strdup(const char *s)
{
    size_t len;
    char *p;

    len = strlen(s);
    if((p = (char *)malloc(len + 1)) == NULL)
	return NULL;
    return strcpy(p, s);
}
#endif

//ATerm libary work arounds

ATermAppl ATAelementAt(ATermList list, int index)
{
  return (ATermAppl) ATelementAt(list, index);
}

ATermList ATLelementAt(ATermList list, int index)
{
  return (ATermList) ATelementAt(list, index);
}

ATermInt ATIelementAt(ATermList list, int index)
{
  return (ATermInt) ATelementAt(list, index);
}

ATermAppl ATAgetArgument(ATermAppl appl, int nr)
{
  return (ATermAppl) ATgetArgument(appl, nr);
}

ATermList ATLgetArgument(ATermAppl appl, int nr)
{
  return (ATermList) ATgetArgument(appl, nr);
}

ATermInt ATIgetArgument(ATermAppl appl, int nr)
{
  return (ATermInt) ATgetArgument(appl, nr);
}

//Constant AFun's for each constructor element of the internal ATerm structure.
static AFun gsAFunSpecV1;
static AFun gsAFunSortSpec;
static AFun gsAFunConsSpec;
static AFun gsAFunMapSpec;
static AFun gsAFunDataEqnSpec;
static AFun gsAFunActSpec;
static AFun gsAFunProcEqnSpec;
static AFun gsAFunInit;
static AFun gsAFunSortId;
static AFun gsAFunSortRef;
static AFun gsAFunOpId;
static AFun gsAFunDataEqn;
static AFun gsAFunDataVarId;
static AFun gsAFunNil;
static AFun gsAFunActId;
static AFun gsAFunProcEqn;
static AFun gsAFunProcVarId;
static AFun gsAFunSortList;
static AFun gsAFunSortSet;
static AFun gsAFunSortBag;
static AFun gsAFunSortStruct;
static AFun gsAFunSortArrowProd;
static AFun gsAFunSortArrow;
static AFun gsAFunStructCons;
static AFun gsAFunStructProj;
static AFun gsAFunDataVarIdOpId;
static AFun gsAFunDataApplProd;
static AFun gsAFunDataAppl;
static AFun gsAFunNumber;
static AFun gsAFunListEnum;
static AFun gsAFunSetEnum;
static AFun gsAFunBagEnum;
static AFun gsAFunSetBagComp;
static AFun gsAFunForall;
static AFun gsAFunExists;
static AFun gsAFunLambda;
static AFun gsAFunWhr;
static AFun gsAFunUnknown;
static AFun gsAFunBagEnumElt;
static AFun gsAFunWhrDecl;
static AFun gsAFunActionProcess;
static AFun gsAFunAction;
static AFun gsAFunProcess;
static AFun gsAFunDelta;
static AFun gsAFunTau;
static AFun gsAFunSum;
static AFun gsAFunRestrict;
static AFun gsAFunAllow;
static AFun gsAFunHide;
static AFun gsAFunRename;
static AFun gsAFunComm;
static AFun gsAFunSync;
static AFun gsAFunAtTime;
static AFun gsAFunSeq;
static AFun gsAFunCond;
static AFun gsAFunBInit;
static AFun gsAFunMerge;
static AFun gsAFunLMerge;
static AFun gsAFunChoice;
static AFun gsAFunMultActName;
static AFun gsAFunRenameExpr;
static AFun gsAFunCommExpr;

static bool gsConstructorFunctionsEnabled = false;

//Enable constructor functions
void gsEnableConstructorFunctions(void)
{
  gsAFunSpecV1        = ATmakeAFun("SpecV1", 7, ATfalse);
  gsAFunSortSpec      = ATmakeAFun("SortSpec", 1, ATfalse);
  gsAFunConsSpec      = ATmakeAFun("ConsSpec", 1, ATfalse);
  gsAFunMapSpec       = ATmakeAFun("MapSpec", 1, ATfalse);
  gsAFunDataEqnSpec   = ATmakeAFun("DataEqnSpec", 1, ATfalse);
  gsAFunActSpec       = ATmakeAFun("ActSpec", 1, ATfalse);
  gsAFunProcEqnSpec   = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
  gsAFunInit          = ATmakeAFun("Init", 1, ATfalse);
  gsAFunSortId        = ATmakeAFun("SortId", 1, ATfalse);
  gsAFunSortRef       = ATmakeAFun("SortRef", 2, ATfalse);
  gsAFunOpId          = ATmakeAFun("OpId", 2, ATfalse);
  gsAFunDataEqn       = ATmakeAFun("DataEqn", 4, ATfalse);
  gsAFunDataVarId     = ATmakeAFun("DataVarId", 2, ATfalse);
  gsAFunNil           = ATmakeAFun("Nil", 0, ATfalse);
  gsAFunActId         = ATmakeAFun("ActId", 2, ATfalse);
  gsAFunProcEqn       = ATmakeAFun("ProcEqn", 3, ATfalse);
  gsAFunProcVarId     = ATmakeAFun("ProcVarId", 2, ATfalse);
  gsAFunSortList      = ATmakeAFun("SortList", 1, ATfalse);
  gsAFunSortSet       = ATmakeAFun("SortSet", 1, ATfalse);
  gsAFunSortBag       = ATmakeAFun("SortBag", 1, ATfalse);
  gsAFunSortStruct    = ATmakeAFun("SortStruct", 1, ATfalse);
  gsAFunSortArrowProd = ATmakeAFun("SortArrowProd", 2, ATfalse);
  gsAFunSortArrow     = ATmakeAFun("SortArrow", 2, ATfalse);
  gsAFunStructCons    = ATmakeAFun("StructCons", 3, ATfalse);
  gsAFunStructProj    = ATmakeAFun("StructProj", 2, ATfalse);
  gsAFunDataVarIdOpId = ATmakeAFun("DataVarIdOpId", 1, ATfalse);
  gsAFunDataApplProd  = ATmakeAFun("DataApplProd", 2, ATfalse);
  gsAFunDataAppl      = ATmakeAFun("DataAppl", 2, ATfalse);
  gsAFunNumber        = ATmakeAFun("Number", 2, ATfalse);
  gsAFunListEnum      = ATmakeAFun("ListEnum", 2, ATfalse);
  gsAFunSetEnum       = ATmakeAFun("SetEnum", 2, ATfalse);
  gsAFunBagEnum       = ATmakeAFun("BagEnum", 2, ATfalse);
  gsAFunSetBagComp    = ATmakeAFun("SetBagComp", 2, ATfalse);
  gsAFunForall        = ATmakeAFun("Forall", 2, ATfalse);
  gsAFunExists        = ATmakeAFun("Exists", 2, ATfalse);
  gsAFunLambda        = ATmakeAFun("Lambda", 2, ATfalse);
  gsAFunWhr           = ATmakeAFun("Whr", 2, ATfalse);
  gsAFunUnknown       = ATmakeAFun("Unknown", 0, ATfalse);
  gsAFunBagEnumElt    = ATmakeAFun("BagEnumElt", 2, ATfalse);
  gsAFunWhrDecl       = ATmakeAFun("WhrDecl", 2, ATfalse);
  gsAFunActionProcess = ATmakeAFun("ActionProcess", 2, ATfalse);
  gsAFunAction        = ATmakeAFun("Action", 2, ATfalse);
  gsAFunProcess       = ATmakeAFun("Process", 2, ATfalse);
  gsAFunDelta         = ATmakeAFun("Delta", 0, ATfalse);
  gsAFunTau           = ATmakeAFun("Tau", 0, ATfalse);
  gsAFunSum           = ATmakeAFun("Sum", 2, ATfalse);
  gsAFunRestrict      = ATmakeAFun("Restrict", 2, ATfalse);
  gsAFunAllow         = ATmakeAFun("Allow", 2, ATfalse);
  gsAFunHide          = ATmakeAFun("Hide", 2, ATfalse);
  gsAFunRename        = ATmakeAFun("Rename", 2, ATfalse);
  gsAFunComm          = ATmakeAFun("Comm", 2, ATfalse);
  gsAFunSync          = ATmakeAFun("Sync", 2, ATfalse);
  gsAFunAtTime        = ATmakeAFun("AtTime", 2, ATfalse);
  gsAFunSeq           = ATmakeAFun("Seq", 2, ATfalse);
  gsAFunCond          = ATmakeAFun("Cond", 3, ATfalse);
  gsAFunBInit         = ATmakeAFun("BInit", 2, ATfalse);
  gsAFunMerge         = ATmakeAFun("Merge", 2, ATfalse);
  gsAFunLMerge        = ATmakeAFun("LMerge", 2, ATfalse);
  gsAFunChoice        = ATmakeAFun("Choice", 2, ATfalse);
  gsAFunMultActName   = ATmakeAFun("MultActName", 1, ATfalse);
  gsAFunRenameExpr    = ATmakeAFun("RenameExpr", 2, ATfalse);
  gsAFunCommExpr      = ATmakeAFun("CommExpr", 2, ATfalse);
  ATprotectAFun(gsAFunSpecV1);
  ATprotectAFun(gsAFunSortSpec);
  ATprotectAFun(gsAFunConsSpec);
  ATprotectAFun(gsAFunMapSpec);
  ATprotectAFun(gsAFunDataEqnSpec);
  ATprotectAFun(gsAFunActSpec);
  ATprotectAFun(gsAFunProcEqnSpec);
  ATprotectAFun(gsAFunInit);
  ATprotectAFun(gsAFunSortId);
  ATprotectAFun(gsAFunSortRef);
  ATprotectAFun(gsAFunOpId);
  ATprotectAFun(gsAFunDataEqn);
  ATprotectAFun(gsAFunDataVarId);
  ATprotectAFun(gsAFunNil);
  ATprotectAFun(gsAFunActId);
  ATprotectAFun(gsAFunProcEqn);
  ATprotectAFun(gsAFunProcVarId);
  ATprotectAFun(gsAFunSortList);
  ATprotectAFun(gsAFunSortSet);
  ATprotectAFun(gsAFunSortBag);
  ATprotectAFun(gsAFunSortStruct);
  ATprotectAFun(gsAFunSortArrowProd);
  ATprotectAFun(gsAFunSortArrow);
  ATprotectAFun(gsAFunStructCons);
  ATprotectAFun(gsAFunStructProj);
  ATprotectAFun(gsAFunDataVarIdOpId);
  ATprotectAFun(gsAFunDataApplProd);
  ATprotectAFun(gsAFunDataAppl);
  ATprotectAFun(gsAFunNumber);
  ATprotectAFun(gsAFunListEnum);
  ATprotectAFun(gsAFunSetEnum);
  ATprotectAFun(gsAFunBagEnum);
  ATprotectAFun(gsAFunSetBagComp);
  ATprotectAFun(gsAFunForall);
  ATprotectAFun(gsAFunExists);
  ATprotectAFun(gsAFunLambda);
  ATprotectAFun(gsAFunWhr);
  ATprotectAFun(gsAFunUnknown);
  ATprotectAFun(gsAFunBagEnumElt);
  ATprotectAFun(gsAFunWhrDecl);
  ATprotectAFun(gsAFunActionProcess);
  ATprotectAFun(gsAFunAction);
  ATprotectAFun(gsAFunProcess);
  ATprotectAFun(gsAFunDelta);
  ATprotectAFun(gsAFunTau);
  ATprotectAFun(gsAFunSum);
  ATprotectAFun(gsAFunRestrict);
  ATprotectAFun(gsAFunAllow);
  ATprotectAFun(gsAFunHide);
  ATprotectAFun(gsAFunRename);
  ATprotectAFun(gsAFunComm);
  ATprotectAFun(gsAFunSync);
  ATprotectAFun(gsAFunAtTime);
  ATprotectAFun(gsAFunSeq);
  ATprotectAFun(gsAFunCond);
  ATprotectAFun(gsAFunBInit);
  ATprotectAFun(gsAFunMerge);
  ATprotectAFun(gsAFunLMerge);
  ATprotectAFun(gsAFunChoice);
  ATprotectAFun(gsAFunMultActName);
  ATprotectAFun(gsAFunRenameExpr);
  ATprotectAFun(gsAFunCommExpr);
  gsConstructorFunctionsEnabled = true;
}

//ATmake extensions

ATermAppl gsString2ATermAppl(char *s)
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();   
  }
}

//Constructor creators

ATermAppl gsMakeSpecV1(
  ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl DataEqnSpec, ATermAppl ActSpec, ATermAppl ProcEqnSpec,
  ATermAppl Init)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl(gsAFunSpecV1, (ATerm) SortSpec, (ATerm) ConsSpec,
    (ATerm) MapSpec, (ATerm) DataEqnSpec, (ATerm) ActSpec, (ATerm) ProcEqnSpec,
    (ATerm) Init);
}

ATermAppl gsMakeSortSpec(ATermList SortDecls)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSpec, (ATerm) SortDecls);
}

ATermAppl gsMakeConsSpec(ATermList OpIds)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunConsSpec, (ATerm) OpIds);
}

ATermAppl gsMakeMapSpec(ATermList OpIds)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMapSpec, (ATerm) OpIds);
}

ATermAppl gsMakeDataEqnSpec(ATermList DataEqns)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunDataEqnSpec, (ATerm) DataEqns);
}

ATermAppl gsMakeActSpec(ATermList ActIds)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunActSpec, (ATerm) ActIds);
}

ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunProcEqnSpec, (ATerm) ProcEqns);
}

ATermAppl gsMakeInit(ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunInit, (ATerm) ProcExpr);
}

ATermAppl gsMakeSortId(ATermAppl Name)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortId, (ATerm) Name);
}

ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortRef, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunOpId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl DataExprCond,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunDataEqn, (ATerm) DataVarIds, (ATerm) DataExprCond,
    (ATerm) DataExprLHS, (ATerm) DataExprRHS);
}

ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataVarId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeNil()
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunNil);
}

ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActId, (ATerm) Name, (ATerm) SortExprs);
}

ATermAppl gsMakeProcEqn(ATermAppl ProcVarId, ATermList DataVarIds,
  ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunProcEqn, (ATerm) ProcVarId, (ATerm) DataVarIds,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeProcVarId(ATermAppl Name, ATermList SortExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcVarId, (ATerm) Name, (ATerm) SortExprs);
}

ATermAppl gsMakeSortList(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortList, (ATerm) SortExpr);
}

ATermAppl gsMakeSortSet(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSet, (ATerm) SortExpr);
}

ATermAppl gsMakeSortBag(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortBag, (ATerm) SortExpr);
}

ATermAppl gsMakeSortStruct(ATermList StructConss)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortStruct, (ATerm) StructConss);
}

ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrowProd, (ATerm) SortExprsDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrow, (ATerm) SortExprDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecName)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStructCons, (ATerm) ConsName, (ATerm) StructProjs,
    (ATerm) RecName);
}

ATermAppl gsMakeStructProj(ATermAppl ProjName, ATermList SortExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStructProj, (ATerm) ProjName, (ATerm) SortExprs);
}

ATermAppl gsMakeDataVarIdOpId(ATermAppl Name)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunDataVarIdOpId, (ATerm) Name);
}

ATermAppl gsMakeDataApplProd(ATermAppl DataExpr, ATermList DataArgs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataApplProd, (ATerm) DataExpr, (ATerm) DataArgs);
}

ATermAppl gsMakeDataAppl(ATermAppl DataExpr, ATermAppl DataArg)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataAppl, (ATerm) DataExpr, (ATerm) DataArg);
}

ATermAppl gsMakeNumber(ATermAppl Number, ATermAppl SortExprOrUnknown)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunNumber, (ATerm) Number, (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeListEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunListEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeBagEnum(ATermList BagEnumElts, ATermAppl SortExprOrUnknown)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnum, (ATerm) BagEnumElts,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetBagComp(ATermAppl DataVarId, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetBagComp, (ATerm) DataVarId, (ATerm) DataExpr);
}

ATermAppl gsMakeForall(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunForall, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeExists(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunExists, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeLambda(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLambda, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhr, (ATerm) DataExpr, (ATerm) WhrDecls);
}

ATermAppl gsMakeUnknown()
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunUnknown);
}

ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnumElt, (ATerm) DataExpr, (ATerm) Multiplicity);
}

ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhrDecl, (ATerm) Name, (ATerm) DataExpr);
}

ATermAppl gsMakeActionProcess(ATermAppl Name, ATermList DataExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActionProcess, (ATerm) Name, (ATerm) DataExprs);
}

ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAction, (ATerm) ActId, (ATerm) DataExprs);
}

ATermAppl gsMakeProcess(ATermAppl ProcId, ATermList DataExprs)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcess, (ATerm) ProcId, (ATerm) DataExprs);
}

ATermAppl gsMakeDelta()
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunDelta);
}

ATermAppl gsMakeTau()
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunTau);
}

ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSum, (ATerm) DataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeRestrict(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRestrict, (ATerm) MultActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAllow, (ATerm) MultActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeHide(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunHide, (ATerm) MultActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeRename(ATermList RenameExprSet, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRename, (ATerm) RenameExprSet, (ATerm) ProcExpr);
}

ATermAppl gsMakeComm(ATermList CommExprSet, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunComm, (ATerm) CommExprSet, (ATerm) ProcExpr);
}

ATermAppl gsMakeSync(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSync, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl DataExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAtTime, (ATerm) ProcExpr, (ATerm) DataExpr);
}

ATermAppl gsMakeSeq(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSeq, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeCond(ATermAppl DataExpr, ATermAppl PEThen, ATermAppl PEElse)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunCond, (ATerm) DataExpr, (ATerm) PEThen,
    (ATerm) PEElse);
}

ATermAppl gsMakeBInit(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBInit, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeLMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeChoice(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunChoice, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeMultActName(ATermList ActNames)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMultActName, (ATerm) ActNames);
}

ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRenameExpr, (ATerm) FromName, (ATerm) ToName);
}

ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActName)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunCommExpr, (ATerm) MultActName, (ATerm) ActName);
}

//Constructor recognisers

bool gsIsSpecV1(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSpecV1;
}

bool gsIsSortSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortSpec;
}

bool gsIsConsSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunConsSpec;
}

bool gsIsMapSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunMapSpec;
}

bool gsIsDataEqnSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataEqnSpec;
}

bool gsIsActSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunActSpec;
}

bool gsIsProcEqnSpec(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunProcEqnSpec;
}

bool gsIsInit(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunInit;
}

bool gsIsSortId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortId;
}

bool gsIsSortRef(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortRef;
}

bool gsIsOpId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunOpId;
}

bool gsIsDataEqn(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataEqn;
}

bool gsIsDataVarId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataVarId;
}

bool gsIsNil(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunNil;
}

bool gsIsActId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunActId;
}

bool gsIsProcEqn(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunProcEqn;
}

bool gsIsProcVarId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunProcVarId;
}

bool gsIsSortList(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortList;
}

bool gsIsSortSet(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortSet;
}

bool gsIsSortBag(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortBag;
}

bool gsIsSortStruct(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortStruct;
}

bool gsIsSortArrowProd(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortArrowProd;
}

bool gsIsSortArrow(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSortArrow;
}

bool gsIsStructCons(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunStructCons;
}

bool gsIsStructProj(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunStructProj;
}

bool gsIsDataVarIdOpId(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataVarIdOpId;
}

bool gsIsDataApplProd(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataApplProd;
}

bool gsIsDataAppl(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDataAppl;
}

bool gsIsNumber(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunNumber;
}

bool gsIsListEnum(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunListEnum;
}

bool gsIsSetEnum(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSetEnum;
}

bool gsIsBagEnum(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunBagEnum;
}

bool gsIsSetBagComp(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSetBagComp;
}

bool gsIsForall(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunForall;
}

bool gsIsExists(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunExists;
}

bool gsIsLambda(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunLambda;
}

bool gsIsWhr(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunWhr;
}

bool gsIsUnknown(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunUnknown;
}

bool gsIsBagEnumElt(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunBagEnumElt;
}

bool gsIsWhrDecl(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunWhrDecl;
}

bool gsIsActionProcess(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunActionProcess;
}

bool gsIsAction(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunAction;
}

bool gsIsProcess(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunProcess;
}

bool gsIsDelta(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunDelta;
}

bool gsIsTau(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunTau;
}

bool gsIsSum(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSum;
}

bool gsIsRestrict(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunRestrict;
}

bool gsIsAllow(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunAllow;
}

bool gsIsHide(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunHide;
}

bool gsIsRename(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunRename;
}

bool gsIsComm(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunComm;
}

bool gsIsSync(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSync;
}

bool gsIsAtTime(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunAtTime;
}

bool gsIsSeq(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunSeq;
}

bool gsIsCond(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunCond;
}

bool gsIsBInit(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunBInit;
}

bool gsIsMerge(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunMerge;
}

bool gsIsLMerge(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunLMerge;
}

bool gsIsChoice(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunChoice;
}

bool gsIsMultActName(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunMultActName;
}

bool gsIsRenameExpr(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunRenameExpr;
}

bool gsIsCommExpr(ATermAppl term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunCommExpr;
}
