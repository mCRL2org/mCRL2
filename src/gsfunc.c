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

//String manipulation
//-------------------

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

//Message printing
//----------------

static bool gsWarning = true; //indicates if warning should be printed
static bool gsVerbose = false;//indicates if verbose messages should be printed
static bool gsDebug   = false;//indicates if debug messages should be printed

void gsSetQuietMsg(void)
{
  gsWarning = false;
  gsVerbose = false;
  gsDebug   = false;
}

void gsSetNormalMsg(void)
{
  gsWarning = true;
  gsVerbose = false;
  gsDebug   = false;
}

void gsSetVerboseMsg(void)
{
  gsWarning = true;
  gsVerbose = true;
  gsDebug   = false;
}

void gsSetDebugMsg(void)
{
  gsWarning = true;
  gsVerbose = true;
  gsDebug   = true;
}

void gsErrorMsg(char *Format, ...)
{
  fprintf(stderr, "error: ");
  va_list Args;
  va_start(Args, Format);
  ATvfprintf(stderr, Format, Args);
  va_end(Args);
}

void gsWarningMsg(char *Format, ...)
{
  if (gsWarning) {
    fprintf(stderr, "warning: ");
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

void gsVerboseMsg(char *Format, ...)
{
  if (gsVerbose) {
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

void gsDebugMsgFunc(const char *FuncName, char *Format, ...)
{
  if (gsDebug) {
    fprintf(stderr, "(%s): ", FuncName);
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

//ATerm libary work arounds
//-------------------------

ATermAppl ATAelementAt(ATermList List, int Index)
{
  return (ATermAppl) ATelementAt(List, Index);
}

ATermList ATLelementAt(ATermList List, int Index)
{
  return (ATermList) ATelementAt(List, Index);
}

ATermAppl ATAgetArgument(ATermAppl Appl, int Nr)
{
  return (ATermAppl) ATgetArgument(Appl, Nr);
}

ATermList ATLgetArgument(ATermAppl Appl, int Nr)
{
  return (ATermList) ATgetArgument(Appl, Nr);
}

ATermAppl ATAgetFirst(ATermList List)
{
  return (ATermAppl) ATgetFirst(List);
}

ATermList ATLgetFirst(ATermList List)
{
  return (ATermList) ATgetFirst(List);
}

void ATprotectAppl(ATermAppl *PAppl)
{
  ATprotect((ATerm *) PAppl);
}

void ATprotectList(ATermAppl *PList)
{
  ATprotect((ATerm *) PList);
}

void ATunprotectAppl(ATermAppl *PAppl)
{
  ATunprotect((ATerm *) PAppl);
}

void ATunprotectList(ATermAppl *PList)
{
  ATunprotect((ATerm *) PList);
}

//Functions for the internal ATerm structure
//------------------------------------------

ATermAppl gsString2ATermAppl(char *s)
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();   
  }
}

char *gsATermAppl2String(ATermAppl term)
{
  AFun head = ATgetAFun(term);
  if ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue)) {
    return ATgetName(head);
  } else {
    return NULL;
  }
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

//Constant ATermAppl's for each sort system identifier name
static ATermAppl gsSortIdNameBool;
static ATermAppl gsSortIdNamePos;
static ATermAppl gsSortIdNameNat;
static ATermAppl gsSortIdNameInt;

//Constant ATermAppl's for each operation system identifier name
static ATermAppl gsOpIdNameTrue;
static ATermAppl gsOpIdNameFalse;
static ATermAppl gsOpIdNameNot;
static ATermAppl gsOpIdNameAnd;
static ATermAppl gsOpIdNameOr;
static ATermAppl gsOpIdNameImp;
static ATermAppl gsOpIdNameEq;
static ATermAppl gsOpIdNameNeq;
static ATermAppl gsOpIdNameIf;
static ATermAppl gsOpIdNameForall;
static ATermAppl gsOpIdNameExists;
static ATermAppl gsOpIdName1;
static ATermAppl gsOpIdNameDot0;
static ATermAppl gsOpIdNameDot1;
static ATermAppl gsOpIdName0;
static ATermAppl gsOpIdNamePosAsNat;
static ATermAppl gsOpIdNameNegatePos;
static ATermAppl gsOpIdNameNatAsInt;
static ATermAppl gsOpIdNamePos2Nat;
static ATermAppl gsOpIdNamePos2Int;
static ATermAppl gsOpIdNameNat2Pos;
static ATermAppl gsOpIdNameNat2Int;
static ATermAppl gsOpIdNameInt2Pos;
static ATermAppl gsOpIdNameInt2Nat;
static ATermAppl gsOpIdNameLT;
static ATermAppl gsOpIdNameGT;
static ATermAppl gsOpIdNameLTE;
static ATermAppl gsOpIdNameGTE;
static ATermAppl gsOpIdNameMax;
static ATermAppl gsOpIdNameMin;
static ATermAppl gsOpIdNameAbs;
static ATermAppl gsOpIdNameNeg;
static ATermAppl gsOpIdNameSucc;
static ATermAppl gsOpIdNamePred;
static ATermAppl gsOpIdNamePredPos;
static ATermAppl gsOpIdNameAdd;
static ATermAppl gsOpIdNameSubt;
static ATermAppl gsOpIdNameSubtAux;
static ATermAppl gsOpIdNameDouble;
static ATermAppl gsOpIdNameMult;
static ATermAppl gsOpIdNameDiv;
static ATermAppl gsOpIdNameMod;
static ATermAppl gsOpIdNameExp;

//Indicates if gsEnableConstructorFunctions has been called
static bool gsConstructorFunctionsEnabled = false;

//Enable constructor functions
void gsEnableConstructorFunctions(void)
{
  //create constructor AFun's
  gsAFunSpecV1               = ATmakeAFun("SpecV1", 7, ATfalse);
  gsAFunSortSpec             = ATmakeAFun("SortSpec", 1, ATfalse);
  gsAFunConsSpec             = ATmakeAFun("ConsSpec", 1, ATfalse);
  gsAFunMapSpec              = ATmakeAFun("MapSpec", 1, ATfalse);
  gsAFunDataEqnSpec          = ATmakeAFun("DataEqnSpec", 1, ATfalse);
  gsAFunActSpec              = ATmakeAFun("ActSpec", 1, ATfalse);
  gsAFunProcEqnSpec          = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
  gsAFunInit                 = ATmakeAFun("Init", 1, ATfalse);
  gsAFunSortId               = ATmakeAFun("SortId", 1, ATfalse);
  gsAFunSortRef              = ATmakeAFun("SortRef", 2, ATfalse);
  gsAFunOpId                 = ATmakeAFun("OpId", 2, ATfalse);
  gsAFunDataEqn              = ATmakeAFun("DataEqn", 4, ATfalse);
  gsAFunDataVarId            = ATmakeAFun("DataVarId", 2, ATfalse);
  gsAFunNil                  = ATmakeAFun("Nil", 0, ATfalse);
  gsAFunActId                = ATmakeAFun("ActId", 2, ATfalse);
  gsAFunProcEqn              = ATmakeAFun("ProcEqn", 3, ATfalse);
  gsAFunProcVarId            = ATmakeAFun("ProcVarId", 2, ATfalse);
  gsAFunSortList             = ATmakeAFun("SortList", 1, ATfalse);
  gsAFunSortSet              = ATmakeAFun("SortSet", 1, ATfalse);
  gsAFunSortBag              = ATmakeAFun("SortBag", 1, ATfalse);
  gsAFunSortStruct           = ATmakeAFun("SortStruct", 1, ATfalse);
  gsAFunSortArrowProd        = ATmakeAFun("SortArrowProd", 2, ATfalse);
  gsAFunSortArrow            = ATmakeAFun("SortArrow", 2, ATfalse);
  gsAFunStructCons           = ATmakeAFun("StructCons", 3, ATfalse);
  gsAFunStructProj           = ATmakeAFun("StructProj", 2, ATfalse);
  gsAFunDataVarIdOpId        = ATmakeAFun("DataVarIdOpId", 1, ATfalse);
  gsAFunDataApplProd         = ATmakeAFun("DataApplProd", 2, ATfalse);
  gsAFunDataAppl             = ATmakeAFun("DataAppl", 2, ATfalse);
  gsAFunNumber               = ATmakeAFun("Number", 2, ATfalse);
  gsAFunListEnum             = ATmakeAFun("ListEnum", 2, ATfalse);
  gsAFunSetEnum              = ATmakeAFun("SetEnum", 2, ATfalse);
  gsAFunBagEnum              = ATmakeAFun("BagEnum", 2, ATfalse);
  gsAFunSetBagComp           = ATmakeAFun("SetBagComp", 2, ATfalse);
  gsAFunForall               = ATmakeAFun("Forall", 2, ATfalse);
  gsAFunExists               = ATmakeAFun("Exists", 2, ATfalse);
  gsAFunLambda               = ATmakeAFun("Lambda", 2, ATfalse);
  gsAFunWhr                  = ATmakeAFun("Whr", 2, ATfalse);
  gsAFunUnknown              = ATmakeAFun("Unknown", 0, ATfalse);
  gsAFunBagEnumElt           = ATmakeAFun("BagEnumElt", 2, ATfalse);
  gsAFunWhrDecl              = ATmakeAFun("WhrDecl", 2, ATfalse);
  gsAFunActionProcess        = ATmakeAFun("ActionProcess", 2, ATfalse);
  gsAFunAction               = ATmakeAFun("Action", 2, ATfalse);
  gsAFunProcess              = ATmakeAFun("Process", 2, ATfalse);
  gsAFunDelta                = ATmakeAFun("Delta", 0, ATfalse);
  gsAFunTau                  = ATmakeAFun("Tau", 0, ATfalse);
  gsAFunSum                  = ATmakeAFun("Sum", 2, ATfalse);
  gsAFunRestrict             = ATmakeAFun("Restrict", 2, ATfalse);
  gsAFunAllow                = ATmakeAFun("Allow", 2, ATfalse);
  gsAFunHide                 = ATmakeAFun("Hide", 2, ATfalse);
  gsAFunRename               = ATmakeAFun("Rename", 2, ATfalse);
  gsAFunComm                 = ATmakeAFun("Comm", 2, ATfalse);
  gsAFunSync                 = ATmakeAFun("Sync", 2, ATfalse);
  gsAFunAtTime               = ATmakeAFun("AtTime", 2, ATfalse);
  gsAFunSeq                  = ATmakeAFun("Seq", 2, ATfalse);
  gsAFunCond                 = ATmakeAFun("Cond", 3, ATfalse);
  gsAFunBInit                = ATmakeAFun("BInit", 2, ATfalse);
  gsAFunMerge                = ATmakeAFun("Merge", 2, ATfalse);
  gsAFunLMerge               = ATmakeAFun("LMerge", 2, ATfalse);
  gsAFunChoice               = ATmakeAFun("Choice", 2, ATfalse);
  gsAFunMultActName          = ATmakeAFun("MultActName", 1, ATfalse);
  gsAFunRenameExpr           = ATmakeAFun("RenameExpr", 2, ATfalse);
  gsAFunCommExpr             = ATmakeAFun("CommExpr", 2, ATfalse);
  //create sort system identifier names
  gsSortIdNameBool           = gsString2ATermAppl("Bool");
  gsSortIdNamePos            = gsString2ATermAppl("Pos");
  gsSortIdNameNat            = gsString2ATermAppl("Nat");
  gsSortIdNameInt            = gsString2ATermAppl("Int");
  //create operation system identifier names
  gsOpIdNameTrue             = gsString2ATermAppl("true");
  gsOpIdNameFalse            = gsString2ATermAppl("false");
  gsOpIdNameNot              = gsString2ATermAppl("!");
  gsOpIdNameAnd              = gsString2ATermAppl("&&");
  gsOpIdNameOr               = gsString2ATermAppl("||");
  gsOpIdNameImp              = gsString2ATermAppl("=>");
  gsOpIdNameEq               = gsString2ATermAppl("==");
  gsOpIdNameNeq              = gsString2ATermAppl("!=");
  gsOpIdNameIf               = gsString2ATermAppl("if");
  gsOpIdNameForall           = gsString2ATermAppl("forall");
  gsOpIdNameExists           = gsString2ATermAppl("exists");
  gsOpIdName1              = gsString2ATermAppl("@1");
  gsOpIdNameDot0        = gsString2ATermAppl("@.0");
  gsOpIdNameDot1 = gsString2ATermAppl("@.1");
  gsOpIdName0             = gsString2ATermAppl("@0");
  gsOpIdNamePosAsNat         = gsString2ATermAppl("@cPosNat");
  gsOpIdNameNegatePos        = gsString2ATermAppl("@-");
  gsOpIdNameNatAsInt         = gsString2ATermAppl("@cNatInt");
  gsOpIdNamePos2Nat          = gsString2ATermAppl("Pos2Nat");
  gsOpIdNamePos2Int          = gsString2ATermAppl("Pos2Int");
  gsOpIdNameNat2Pos          = gsString2ATermAppl("Nat2Pos");
  gsOpIdNameNat2Int          = gsString2ATermAppl("Nat2Int");
  gsOpIdNameInt2Pos          = gsString2ATermAppl("Int2Pos");
  gsOpIdNameInt2Nat          = gsString2ATermAppl("Int2Nat");
  gsOpIdNameLT               = gsString2ATermAppl("<");
  gsOpIdNameGT               = gsString2ATermAppl(">");
  gsOpIdNameLTE              = gsString2ATermAppl("<=");
  gsOpIdNameGTE              = gsString2ATermAppl(">=");
  gsOpIdNameMax              = gsString2ATermAppl("max");
  gsOpIdNameMin              = gsString2ATermAppl("min");
  gsOpIdNameAbs              = gsString2ATermAppl("abs");
  gsOpIdNameNeg              = gsString2ATermAppl("-");
  gsOpIdNameSucc             = gsString2ATermAppl("succ");
  gsOpIdNamePred             = gsString2ATermAppl("pred");
  gsOpIdNamePredPos          = gsString2ATermAppl("@pred");
  gsOpIdNameAdd              = gsString2ATermAppl("+");
  gsOpIdNameSubt             = gsString2ATermAppl("-");
  gsOpIdNameSubtAux          = gsString2ATermAppl("@-");
  gsOpIdNameDouble           = gsString2ATermAppl("@double");
  gsOpIdNameMult             = gsString2ATermAppl("*");
  gsOpIdNameDiv              = gsString2ATermAppl("div");
  gsOpIdNameMod              = gsString2ATermAppl("mod");
  gsOpIdNameExp              = gsString2ATermAppl("exp");
  //protect constructor AFun's
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
  //protect sort system identifier names
  ATprotectAppl(&gsSortIdNameBool);
  ATprotectAppl(&gsSortIdNamePos);
  ATprotectAppl(&gsSortIdNameNat);
  ATprotectAppl(&gsSortIdNameInt);
  //protect operation system identifier names
  ATprotectAppl(&gsOpIdNameTrue);
  ATprotectAppl(&gsOpIdNameFalse);
  ATprotectAppl(&gsOpIdNameNot);
  ATprotectAppl(&gsOpIdNameAnd);
  ATprotectAppl(&gsOpIdNameOr);
  ATprotectAppl(&gsOpIdNameImp);
  ATprotectAppl(&gsOpIdNameEq);
  ATprotectAppl(&gsOpIdNameNeq);
  ATprotectAppl(&gsOpIdNameIf);
  ATprotectAppl(&gsOpIdNameForall);
  ATprotectAppl(&gsOpIdNameExists);
  ATprotectAppl(&gsOpIdName1);
  ATprotectAppl(&gsOpIdNameDot0);
  ATprotectAppl(&gsOpIdNameDot1);
  ATprotectAppl(&gsOpIdName0);
  ATprotectAppl(&gsOpIdNamePosAsNat);
  ATprotectAppl(&gsOpIdNameNegatePos);
  ATprotectAppl(&gsOpIdNameNatAsInt);
  ATprotectAppl(&gsOpIdNamePos2Nat);
  ATprotectAppl(&gsOpIdNamePos2Int);
  ATprotectAppl(&gsOpIdNameNat2Pos);
  ATprotectAppl(&gsOpIdNameNat2Int);
  ATprotectAppl(&gsOpIdNameInt2Pos);
  ATprotectAppl(&gsOpIdNameInt2Nat);
  ATprotectAppl(&gsOpIdNameLT);
  ATprotectAppl(&gsOpIdNameGT);
  ATprotectAppl(&gsOpIdNameLTE);
  ATprotectAppl(&gsOpIdNameGTE);
  ATprotectAppl(&gsOpIdNameMax);
  ATprotectAppl(&gsOpIdNameMin);
  ATprotectAppl(&gsOpIdNameAbs);
  ATprotectAppl(&gsOpIdNameNeg);
  ATprotectAppl(&gsOpIdNameSucc);
  ATprotectAppl(&gsOpIdNamePred);
  ATprotectAppl(&gsOpIdNamePredPos);
  ATprotectAppl(&gsOpIdNameAdd);
  ATprotectAppl(&gsOpIdNameSubt);
  ATprotectAppl(&gsOpIdNameSubtAux);
  ATprotectAppl(&gsOpIdNameDouble);
  ATprotectAppl(&gsOpIdNameMult);
  ATprotectAppl(&gsOpIdNameDiv);
  ATprotectAppl(&gsOpIdNameMod);
  ATprotectAppl(&gsOpIdNameExp);
  gsConstructorFunctionsEnabled = true;
}

//Creation of all constructor elements of the internal ATerm structure.
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

ATermAppl gsMakeRestrict(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRestrict, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeHide(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunHide, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeRename(ATermList RenameExprs, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRename, (ATerm) RenameExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeComm(ATermList CommExprs, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunComm, (ATerm) CommExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAllow, (ATerm) MultActNames, (ATerm) ProcExpr);
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

//Recognisers of all constructor elements of the internal ATerm structure.
bool gsIsSpecV1(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSpecV1;
}

bool gsIsSortSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSpec;
}

bool gsIsConsSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunConsSpec;
}

bool gsIsMapSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMapSpec;
}

bool gsIsDataEqnSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqnSpec;
}

bool gsIsActSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActSpec;
}

bool gsIsProcEqnSpec(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqnSpec;
}

bool gsIsInit(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunInit;
}

bool gsIsSortId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortId;
}

bool gsIsSortRef(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortRef;
}

bool gsIsOpId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunOpId;
}

bool gsIsDataEqn(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqn;
}

bool gsIsDataVarId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarId;
}

bool gsIsNil(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNil;
}

bool gsIsActId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActId;
}

bool gsIsProcEqn(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqn;
}

bool gsIsProcVarId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcVarId;
}

bool gsIsSortList(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortList;
}

bool gsIsSortSet(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSet;
}

bool gsIsSortBag(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortBag;
}

bool gsIsSortStruct(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortStruct;
}

bool gsIsSortArrowProd(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrowProd;
}

bool gsIsSortArrow(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrow;
}

bool gsIsStructCons(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructCons;
}

bool gsIsStructProj(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructProj;
}

bool gsIsDataVarIdOpId(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarIdOpId;
}

bool gsIsDataApplProd(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataApplProd;
}

bool gsIsDataAppl(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataAppl;
}

bool gsIsNumber(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNumber;
}

bool gsIsListEnum(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunListEnum;
}

bool gsIsSetEnum(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetEnum;
}

bool gsIsBagEnum(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnum;
}

bool gsIsSetBagComp(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetBagComp;
}

bool gsIsForall(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunForall;
}

bool gsIsExists(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunExists;
}

bool gsIsLambda(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLambda;
}

bool gsIsWhr(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhr;
}

bool gsIsUnknown(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunUnknown;
}

bool gsIsBagEnumElt(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnumElt;
}

bool gsIsWhrDecl(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhrDecl;
}

bool gsIsActionProcess(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActionProcess;
}

bool gsIsAction(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAction;
}

bool gsIsProcess(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcess;
}

bool gsIsDelta(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDelta;
}

bool gsIsTau(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunTau;
}

bool gsIsSum(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSum;
}

bool gsIsRestrict(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRestrict;
}

bool gsIsHide(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunHide;
}

bool gsIsRename(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRename;
}

bool gsIsComm(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunComm;
}

bool gsIsAllow(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAllow;
}

bool gsIsSync(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSync;
}

bool gsIsAtTime(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAtTime;
}

bool gsIsSeq(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSeq;
}

bool gsIsCond(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunCond;
}

bool gsIsBInit(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBInit;
}

bool gsIsMerge(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMerge;
}

bool gsIsLMerge(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLMerge;
}

bool gsIsChoice(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunChoice;
}

bool gsIsMultActName(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMultActName;
}

bool gsIsRenameExpr(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRenameExpr;
}

bool gsIsCommExpr(ATermAppl Term) {
  assert(gsConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunCommExpr;
}


//Creation of sort identifiers for system defined sorts.
ATermAppl gsMakeSortIdBool()
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameBool);
}

ATermAppl gsMakeSortIdPos()
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNamePos);
}

ATermAppl gsMakeSortIdNat()
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameNat);
}

ATermAppl gsMakeSortIdInt()
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameInt);
}


//Creation of sort expressions for system defined sorts.
ATermAppl gsMakeSortExprBool()
{
  return gsMakeSortIdBool();
}

ATermAppl gsMakeSortExprPos()
{
  return gsMakeSortIdPos();
}

ATermAppl gsMakeSortExprNat()
{
  return gsMakeSortIdNat();
}

ATermAppl gsMakeSortExprInt()
{
  return gsMakeSortIdInt();
}


//Auxiliary functions concerning sort expressions
ATermAppl gsMakeSortArrow2(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprResult)
{
  return gsMakeSortArrowList(
    ATmakeList2((ATerm) SortExprDom1, (ATerm) SortExprDom2),
    SortExprResult);
}

ATermAppl gsMakeSortArrow3(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprResult)
{
  return gsMakeSortArrowList(
    ATmakeList3((ATerm) SortExprDom1, (ATerm) SortExprDom2,
      (ATerm) SortExprDom3),
    SortExprResult);
}

ATermAppl gsMakeSortArrowList(ATermList SortExprDomain,
  ATermAppl SortExprResult)
{
  ATermAppl Result = SortExprResult;
  int n = ATgetLength(SortExprDomain);
  for (int i = n-1; i >= 0; i--) {
    Result = gsMakeSortArrow(ATAelementAt(SortExprDomain, i), Result);
  }
  return Result;
}

ATermAppl gsGetSort(ATermAppl DataExpr)
{
  if (gsIsDataAppl(DataExpr)) {
    ATermAppl HeadSort = gsGetSort(ATAgetArgument(DataExpr, 0));
    if (gsIsSortArrow(HeadSort)) {
      ATermAppl ResultSort = gsGetSort(ATAgetArgument(DataExpr, 1));
      if (!gsIsUnknown(ResultSort) &&
          ATisEqual(ResultSort, ATAgetArgument(HeadSort, 0))) {
        return ATAgetArgument(HeadSort, 1);
      } else {
        return gsMakeSortArrow(HeadSort, ResultSort);
      }
    } else {
      return gsMakeUnknown();
    }
  } else if (gsIsDataVarId(DataExpr) || gsIsOpId(DataExpr)) {
    return ATAgetArgument(DataExpr, 1);
  } else {
    return gsMakeUnknown();
  }
}


//Creation of operation identifiers for system defined operations.
ATermAppl gsMakeOpIdTrue(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameTrue, gsMakeSortExprBool());
} 

ATermAppl gsMakeOpIdFalse(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameFalse, gsMakeSortExprBool());
} 

ATermAppl gsMakeOpIdNot(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNot,
    gsMakeSortArrow(gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdAnd(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameAnd, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdOr(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameOr, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdImp(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameImp, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdEq(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEq, gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdNeq(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNeq, gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdIf(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameIf, gsMakeSortArrow3(
    gsMakeSortExprBool(), SortExpr, SortExpr, SortExpr));
} 

ATermAppl gsMakeOpIdForall(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameForall, gsMakeSortArrow(
    gsMakeSortArrow(SortExpr, gsMakeSortExprBool()), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdExists(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameExists, gsMakeSortArrow(
    gsMakeSortArrow(SortExpr, gsMakeSortExprBool()), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpId1(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdName1, gsMakeSortExprPos());
} 

ATermAppl gsMakeOpIdDot0(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameDot0,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdDot1(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameDot1,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpId0(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdName1, gsMakeSortExprNat());
} 

ATermAppl gsMakeOpIdPosAsNat(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePosAsNat,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdNegatePos(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNegatePos,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdNatAsInt(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNatAsInt,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdPos2Nat(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePos2Nat,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdPos2Int(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePos2Int,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdNat2Pos(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNat2Pos,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdNat2Int(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNat2Int,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdInt2Pos(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameInt2Pos,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdInt2Nat(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameInt2Nat,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdLT(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameLT,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGT(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameGT,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdLTE(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameLTE,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGTE(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameGTE,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdMax(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameMax,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdMin(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameMin,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdAbs(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameAbs,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdNeg(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNeg,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdSucc(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  ATermAppl SuccSort;
  if ATisEqual(SortExpr, gsMakeSortExprNat()) {
    SuccSort = gsMakeSortArrow(SortExpr, gsMakeSortExprPos());
  } else {
    SuccSort = gsMakeSortArrow(SortExpr, SortExpr);
  }    
  return gsMakeOpId(gsOpIdNameSucc, SuccSort);
}

ATermAppl gsMakeOpIdPred(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  ATermAppl PredSort;
  if ATisEqual(SortExpr, gsMakeSortExprPos()) {
    PredSort = gsMakeSortArrow(SortExpr, gsMakeSortExprNat());
  } else {
    PredSort = gsMakeSortArrow(SortExpr, SortExpr);
  }    
  return gsMakeOpId(gsOpIdNamePred, PredSort);
}

ATermAppl gsMakeOpIdPredPos(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePredPos,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdAdd(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameAdd,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSubt(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubt, gsMakeSortArrow2(
    gsMakeSortExprInt(), gsMakeSortExprInt(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdSubtAux(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubtAux, gsMakeSortArrow2(
    gsMakeSortExprPos(), gsMakeSortExprPos(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdDouble(void)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameDouble, gsMakeSortArrow(
    gsMakeSortExprInt(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdMult(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameMult,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdDiv(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameDiv,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(), SortExpr));
}

ATermAppl gsMakeOpIdMod(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameMod,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdExp(ATermAppl SortExpr)
{
  assert(gsConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameExp,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprNat(), SortExpr));
}


//Creation of data expressions for system defined operations.
ATermAppl gsMakeDataExprTrue(void)
{
  return gsMakeOpIdTrue();
}

ATermAppl gsMakeDataExprFalse(void)
{
  return gsMakeOpIdFalse();
}

ATermAppl gsMakeDataExprNot(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNot(), DataExpr);
}

ATermAppl gsMakeDataExprAnd(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdAnd(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprOr(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdOr(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprImp(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdImp(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS)))
  {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdEq(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprNeq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS)))
  {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdNeq(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprIf(ATermAppl DataExprCond, ATermAppl DataExprThen,
  ATermAppl DataExprElse)
{
  ATermAppl Result = NULL;
  if (!ATisEqual(gsGetSort(DataExprCond), gsMakeSortIdBool())) {
    ThrowVM(NULL, "data expression %t should be of sort %t", DataExprCond,
      gsMakeSortIdBool());
  }
  ATermAppl ExprSort = gsGetSort(DataExprThen);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprThen);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprElse)))
  {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprElse), DataExprElse);
  }   
  Result = gsMakeDataAppl3(gsMakeOpIdIf(ExprSort), DataExprCond, DataExprThen,
    DataExprElse);
finally:
  return Result;
}

ATermAppl gsMakeDataExpr1(void)
{
  return gsMakeOpId1();
}

ATermAppl gsMakeDataExprDot0(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdDot0(), DataExpr);
}

ATermAppl gsMakeDataExprDot1(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdDot1(), DataExpr);
}

ATermAppl gsMakeDataExpr0(void)
{
  return gsMakeOpId0();
}

ATermAppl gsMakeDataExprPosAsNat(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdPosAsNat(), DataExpr);
}

ATermAppl gsMakeDataExprNegatePos(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNegatePos(), DataExpr);
}

ATermAppl gsMakeDataExprNatAsInt(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNatAsInt(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Nat(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdPos2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Int(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdPos2Int(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Pos(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNat2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Int(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNat2Int(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Pos(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdInt2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Nat(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdInt2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprLT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdLT(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprGT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdGT(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprLTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdLTE(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprGTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdGTE(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprMax(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdMax(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprMin(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdMin(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprAbs(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdAbs(), DataExpr);
}

ATermAppl gsMakeDataExprNeg(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNeg(), DataExpr);
}

ATermAppl gsMakeDataExprSucc(ATermAppl DataExpr)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExpr);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExpr);
  }
  Result = gsMakeDataAppl(gsMakeOpIdSucc(ExprSort), DataExpr);
finally:
  return Result;
}

ATermAppl gsMakeDataExprPred(ATermAppl DataExpr)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExpr);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExpr);
  }
  Result = gsMakeDataAppl(gsMakeOpIdPred(ExprSort), DataExpr);
finally:
  return Result;
}

ATermAppl gsMakeDataExprPredPos(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdPredPos(), DataExpr);
}

ATermAppl gsMakeDataExprAdd(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdAdd(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprSubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdSubt(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSubtAux(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(gsMakeOpIdSubtAux(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprDouble(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdDouble(), DataExpr);
}

ATermAppl gsMakeDataExprMult(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  if (!ATisEqual(ExprSort, gsGetSort(DataExprRHS))) {
    ThrowVM(NULL, "expected sort %t instead of %t for data expression %t",
      ExprSort, gsGetSort(DataExprRHS), DataExprLHS);
  }   
  Result = gsMakeDataAppl2(gsMakeOpIdMult(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprDiv(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  Result = gsMakeDataAppl2(gsMakeOpIdDiv(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  Result = gsMakeDataAppl2(gsMakeOpIdMod(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}

ATermAppl gsMakeDataExprExp(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl Result = NULL;
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  if (gsIsUnknown(ExprSort)) {
    ThrowVM(NULL, "sort of data expression %t is unknown", DataExprLHS);
  }
  Result = gsMakeDataAppl2(gsMakeOpIdExp(ExprSort), DataExprLHS, DataExprRHS);
finally:
  return Result;
}


//Auxiliary functions to create data expressions
ATermAppl gsMakeDataAppl2(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2)
{
  return gsMakeDataApplList(DataExpr,
    ATmakeList2((ATerm) DataExprArg1, (ATerm) DataExprArg2));
}

ATermAppl gsMakeDataAppl3(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3)
{
  return gsMakeDataApplList(DataExpr,
    ATmakeList3((ATerm) DataExprArg1, (ATerm) DataExprArg2,
      (ATerm) DataExprArg3));
}

ATermAppl gsMakeDataApplList(ATermAppl DataExpr,
  ATermList DataExprArgs)
{
  ATermAppl Result = DataExpr;
  int n = ATgetLength(DataExprArgs);
  for (int i = 0; i < n; i++) {
    Result = gsMakeDataAppl(Result, ATAelementAt(DataExprArgs, i));
  }
  return Result;
}

inline int gsChar2Int(char c)
//Pre: c is in {'0', ..., '9'}
//Ret: integer value corresponding to c
{
  assert(c >= '0' && c <= '9');
  return c - '0';
}

inline char gsInt2Char(int n)
//Pre: 0 <= n <= 9
//Ret: character corresponding to the value of n
{
  assert(n >= 0 && n <= 9);
  return n + '0';
}

char *gsStringDiv2(char *n)
//Pre: n is of the form "[1-9][0-9]*"
//Ret: the string representation of n div 2
//     Note that the resulting string is created with malloc, so it has to be
//     freed
{
  assert(strlen(n) > 0);
  int l = strlen(n); //length of n
  char *r = (char *) 
    malloc((l - (((l>0)&&(n[0]=='1'))?1:0) + 1) * sizeof(char)); //result char*
  //calculate r[0]
  r[0] = gsInt2Char(gsChar2Int(n[0])/2);
  //declare counter for the elements of r
  int j = (r[0] == '0')?0:1;
  //calculate remaining indices of r
  for (int i=1; i<l; i++)
  {
    //r[j] = 5*(n[i-1] mod 2) + n[i] div 2
    r[j] = gsInt2Char(5*(gsChar2Int(n[i-1])%2) + gsChar2Int(n[i])/2);
    //update j
    j = j+1;
  }
  //terminate string
  r[j] = 0;
  return r;
}

int gsStringMod2(char *n)
//Pre: n is of the form "[1-9][0-9]*"
//Ret: the value of n mod 2
{
  assert(strlen(n) > 0);
  return gsChar2Int(n[strlen(n)-1]) % 2;
}

ATermAppl gsMakeDataExprPos(char *p)
//Pre: p is of the form "[1-9][0-9]*"
//Ret: data expression of sort Pos that is a representation of p
{
  assert(strlen(p) > 0);
  if (!strcmp(p, "1")) {
    return gsMakeDataExpr1();
  } else {
    char *d = gsStringDiv2(p);
    if (gsStringMod2(p) == 0) {
      return gsMakeDataExprDot0(gsMakeDataExprPos(d));
    } else {
      return gsMakeDataExprDot1(gsMakeDataExprPos(d));
    }
    free(d);
  }
}

ATermAppl gsMakeDataExprNat(char *n)
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: data expression of sort Nat that is a representation of n
{
  if (!strcmp(n, "0")) {
    return gsMakeDataExpr0();
  } else {
    return gsMakeDataExprPosAsNat(gsMakeDataExprPos(n));
  }
}

ATermAppl gsMakeDataExprInt(char *i)
//Pre: i is of the form "0 | -? [1-9][0-9]*"
//Ret: data expression of sort Int that is a representation of i
{
  if (!strncmp(i, "-", 1)) {
    return gsMakeDataExprNegatePos(gsMakeDataExprPos(i+1));
  } else {
    return gsMakeDataExprNatAsInt(gsMakeDataExprNat(i));
  }
}
