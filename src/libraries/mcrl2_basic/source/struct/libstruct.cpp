#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "libstruct.h"
#include "liblowlevel.h"

extern "C" {

//Local declarations
//------------------

bool IsPNSort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos or Nat

bool IsPNISort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos, Nat or Int

bool IsPNIRSort(ATermAppl SortExpr);
//Ret: SortExpr is a sort expression for Pos, Nat, Int or Real

ATermAppl IntersectPNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNSort(SortExpr1) and IsPNSort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

ATermAppl IntersectPNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNISort(SortExpr1) and IsPNISort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

ATermAppl IntersectPNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNIRSort(SortExpr1) and IsPNIRSort(SortExpr2)
//Ret: the sort resulting from intersecting SortExpr1 and SortExpr2

ATermAppl UnitePNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNSort(SortExpr1) and IsPNSort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

ATermAppl UnitePNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNISort(SortExpr1) and IsPNISort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

ATermAppl UnitePNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2);
//Pre: IsPNIRSort(SortExpr1) and IsPNIRSort(SortExpr2)
//Ret: the sort resulting from uniting SortExpr1 and SortExpr2

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

ATermAppl gsString2ATermAppl(const char *s)
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

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix)
{
  bool found = false;
  ATermAppl NewTerm = gsString2ATermAppl(s);
  if (TryNoSuffix) {
    //try "s"
    found = !gsOccurs((ATerm) NewTerm, Term);
  }
  if (!found) {
    //find "sk" that does not occur in Term
    char *Name = (char *) malloc((strlen(s)+NrOfChars(INT_MAX)+1)*sizeof(char));
    for (int i = 0; i < INT_MAX && !found; i++) {
      sprintf(Name, "%s%d", s, i);
      NewTerm = gsString2ATermAppl(Name);
      found = !gsOccurs((ATerm) NewTerm, Term);
    }
    free(Name);
  }
  if (found) {
    return NewTerm;
  } else {
    //there is no fresh ATermAppl "si", with 0 <= i < INT_MAX
    fprintf(stderr, "error: cannot generate fresh ATermAppl with prefix %s\n", s);
    return NULL;
  }
}

//Enabling constructor functions
//------------------------------

//Constant AFun's for each constructor element of the internal ATerm structure.
static AFun gsAFunSpecV1;
static AFun gsAFunSortSpec;
static AFun gsAFunConsSpec;
static AFun gsAFunMapSpec;
static AFun gsAFunDataEqnSpec;
static AFun gsAFunActSpec;
static AFun gsAFunSortId;
static AFun gsAFunSortRef;
static AFun gsAFunOpId;
static AFun gsAFunDataEqn;
static AFun gsAFunDataVarId;
static AFun gsAFunNil;
static AFun gsAFunActId;
static AFun gsAFunProcEqnSpec;
static AFun gsAFunLPE;
static AFun gsAFunProcEqn;
static AFun gsAFunProcVarId;
static AFun gsAFunLPESummand;
static AFun gsAFunMultAct;
static AFun gsAFunDelta;
static AFun gsAFunAction;
static AFun gsAFunAssignment;
static AFun gsAFunInit;
static AFun gsAFunLPEInit;
static AFun gsAFunSortList;
static AFun gsAFunSortSet;
static AFun gsAFunSortBag;
static AFun gsAFunSortStruct;
static AFun gsAFunSortArrowProd;
static AFun gsAFunSortArrow;
static AFun gsAFunStructCons;
static AFun gsAFunStructProj;
static AFun gsAFunId;
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
static AFun gsAFunParamId;
static AFun gsAFunProcess;
static AFun gsAFunTau;
static AFun gsAFunSum;
static AFun gsAFunBlock;
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
static AFun gsAFunStateTrue;
static AFun gsAFunStateFalse;
static AFun gsAFunStateNot;
static AFun gsAFunStateAnd;
static AFun gsAFunStateOr;
static AFun gsAFunStateImp;
static AFun gsAFunStateForall;
static AFun gsAFunStateExists;
static AFun gsAFunStateMust;
static AFun gsAFunStateMay;
static AFun gsAFunStateDelay;
static AFun gsAFunStateDelayTimed;
static AFun gsAFunStateVar;
static AFun gsAFunStateNu;
static AFun gsAFunStateMu;
static AFun gsAFunDataVarIdInit;
static AFun gsAFunRegNil;
static AFun gsAFunRegSeq;
static AFun gsAFunRegAlt;
static AFun gsAFunRegTrans;
static AFun gsAFunRegTransOrNil;
static AFun gsAFunActTrue;
static AFun gsAFunActFalse;
static AFun gsAFunActNot;
static AFun gsAFunActAnd;
static AFun gsAFunActOr;
static AFun gsAFunActImp;
static AFun gsAFunActForall;
static AFun gsAFunActExists;
static AFun gsAFunActAt;

// pbes symbols
static AFun gsAFunMu;
static AFun gsAFunNu;
static AFun gsAFunPBES;
static AFun gsAFunPBESAnd;
static AFun gsAFunPBESExists;
static AFun gsAFunPBESFalse;
static AFun gsAFunPBESForall;
static AFun gsAFunPBESImp;
static AFun gsAFunPBESNot;
static AFun gsAFunPBESOr;
static AFun gsAFunPBESTrue;
static AFun gsAFunPBEqn;
static AFun gsAFunPropVarDecl;
static AFun gsAFunPropVarInst;

//Constant ATermAppl's for each sort system identifier name
static ATermAppl gsSortIdNameBool;
static ATermAppl gsSortIdNamePos;
static ATermAppl gsSortIdNameNat;
static ATermAppl gsSortIdNameNatPair;
static ATermAppl gsSortIdNameInt;
static ATermAppl gsSortIdNameReal;

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
static ATermAppl gsOpIdNameC1;
static ATermAppl gsOpIdNameCDub;
static ATermAppl gsOpIdNameC0;
static ATermAppl gsOpIdNameCNat;
static ATermAppl gsOpIdNameCPair;
static ATermAppl gsOpIdNameCNeg;
static ATermAppl gsOpIdNameCInt;
static ATermAppl gsOpIdNameCReal;
static ATermAppl gsOpIdNamePos2Nat;
static ATermAppl gsOpIdNamePos2Int;
static ATermAppl gsOpIdNamePos2Real;
static ATermAppl gsOpIdNameNat2Pos;
static ATermAppl gsOpIdNameNat2Int;
static ATermAppl gsOpIdNameNat2Real;
static ATermAppl gsOpIdNameInt2Pos;
static ATermAppl gsOpIdNameInt2Nat;
static ATermAppl gsOpIdNameInt2Real;
static ATermAppl gsOpIdNameReal2Pos;
static ATermAppl gsOpIdNameReal2Nat;
static ATermAppl gsOpIdNameReal2Int;
static ATermAppl gsOpIdNameLTE;
static ATermAppl gsOpIdNameLT;
static ATermAppl gsOpIdNameGTE;
static ATermAppl gsOpIdNameGT;
static ATermAppl gsOpIdNameMax;
static ATermAppl gsOpIdNameMin;
static ATermAppl gsOpIdNameAbs;
static ATermAppl gsOpIdNameNeg;
static ATermAppl gsOpIdNameSucc;
static ATermAppl gsOpIdNamePred;
static ATermAppl gsOpIdNameDub;
static ATermAppl gsOpIdNameAdd;
static ATermAppl gsOpIdNameAddC;
static ATermAppl gsOpIdNameSubt;
static ATermAppl gsOpIdNameGTESubt;
static ATermAppl gsOpIdNameGTESubtB;
static ATermAppl gsOpIdNameMult;
static ATermAppl gsOpIdNameMultIR;
static ATermAppl gsOpIdNameDiv;
static ATermAppl gsOpIdNameMod;
static ATermAppl gsOpIdNameDivMod;
static ATermAppl gsOpIdNameGDivMod;
static ATermAppl gsOpIdNameGGDivMod;
static ATermAppl gsOpIdNameFirst;
static ATermAppl gsOpIdNameLast;
static ATermAppl gsOpIdNameExp;
static ATermAppl gsOpIdNameEven;
static ATermAppl gsOpIdNameEmptyList;
static ATermAppl gsOpIdNameListSize;
static ATermAppl gsOpIdNameCons;
static ATermAppl gsOpIdNameSnoc;
static ATermAppl gsOpIdNameConcat;
static ATermAppl gsOpIdNameEltAt;
static ATermAppl gsOpIdNameHead;
static ATermAppl gsOpIdNameTail;
static ATermAppl gsOpIdNameRHead;
static ATermAppl gsOpIdNameRTail;
static ATermAppl gsOpIdNameEltIn;
static ATermAppl gsOpIdNameSetComp;
static ATermAppl gsOpIdNameEmptySet;
static ATermAppl gsOpIdNameSubSetEq;
static ATermAppl gsOpIdNameSubSet;
static ATermAppl gsOpIdNameSetUnion;
static ATermAppl gsOpIdNameSetDiff;
static ATermAppl gsOpIdNameSetIntersect;
static ATermAppl gsOpIdNameSetCompl;
static ATermAppl gsOpIdNameBagComp;
static ATermAppl gsOpIdNameBag2Set;
static ATermAppl gsOpIdNameSet2Bag;
static ATermAppl gsOpIdNameEmptyBag;
static ATermAppl gsOpIdNameCount;
static ATermAppl gsOpIdNameSubBagEq;
static ATermAppl gsOpIdNameSubBag;
static ATermAppl gsOpIdNameBagUnion;
static ATermAppl gsOpIdNameBagDiff;
static ATermAppl gsOpIdNameBagIntersect;

//Indicates if gsEnableConstructorFunctions has been called
static bool ConstructorFunctionsEnabled = false;

bool gsConstructorFunctionsEnabled(void)
{
  return ConstructorFunctionsEnabled;
}

void gsEnableConstructorFunctions(void)
{
  if (!ConstructorFunctionsEnabled) {
    //create constructor AFun's
    gsAFunSpecV1           = ATmakeAFun("SpecV1", 7, ATfalse);
    gsAFunSortSpec         = ATmakeAFun("SortSpec", 1, ATfalse);
    gsAFunConsSpec         = ATmakeAFun("ConsSpec", 1, ATfalse);
    gsAFunMapSpec          = ATmakeAFun("MapSpec", 1, ATfalse);
    gsAFunDataEqnSpec      = ATmakeAFun("DataEqnSpec", 1, ATfalse);
    gsAFunActSpec          = ATmakeAFun("ActSpec", 1, ATfalse);
    gsAFunSortId           = ATmakeAFun("SortId", 1, ATfalse);
    gsAFunSortRef          = ATmakeAFun("SortRef", 2, ATfalse);
    gsAFunOpId             = ATmakeAFun("OpId", 2, ATfalse);
    gsAFunDataEqn          = ATmakeAFun("DataEqn", 4, ATfalse);
    gsAFunDataVarId        = ATmakeAFun("DataVarId", 2, ATfalse);
    gsAFunNil              = ATmakeAFun("Nil", 0, ATfalse);
    gsAFunActId            = ATmakeAFun("ActId", 2, ATfalse);
    gsAFunProcEqnSpec      = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
    gsAFunLPE              = ATmakeAFun("LPE", 3, ATfalse);
    gsAFunProcEqn          = ATmakeAFun("ProcEqn", 4, ATfalse);
    gsAFunProcVarId        = ATmakeAFun("ProcVarId", 2, ATfalse);
    gsAFunLPESummand       = ATmakeAFun("LPESummand", 5, ATfalse);
    gsAFunMultAct          = ATmakeAFun("MultAct", 1, ATfalse);
    gsAFunDelta            = ATmakeAFun("Delta", 0, ATfalse);
    gsAFunAction           = ATmakeAFun("Action", 2, ATfalse);
    gsAFunAssignment       = ATmakeAFun("Assignment", 2, ATfalse);
    gsAFunInit             = ATmakeAFun("Init", 2, ATfalse);
    gsAFunLPEInit          = ATmakeAFun("LPEInit", 2, ATfalse);
    gsAFunSortList         = ATmakeAFun("SortList", 1, ATfalse);
    gsAFunSortSet          = ATmakeAFun("SortSet", 1, ATfalse);
    gsAFunSortBag          = ATmakeAFun("SortBag", 1, ATfalse);
    gsAFunSortStruct       = ATmakeAFun("SortStruct", 1, ATfalse);
    gsAFunSortArrowProd    = ATmakeAFun("SortArrowProd", 2, ATfalse);
    gsAFunSortArrow        = ATmakeAFun("SortArrow", 2, ATfalse);
    gsAFunStructCons       = ATmakeAFun("StructCons", 3, ATfalse);
    gsAFunStructProj       = ATmakeAFun("StructProj", 2, ATfalse);
    gsAFunId               = ATmakeAFun("Id", 1, ATfalse);
    gsAFunDataApplProd     = ATmakeAFun("DataApplProd", 2, ATfalse);
    gsAFunDataAppl         = ATmakeAFun("DataAppl", 2, ATfalse);
    gsAFunNumber           = ATmakeAFun("Number", 2, ATfalse);
    gsAFunListEnum         = ATmakeAFun("ListEnum", 2, ATfalse);
    gsAFunSetEnum          = ATmakeAFun("SetEnum", 2, ATfalse);
    gsAFunBagEnum          = ATmakeAFun("BagEnum", 2, ATfalse);
    gsAFunSetBagComp       = ATmakeAFun("SetBagComp", 2, ATfalse);
    gsAFunForall           = ATmakeAFun("Forall", 2, ATfalse);
    gsAFunExists           = ATmakeAFun("Exists", 2, ATfalse);
    gsAFunLambda           = ATmakeAFun("Lambda", 2, ATfalse);
    gsAFunWhr              = ATmakeAFun("Whr", 2, ATfalse);
    gsAFunUnknown          = ATmakeAFun("Unknown", 0, ATfalse);
    gsAFunBagEnumElt       = ATmakeAFun("BagEnumElt", 2, ATfalse);
    gsAFunWhrDecl          = ATmakeAFun("WhrDecl", 2, ATfalse);
    gsAFunParamId          = ATmakeAFun("ParamId", 2, ATfalse);
    gsAFunProcess          = ATmakeAFun("Process", 2, ATfalse);
    gsAFunTau              = ATmakeAFun("Tau", 0, ATfalse);
    gsAFunSum              = ATmakeAFun("Sum", 2, ATfalse);
    gsAFunBlock            = ATmakeAFun("Block", 2, ATfalse);
    gsAFunAllow            = ATmakeAFun("Allow", 2, ATfalse);
    gsAFunHide             = ATmakeAFun("Hide", 2, ATfalse);
    gsAFunRename           = ATmakeAFun("Rename", 2, ATfalse);
    gsAFunComm             = ATmakeAFun("Comm", 2, ATfalse);
    gsAFunSync             = ATmakeAFun("Sync", 2, ATfalse);
    gsAFunAtTime           = ATmakeAFun("AtTime", 2, ATfalse);
    gsAFunSeq              = ATmakeAFun("Seq", 2, ATfalse);
    gsAFunCond             = ATmakeAFun("Cond", 3, ATfalse);
    gsAFunBInit            = ATmakeAFun("BInit", 2, ATfalse);
    gsAFunMerge            = ATmakeAFun("Merge", 2, ATfalse);
    gsAFunLMerge           = ATmakeAFun("LMerge", 2, ATfalse);
    gsAFunChoice           = ATmakeAFun("Choice", 2, ATfalse);
    gsAFunMultActName      = ATmakeAFun("MultActName", 1, ATfalse);
    gsAFunRenameExpr       = ATmakeAFun("RenameExpr", 2, ATfalse);
    gsAFunCommExpr         = ATmakeAFun("CommExpr", 2, ATfalse);
    gsAFunStateTrue        = ATmakeAFun("StateTrue", 0, ATfalse);
    gsAFunStateFalse       = ATmakeAFun("StateFalse", 0, ATfalse);
    gsAFunStateNot         = ATmakeAFun("StateNot", 1, ATfalse);
    gsAFunStateAnd         = ATmakeAFun("StateAnd", 2, ATfalse);
    gsAFunStateOr          = ATmakeAFun("StateOr", 2, ATfalse);
    gsAFunStateImp         = ATmakeAFun("StateImp", 2, ATfalse);
    gsAFunStateForall      = ATmakeAFun("StateForall", 2, ATfalse);
    gsAFunStateExists      = ATmakeAFun("StateExists", 2, ATfalse);
    gsAFunStateMust        = ATmakeAFun("StateMust", 2, ATfalse);
    gsAFunStateMay         = ATmakeAFun("StateMay", 2, ATfalse);
    gsAFunStateDelay       = ATmakeAFun("StateDelay", 0, ATfalse);
    gsAFunStateDelayTimed  = ATmakeAFun("StateDelayTimed", 1, ATfalse);
    gsAFunStateVar         = ATmakeAFun("StateVar", 2, ATfalse);
    gsAFunStateNu          = ATmakeAFun("StateNu", 3, ATfalse);
    gsAFunStateMu          = ATmakeAFun("StateMu", 3, ATfalse);
    gsAFunDataVarIdInit    = ATmakeAFun("DataVarIdInit", 3, ATfalse);
    gsAFunRegNil           = ATmakeAFun("RegNil", 0, ATfalse);
    gsAFunRegSeq           = ATmakeAFun("RegSeq", 2, ATfalse);
    gsAFunRegAlt           = ATmakeAFun("RegAlt", 2, ATfalse);
    gsAFunRegTrans         = ATmakeAFun("RegTrans", 1, ATfalse);
    gsAFunRegTransOrNil    = ATmakeAFun("RegTransOrNil", 1, ATfalse);
    gsAFunActTrue          = ATmakeAFun("ActTrue", 0, ATfalse);
    gsAFunActFalse         = ATmakeAFun("ActFalse", 0, ATfalse);
    gsAFunActNot           = ATmakeAFun("ActNot", 1, ATfalse);
    gsAFunActAnd           = ATmakeAFun("ActAnd", 2, ATfalse);
    gsAFunActOr            = ATmakeAFun("ActOr", 2, ATfalse);
    gsAFunActImp           = ATmakeAFun("ActImp", 2, ATfalse);
    gsAFunActForall        = ATmakeAFun("ActForall", 2, ATfalse);
    gsAFunActExists        = ATmakeAFun("ActExists", 2, ATfalse);
    gsAFunActAt            = ATmakeAFun("ActAt", 2, ATfalse);

    //create sort system identifier names
    gsSortIdNameBool       = gsString2ATermAppl("Bool");
    gsSortIdNamePos        = gsString2ATermAppl("Pos");
    gsSortIdNameNat        = gsString2ATermAppl("Nat");
    gsSortIdNameNatPair    = gsString2ATermAppl("@NatPair");
    gsSortIdNameInt        = gsString2ATermAppl("Int");
    gsSortIdNameReal       = gsString2ATermAppl("Real");

    //create operation system identifier names
    gsOpIdNameTrue         = gsString2ATermAppl("true");
    gsOpIdNameFalse        = gsString2ATermAppl("false");
    gsOpIdNameNot          = gsString2ATermAppl("!");
    gsOpIdNameAnd          = gsString2ATermAppl("&&");
    gsOpIdNameOr           = gsString2ATermAppl("||");
    gsOpIdNameImp          = gsString2ATermAppl("=>");
    gsOpIdNameEq           = gsString2ATermAppl("==");
    gsOpIdNameNeq          = gsString2ATermAppl("!=");
    gsOpIdNameIf           = gsString2ATermAppl("if");
    gsOpIdNameForall       = gsString2ATermAppl("forall");
    gsOpIdNameExists       = gsString2ATermAppl("exists");
    gsOpIdNameC1           = gsString2ATermAppl("@c1");
    gsOpIdNameCDub         = gsString2ATermAppl("@cDub");
    gsOpIdNameC0           = gsString2ATermAppl("@c0");
    gsOpIdNameCNat         = gsString2ATermAppl("@cNat");
    gsOpIdNameCPair        = gsString2ATermAppl("@cPair");
    gsOpIdNameCNeg         = gsString2ATermAppl("@cNeg");
    gsOpIdNameCInt         = gsString2ATermAppl("@cInt");
    gsOpIdNameCReal        = gsString2ATermAppl("@cReal");
    gsOpIdNamePos2Nat      = gsString2ATermAppl("Pos2Nat");
    gsOpIdNamePos2Int      = gsString2ATermAppl("Pos2Int");
    gsOpIdNamePos2Real     = gsString2ATermAppl("Pos2Real");
    gsOpIdNameNat2Pos      = gsString2ATermAppl("Nat2Pos");
    gsOpIdNameNat2Int      = gsString2ATermAppl("Nat2Int");
    gsOpIdNameNat2Real     = gsString2ATermAppl("Nat2Real");
    gsOpIdNameInt2Pos      = gsString2ATermAppl("Int2Pos");
    gsOpIdNameInt2Nat      = gsString2ATermAppl("Int2Nat");
    gsOpIdNameInt2Real     = gsString2ATermAppl("Int2Real");
    gsOpIdNameReal2Pos     = gsString2ATermAppl("Real2Pos");
    gsOpIdNameReal2Nat     = gsString2ATermAppl("Real2Nat");
    gsOpIdNameReal2Int     = gsString2ATermAppl("Real2Int");
    gsOpIdNameLTE          = gsString2ATermAppl("<=");
    gsOpIdNameLT           = gsString2ATermAppl("<");
    gsOpIdNameGTE          = gsString2ATermAppl(">=");
    gsOpIdNameGT           = gsString2ATermAppl(">");
    gsOpIdNameMax          = gsString2ATermAppl("max");
    gsOpIdNameMin          = gsString2ATermAppl("min");
    gsOpIdNameAbs          = gsString2ATermAppl("abs");
    gsOpIdNameNeg          = gsString2ATermAppl("-");
    gsOpIdNameSucc         = gsString2ATermAppl("succ");
    gsOpIdNamePred         = gsString2ATermAppl("pred");
    gsOpIdNameDub          = gsString2ATermAppl("@dub");
    gsOpIdNameAdd          = gsString2ATermAppl("+");
    gsOpIdNameAddC         = gsString2ATermAppl("@addc");
    gsOpIdNameSubt         = gsString2ATermAppl("-");
    gsOpIdNameGTESubt      = gsString2ATermAppl("@gtesubt");
    gsOpIdNameGTESubtB     = gsString2ATermAppl("@gtesubtb");
    gsOpIdNameMult         = gsString2ATermAppl("*");
    gsOpIdNameMultIR       = gsString2ATermAppl("@multir");
    gsOpIdNameDiv          = gsString2ATermAppl("div");
    gsOpIdNameMod          = gsString2ATermAppl("mod");
    gsOpIdNameDivMod       = gsString2ATermAppl("@divmod");
    gsOpIdNameGDivMod      = gsString2ATermAppl("@gdivmod");
    gsOpIdNameGGDivMod     = gsString2ATermAppl("@ggdivmod");
    gsOpIdNameFirst        = gsString2ATermAppl("@first");
    gsOpIdNameLast         = gsString2ATermAppl("@last");
    gsOpIdNameExp          = gsString2ATermAppl("exp");
    gsOpIdNameEven         = gsString2ATermAppl("@even");
    gsOpIdNameEmptyList    = gsString2ATermAppl("[]");
    gsOpIdNameListSize     = gsString2ATermAppl("#");
    gsOpIdNameCons         = gsString2ATermAppl("|>");
    gsOpIdNameSnoc         = gsString2ATermAppl("<|");
    gsOpIdNameConcat       = gsString2ATermAppl("++");
    gsOpIdNameEltAt        = gsString2ATermAppl(".");
    gsOpIdNameHead         = gsString2ATermAppl("head");
    gsOpIdNameTail         = gsString2ATermAppl("tail");
    gsOpIdNameRHead        = gsString2ATermAppl("rhead");
    gsOpIdNameRTail        = gsString2ATermAppl("rtail");
    gsOpIdNameEltIn        = gsString2ATermAppl("in");
    gsOpIdNameSetComp      = gsString2ATermAppl("@set");
    gsOpIdNameEmptySet     = gsString2ATermAppl("{}");
    gsOpIdNameSubSetEq     = gsString2ATermAppl("<=");
    gsOpIdNameSubSet       = gsString2ATermAppl("<");
    gsOpIdNameSetUnion     = gsString2ATermAppl("+");
    gsOpIdNameSetDiff      = gsString2ATermAppl("-");
    gsOpIdNameSetIntersect = gsString2ATermAppl("*");
    gsOpIdNameSetCompl     = gsString2ATermAppl("!");
    gsOpIdNameBagComp      = gsString2ATermAppl("@bag");
    gsOpIdNameBag2Set      = gsString2ATermAppl("Bag2Set");
    gsOpIdNameSet2Bag      = gsString2ATermAppl("Set2Bag");
    gsOpIdNameEmptyBag     = gsString2ATermAppl("{}");
    gsOpIdNameCount        = gsString2ATermAppl("count");
    gsOpIdNameSubBagEq     = gsString2ATermAppl("<=");
    gsOpIdNameSubBag       = gsString2ATermAppl("<");
    gsOpIdNameBagUnion     = gsString2ATermAppl("+");
    gsOpIdNameBagDiff      = gsString2ATermAppl("-");
    gsOpIdNameBagIntersect = gsString2ATermAppl("*");

    //protect constructor AFun's
    ATprotectAFun(gsAFunSpecV1);
    ATprotectAFun(gsAFunSortSpec);
    ATprotectAFun(gsAFunConsSpec);
    ATprotectAFun(gsAFunMapSpec);
    ATprotectAFun(gsAFunDataEqnSpec);
    ATprotectAFun(gsAFunActSpec);
    ATprotectAFun(gsAFunSortId);
    ATprotectAFun(gsAFunSortRef);
    ATprotectAFun(gsAFunOpId);
    ATprotectAFun(gsAFunDataEqn);
    ATprotectAFun(gsAFunDataVarId);
    ATprotectAFun(gsAFunNil);
    ATprotectAFun(gsAFunActId);
    ATprotectAFun(gsAFunProcEqnSpec);
    ATprotectAFun(gsAFunLPE);
    ATprotectAFun(gsAFunProcEqn);
    ATprotectAFun(gsAFunProcVarId);
    ATprotectAFun(gsAFunLPESummand);
    ATprotectAFun(gsAFunMultAct);
    ATprotectAFun(gsAFunDelta);
    ATprotectAFun(gsAFunAction);
    ATprotectAFun(gsAFunAssignment);
    ATprotectAFun(gsAFunInit);
    ATprotectAFun(gsAFunLPEInit);
    ATprotectAFun(gsAFunSortList);
    ATprotectAFun(gsAFunSortSet);
    ATprotectAFun(gsAFunSortBag);
    ATprotectAFun(gsAFunSortStruct);
    ATprotectAFun(gsAFunSortArrowProd);
    ATprotectAFun(gsAFunSortArrow);
    ATprotectAFun(gsAFunStructCons);
    ATprotectAFun(gsAFunStructProj);
    ATprotectAFun(gsAFunId);
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
    ATprotectAFun(gsAFunParamId);
    ATprotectAFun(gsAFunProcess);
    ATprotectAFun(gsAFunTau);
    ATprotectAFun(gsAFunSum);
    ATprotectAFun(gsAFunBlock);
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
    ATprotectAFun(gsAFunStateTrue);
    ATprotectAFun(gsAFunStateFalse);
    ATprotectAFun(gsAFunStateNot);
    ATprotectAFun(gsAFunStateAnd);
    ATprotectAFun(gsAFunStateOr);
    ATprotectAFun(gsAFunStateImp);
    ATprotectAFun(gsAFunStateForall);
    ATprotectAFun(gsAFunStateExists);
    ATprotectAFun(gsAFunStateMust);
    ATprotectAFun(gsAFunStateMay);
    ATprotectAFun(gsAFunStateDelay);
    ATprotectAFun(gsAFunStateDelayTimed);
    ATprotectAFun(gsAFunStateVar);
    ATprotectAFun(gsAFunStateNu);
    ATprotectAFun(gsAFunStateMu);
    ATprotectAFun(gsAFunDataVarIdInit);
    ATprotectAFun(gsAFunRegNil);
    ATprotectAFun(gsAFunRegSeq);
    ATprotectAFun(gsAFunRegAlt);
    ATprotectAFun(gsAFunRegTrans);
    ATprotectAFun(gsAFunRegTransOrNil);
    ATprotectAFun(gsAFunActTrue);
    ATprotectAFun(gsAFunActFalse);
    ATprotectAFun(gsAFunActNot);
    ATprotectAFun(gsAFunActAnd);
    ATprotectAFun(gsAFunActOr);
    ATprotectAFun(gsAFunActImp);
    ATprotectAFun(gsAFunActForall);
    ATprotectAFun(gsAFunActExists);
    ATprotectAFun(gsAFunActAt);
    //protect sort system identifier names
    ATprotectAppl(&gsSortIdNameBool);
    ATprotectAppl(&gsSortIdNamePos);
    ATprotectAppl(&gsSortIdNameNat);
    ATprotectAppl(&gsSortIdNameNatPair);
    ATprotectAppl(&gsSortIdNameInt);
    ATprotectAppl(&gsSortIdNameReal);
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
    ATprotectAppl(&gsOpIdNameC1);
    ATprotectAppl(&gsOpIdNameCDub);
    ATprotectAppl(&gsOpIdNameC0);
    ATprotectAppl(&gsOpIdNameCNat);
    ATprotectAppl(&gsOpIdNameCPair);
    ATprotectAppl(&gsOpIdNameCNeg);
    ATprotectAppl(&gsOpIdNameCInt);
    ATprotectAppl(&gsOpIdNameCReal);
    ATprotectAppl(&gsOpIdNamePos2Nat);
    ATprotectAppl(&gsOpIdNamePos2Int);
    ATprotectAppl(&gsOpIdNamePos2Real);
    ATprotectAppl(&gsOpIdNameNat2Pos);
    ATprotectAppl(&gsOpIdNameNat2Int);
    ATprotectAppl(&gsOpIdNameNat2Real);
    ATprotectAppl(&gsOpIdNameInt2Pos);
    ATprotectAppl(&gsOpIdNameInt2Nat);
    ATprotectAppl(&gsOpIdNameInt2Real);
    ATprotectAppl(&gsOpIdNameReal2Pos);
    ATprotectAppl(&gsOpIdNameReal2Nat);
    ATprotectAppl(&gsOpIdNameReal2Int);
    ATprotectAppl(&gsOpIdNameLTE);
    ATprotectAppl(&gsOpIdNameLT);
    ATprotectAppl(&gsOpIdNameGTE);
    ATprotectAppl(&gsOpIdNameGT);
    ATprotectAppl(&gsOpIdNameMax);
    ATprotectAppl(&gsOpIdNameMin);
    ATprotectAppl(&gsOpIdNameAbs);
    ATprotectAppl(&gsOpIdNameNeg);
    ATprotectAppl(&gsOpIdNameSucc);
    ATprotectAppl(&gsOpIdNamePred);
    ATprotectAppl(&gsOpIdNameDub);
    ATprotectAppl(&gsOpIdNameAdd);
    ATprotectAppl(&gsOpIdNameAddC);
    ATprotectAppl(&gsOpIdNameSubt);
    ATprotectAppl(&gsOpIdNameGTESubt);
    ATprotectAppl(&gsOpIdNameGTESubtB);
    ATprotectAppl(&gsOpIdNameMult);
    ATprotectAppl(&gsOpIdNameMultIR);
    ATprotectAppl(&gsOpIdNameDiv);
    ATprotectAppl(&gsOpIdNameMod);
    ATprotectAppl(&gsOpIdNameDivMod);
    ATprotectAppl(&gsOpIdNameGDivMod);
    ATprotectAppl(&gsOpIdNameGGDivMod);
    ATprotectAppl(&gsOpIdNameFirst);
    ATprotectAppl(&gsOpIdNameLast);
    ATprotectAppl(&gsOpIdNameExp);
    ATprotectAppl(&gsOpIdNameEven);
    ATprotectAppl(&gsOpIdNameEmptyList);
    ATprotectAppl(&gsOpIdNameListSize);
    ATprotectAppl(&gsOpIdNameCons);
    ATprotectAppl(&gsOpIdNameSnoc);
    ATprotectAppl(&gsOpIdNameConcat);
    ATprotectAppl(&gsOpIdNameEltAt);
    ATprotectAppl(&gsOpIdNameHead);
    ATprotectAppl(&gsOpIdNameTail);
    ATprotectAppl(&gsOpIdNameRHead);
    ATprotectAppl(&gsOpIdNameRTail);
    ATprotectAppl(&gsOpIdNameSetComp);
    ATprotectAppl(&gsOpIdNameEmptySet);
    ATprotectAppl(&gsOpIdNameEltIn);
    ATprotectAppl(&gsOpIdNameSubSetEq);
    ATprotectAppl(&gsOpIdNameSubSet);
    ATprotectAppl(&gsOpIdNameSetUnion);
    ATprotectAppl(&gsOpIdNameSetDiff);
    ATprotectAppl(&gsOpIdNameSetIntersect);
    ATprotectAppl(&gsOpIdNameSetCompl);
    ATprotectAppl(&gsOpIdNameBagComp);
    ATprotectAppl(&gsOpIdNameBag2Set);
    ATprotectAppl(&gsOpIdNameSet2Bag);
    ATprotectAppl(&gsOpIdNameEmptyBag);
    ATprotectAppl(&gsOpIdNameCount);
    ATprotectAppl(&gsOpIdNameSubBagEq);
    ATprotectAppl(&gsOpIdNameSubBag);
    ATprotectAppl(&gsOpIdNameBagUnion);
    ATprotectAppl(&gsOpIdNameBagDiff);
    ATprotectAppl(&gsOpIdNameBagIntersect);

    // pbes
    gsAFunMu = ATmakeAFun("Mu", 0, ATfalse);
    gsAFunNu = ATmakeAFun("Nu", 0, ATfalse);
    gsAFunPBES = ATmakeAFun("PBES", 2, ATfalse);
    gsAFunPBESAnd = ATmakeAFun("PBESAnd", 2, ATfalse);
    gsAFunPBESExists = ATmakeAFun("PBESExists", 2, ATfalse);
    gsAFunPBESFalse = ATmakeAFun("PBESFalse", 0, ATfalse);
    gsAFunPBESForall = ATmakeAFun("PBESForall", 2, ATfalse);
    gsAFunPBESImp = ATmakeAFun("PBESImp", 2, ATfalse);
    gsAFunPBESNot = ATmakeAFun("PBESNot", 1, ATfalse);
    gsAFunPBESOr = ATmakeAFun("PBESOr", 2, ATfalse);
    gsAFunPBESTrue = ATmakeAFun("PBESTrue", 0, ATfalse);
    gsAFunPBEqn = ATmakeAFun("PBEqn", 3, ATfalse);
    gsAFunPropVarDecl = ATmakeAFun("PropVarDecl", 2, ATfalse);
    gsAFunPropVarInst = ATmakeAFun("PropVarInst", 2, ATfalse);
    ATprotectAFun(gsAFunMu);
    ATprotectAFun(gsAFunNu);
    ATprotectAFun(gsAFunPBES);
    ATprotectAFun(gsAFunPBESAnd);
    ATprotectAFun(gsAFunPBESExists);
    ATprotectAFun(gsAFunPBESFalse);
    ATprotectAFun(gsAFunPBESForall);
    ATprotectAFun(gsAFunPBESImp);
    ATprotectAFun(gsAFunPBESNot);
    ATprotectAFun(gsAFunPBESOr);
    ATprotectAFun(gsAFunPBESTrue);
    ATprotectAFun(gsAFunPBEqn);
    ATprotectAFun(gsAFunPropVarDecl);
    ATprotectAFun(gsAFunPropVarInst);

    ConstructorFunctionsEnabled = true;
  }
}

//Creation of all constructor elements of the internal structure
//--------------------------------------------------------------

ATermAppl gsMakeSpecV1(
  ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl DataEqnSpec, ATermAppl ActSpec, ATermAppl ProcEqnSpec,
  ATermAppl Init)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl(gsAFunSpecV1, (ATerm) SortSpec, (ATerm) ConsSpec,
    (ATerm) MapSpec, (ATerm) DataEqnSpec, (ATerm) ActSpec, (ATerm) ProcEqnSpec,
    (ATerm) Init);
}

ATermAppl gsMakeSortSpec(ATermList SortDecls)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSpec, (ATerm) SortDecls);
}

ATermAppl gsMakeConsSpec(ATermList OpIds)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunConsSpec, (ATerm) OpIds);
}

ATermAppl gsMakeMapSpec(ATermList OpIds)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMapSpec, (ATerm) OpIds);
}

ATermAppl gsMakeDataEqnSpec(ATermList DataEqns)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunDataEqnSpec, (ATerm) DataEqns);
}

ATermAppl gsMakeActSpec(ATermList ActIds)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunActSpec, (ATerm) ActIds);
}

ATermAppl gsMakeSortId(ATermAppl Name)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortId, (ATerm) Name);
}

ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortRef, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunOpId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl BoolExprOrNil,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunDataEqn, (ATerm) DataVarIds, (ATerm) BoolExprOrNil,
    (ATerm) DataExprLHS, (ATerm) DataExprRHS);
}

ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataVarId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeNil()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunNil);
}

ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActId, (ATerm) Name, (ATerm) SortExprs);
}

ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunProcEqnSpec, (ATerm) ProcEqns);
}

ATermAppl gsMakeLPE(ATermList GlobDataVarIds, ATermList ProcDataVarIds,
  ATermList LPESummands)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunLPE, (ATerm) GlobDataVarIds, (ATerm) ProcDataVarIds,
    (ATerm) LPESummands);
}

ATermAppl gsMakeProcEqn(ATermList GlobDataVarIds, ATermAppl ProcVarId,
  ATermList ProcDataVarIds, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunProcEqn, (ATerm) GlobDataVarIds, (ATerm) ProcVarId,
    (ATerm) ProcDataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeProcVarId(ATermAppl Name, ATermList SortExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcVarId, (ATerm) Name, (ATerm) SortExprs);
}

ATermAppl gsMakeLPESummand(ATermList DataVarIds, ATermAppl BoolExpr,
  ATermAppl MultiAction, ATermAppl TimeExprOrNil, ATermList Assignments)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl5(gsAFunLPESummand, (ATerm) DataVarIds, (ATerm) BoolExpr,
    (ATerm) MultiAction, (ATerm) TimeExprOrNil, (ATerm) Assignments);
}

ATermAppl gsMakeMultAct(ATermList Actions)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMultAct, (ATerm) Actions);
}

ATermAppl gsMakeDelta()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunDelta);
}

ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAction, (ATerm) ActId, (ATerm) DataExprs);
}

ATermAppl gsMakeAssignment(ATermAppl DataVarId, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAssignment, (ATerm) DataVarId, (ATerm) DataExpr);
}

ATermAppl gsMakeInit(ATermList GlobDataVarIds, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunInit, (ATerm) GlobDataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeLPEInit(ATermList GlobDataVarIds, ATermList DataExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLPEInit, (ATerm) GlobDataVarIds, (ATerm) DataExprs);
}

ATermAppl gsMakeSortList(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortList, (ATerm) SortExpr);
}

ATermAppl gsMakeSortSet(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSet, (ATerm) SortExpr);
}

ATermAppl gsMakeSortBag(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortBag, (ATerm) SortExpr);
}

ATermAppl gsMakeSortStruct(ATermList StructConss)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortStruct, (ATerm) StructConss);
}

ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrowProd, (ATerm) SortExprsDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrow, (ATerm) SortExprDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecNameOrNil)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStructCons, (ATerm) ConsName, (ATerm) StructProjs,
    (ATerm) RecNameOrNil);
}

ATermAppl gsMakeStructProj(ATermAppl ProjNameOrNil, ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStructProj, (ATerm) ProjNameOrNil,
    (ATerm) SortExpr);
}

ATermAppl gsMakeId(ATermAppl Name)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunId, (ATerm) Name);
}

ATermAppl gsMakeDataApplProd(ATermAppl DataExpr, ATermList DataArgs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataApplProd, (ATerm) DataExpr, (ATerm) DataArgs);
}

ATermAppl gsMakeDataAppl(ATermAppl DataExpr, ATermAppl DataArg)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataAppl, (ATerm) DataExpr, (ATerm) DataArg);
}

ATermAppl gsMakeNumber(ATermAppl Number, ATermAppl SortExprOrUnknown)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunNumber, (ATerm) Number, (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeListEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunListEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeBagEnum(ATermList BagEnumElts, ATermAppl SortExprOrUnknown)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnum, (ATerm) BagEnumElts,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetBagComp(ATermAppl DataVarId, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetBagComp, (ATerm) DataVarId, (ATerm) DataExpr);
}

ATermAppl gsMakeForall(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunForall, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeExists(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunExists, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeLambda(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLambda, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhr, (ATerm) DataExpr, (ATerm) WhrDecls);
}

ATermAppl gsMakeUnknown()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunUnknown);
}

ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnumElt, (ATerm) DataExpr, (ATerm) Multiplicity);
}

ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhrDecl, (ATerm) Name, (ATerm) DataExpr);
}

ATermAppl gsMakeParamId(ATermAppl Name, ATermList DataExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunParamId, (ATerm) Name, (ATerm) DataExprs);
}

ATermAppl gsMakeProcess(ATermAppl ProcId, ATermList DataExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcess, (ATerm) ProcId, (ATerm) DataExprs);
}

ATermAppl gsMakeTau()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunTau);
}

ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSum, (ATerm) DataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeBlock(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBlock, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeHide(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunHide, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeRename(ATermList RenameExprs, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRename, (ATerm) RenameExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeComm(ATermList CommExprs, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunComm, (ATerm) CommExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAllow, (ATerm) MultActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeSync(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSync, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl TimeExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAtTime, (ATerm) ProcExpr, (ATerm) TimeExpr);
}

ATermAppl gsMakeSeq(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSeq, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeCond(ATermAppl BoolExpr, ATermAppl ProcExprThen,
  ATermAppl ProcExprElse)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunCond, (ATerm) BoolExpr, (ATerm) ProcExprThen,
    (ATerm) ProcExprElse);
}

ATermAppl gsMakeBInit(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBInit, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeLMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeChoice(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunChoice, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeMultActName(ATermList ActNames)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMultActName, (ATerm) ActNames);
}

ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRenameExpr, (ATerm) FromName, (ATerm) ToName);
}

ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActNameOrNil)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunCommExpr, (ATerm) MultActName, (ATerm) ActNameOrNil);
}

ATermAppl gsMakeStateTrue()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateTrue);
}

ATermAppl gsMakeStateFalse()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateFalse);
}

ATermAppl gsMakeStateNot(ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunStateNot, (ATerm) StateFrm);
}

ATermAppl gsMakeStateAnd(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateAnd, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateOr(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateOr, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateImp(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateImp, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateForall(ATermList DataVarIds, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateForall, (ATerm) DataVarIds, (ATerm) StateFrm);
}

ATermAppl gsMakeStateExists(ATermList DataVarIds, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateExists, (ATerm) DataVarIds, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMust(ATermAppl RegFrm, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateMust, (ATerm) RegFrm, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMay(ATermAppl RegFrm, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateMay, (ATerm) RegFrm, (ATerm) StateFrm);
}

ATermAppl gsMakeStateDelay()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateDelay);
}

ATermAppl gsMakeStateDelayTimed(ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunStateDelayTimed, (ATerm) DataExpr);
}

ATermAppl gsMakeStateVar(ATermAppl VarName, ATermList DataExprs)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateVar, (ATerm) VarName, (ATerm) DataExprs);
}

ATermAppl gsMakeStateNu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStateNu, (ATerm) VarName, (ATerm) DataVarIdInits, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStateMu, (ATerm) VarName, (ATerm) DataVarIdInits, (ATerm) StateFrm);
}

ATermAppl gsMakeDataVarIdInit(ATermAppl VarName, ATermAppl SortExpr, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunDataVarIdInit, (ATerm) VarName, (ATerm) SortExpr, (ATerm) DataExpr);
}

ATermAppl gsMakeRegNil()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunRegNil);
}

ATermAppl gsMakeRegSeq(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRegSeq, (ATerm) RegFrmLHS, (ATerm) RegFrmRHS);
}

ATermAppl gsMakeRegAlt(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRegAlt, (ATerm) RegFrmLHS, (ATerm) RegFrmRHS);
}

ATermAppl gsMakeRegTrans(ATermAppl RegFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunRegTrans, (ATerm) RegFrm);
}

ATermAppl gsMakeRegTransOrNil(ATermAppl RegFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunRegTransOrNil, (ATerm) RegFrm);
}

ATermAppl gsMakeActTrue()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunActTrue);
}

ATermAppl gsMakeActFalse()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunActFalse);
}

ATermAppl gsMakeActNot(ATermAppl ActFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunActNot, (ATerm) ActFrm);
}

ATermAppl gsMakeActAnd(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActAnd, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActOr(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActOr, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActImp(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActImp, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActForall(ATermList DataVarIds, ATermAppl ActFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActForall, (ATerm) DataVarIds, (ATerm) ActFrm);
}

ATermAppl gsMakeActExists(ATermList DataVarIds, ATermAppl ActFrm)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActExists, (ATerm) DataVarIds, (ATerm) ActFrm);
}

ATermAppl gsMakeActAt(ATermAppl ActFrm, ATermAppl DataExpr)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActAt, (ATerm) ActFrm, (ATerm) DataExpr);
}

//Recognisers of all constructor elements of the internal structure
//-----------------------------------------------------------------

bool gsIsSpecV1(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSpecV1;
}

bool gsIsSortSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSpec;
}

bool gsIsConsSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunConsSpec;
}

bool gsIsMapSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMapSpec;
}

bool gsIsDataEqnSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqnSpec;
}

bool gsIsActSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActSpec;
}

bool gsIsSortId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortId;
}

bool gsIsSortRef(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortRef;
}

bool gsIsOpId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunOpId;
}

bool gsIsDataEqn(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqn;
}

bool gsIsDataVarId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarId;
}

bool gsIsNil(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNil;
}

bool gsIsActId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActId;
}

bool gsIsProcEqnSpec(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqnSpec;
}

bool gsIsLPE(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPE;
}

bool gsIsProcEqn(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqn;
}

bool gsIsProcVarId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcVarId;
}

bool gsIsLPESummand(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPESummand;
}

bool gsIsMultAct(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMultAct;
}

bool gsIsDelta(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDelta;
}

bool gsIsAction(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAction;
}

bool gsIsAssignment(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAssignment;
}

bool gsIsInit(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunInit;
}

bool gsIsLPEInit(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPEInit;
}

bool gsIsSortList(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortList;
}

bool gsIsSortSet(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSet;
}

bool gsIsSortBag(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortBag;
}

bool gsIsSortStruct(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortStruct;
}

bool gsIsSortArrowProd(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrowProd;
}

bool gsIsSortArrow(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrow;
}

bool gsIsStructCons(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructCons;
}

bool gsIsStructProj(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructProj;
}

bool gsIsId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunId;
}

bool gsIsDataApplProd(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataApplProd;
}

bool gsIsDataAppl(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataAppl;
}

bool gsIsNumber(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNumber;
}

bool gsIsListEnum(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunListEnum;
}

bool gsIsSetEnum(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetEnum;
}

bool gsIsBagEnum(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnum;
}

bool gsIsSetBagComp(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetBagComp;
}

bool gsIsForall(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunForall;
}

bool gsIsExists(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunExists;
}

bool gsIsLambda(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLambda;
}

bool gsIsWhr(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhr;
}

bool gsIsUnknown(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunUnknown;
}

bool gsIsBagEnumElt(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnumElt;
}

bool gsIsWhrDecl(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhrDecl;
}

bool gsIsParamId(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunParamId;
}

bool gsIsProcess(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcess;
}

bool gsIsTau(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunTau;
}

bool gsIsSum(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSum;
}

bool gsIsBlock(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBlock;
}

bool gsIsHide(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunHide;
}

bool gsIsRename(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRename;
}

bool gsIsComm(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunComm;
}

bool gsIsAllow(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAllow;
}

bool gsIsSync(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSync;
}

bool gsIsAtTime(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAtTime;
}

bool gsIsSeq(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSeq;
}

bool gsIsCond(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunCond;
}

bool gsIsBInit(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBInit;
}

bool gsIsMerge(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMerge;
}

bool gsIsLMerge(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLMerge;
}

bool gsIsChoice(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunChoice;
}

bool gsIsMultActName(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMultActName;
}

bool gsIsRenameExpr(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRenameExpr;
}

bool gsIsCommExpr(ATermAppl Term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunCommExpr;
}

bool gsIsStateTrue(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateTrue;
}

bool gsIsStateFalse(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateFalse;
}

bool gsIsStateNot(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateNot;
}

bool gsIsStateAnd(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateAnd;
}

bool gsIsStateOr(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateOr;
}

bool gsIsStateImp(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateImp;
}

bool gsIsStateForall(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateForall;
}

bool gsIsStateExists(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateExists;
}

bool gsIsStateMust(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMust;
}

bool gsIsStateMay(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMay;
}

bool gsIsStateDelay(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateDelay;
}

bool gsIsStateDelayTimed(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateDelayTimed;
}

bool gsIsStateVar(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateVar;
}

bool gsIsStateNu(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateNu;
}

bool gsIsStateMu(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMu;
}

bool gsIsDataVarIdInit(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarIdInit;
}

bool gsIsRegNil(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegNil;
}

bool gsIsRegSeq(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegSeq;
}

bool gsIsRegAlt(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegAlt;
}

bool gsIsRegTrans(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegTrans;
}

bool gsIsRegTransOrNil(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegTransOrNil;
}

bool gsIsActTrue(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActTrue;
}

bool gsIsActFalse(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActFalse;
}

bool gsIsActNot(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActNot;
}

bool gsIsActAnd(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActAnd;
}

bool gsIsActOr(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActOr;
}

bool gsIsActImp(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActImp;
}

bool gsIsActForall(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActForall;
}

bool gsIsActExists(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActExists;
}

bool gsIsActAt(ATermAppl Term)
{
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActAt;
}

//Sort expressions
//----------------

bool gsIsSortExpr(ATermAppl Term)
{
  return
    gsIsSortId(Term)   || gsIsSortArrow(Term)  ||
    gsIsSortList(Term) || gsIsSortSet(Term)    ||
    gsIsSortBag(Term)  || gsIsSortStruct(Term) ||
    gsIsSortArrowProd(Term);
}

bool gsIsSortExprOrUnknown(ATermAppl Term)
{
  return gsIsSortExpr(Term) || gsIsUnknown(Term);
}


//Creation of names for system sort identifiers

ATermAppl gsMakeSortIdNameBool() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNameBool;
}

ATermAppl gsMakeSortIdNamePos() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNamePos;
}

ATermAppl gsMakeSortIdNameNat() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNameNat;
}

ATermAppl gsMakeSortIdNameNatPair() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNameNatPair;
}

ATermAppl gsMakeSortIdNameInt() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNameInt;
}

ATermAppl gsMakeSortIdNameReal() {
  assert(ConstructorFunctionsEnabled);
  return gsSortIdNameReal;
}


//Creation of sort identifiers for system defined sorts.

ATermAppl gsMakeSortIdBool()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameBool);
}

ATermAppl gsMakeSortIdPos()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNamePos);
}

ATermAppl gsMakeSortIdNat()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameNat);
}

ATermAppl gsMakeSortIdNatPair()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameNatPair);
}

ATermAppl gsMakeSortIdInt()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameInt);
}

ATermAppl gsMakeSortIdReal()
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeSortId(gsSortIdNameReal);
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

ATermAppl gsMakeSortExprNatPair()
{
  return gsMakeSortIdNatPair();
}

ATermAppl gsMakeSortExprInt()
{
  return gsMakeSortIdInt();
}

ATermAppl gsMakeSortExprReal()
{
  return gsMakeSortIdReal();
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

ATermAppl gsMakeSortArrow4(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprDom4, ATermAppl SortExprResult)
{
  return gsMakeSortArrowList(
    ATmakeList4((ATerm) SortExprDom1, (ATerm) SortExprDom2,
      (ATerm) SortExprDom3, (ATerm) SortExprDom4),
    SortExprResult);
}

ATermAppl gsMakeSortArrowList(ATermList SortExprDomain,
  ATermAppl SortExprResult)
{
  ATermAppl Result = SortExprResult;
  ATermList l = ATreverse(SortExprDomain);
  while (!ATisEmpty(l))
  {
    Result = gsMakeSortArrow(ATAgetFirst(l), Result);
    l = ATgetNext(l);
  }
  return Result;
}

ATermAppl gsGetSortExprResult(ATermAppl SortExpr)
{
  while (gsIsSortArrow(SortExpr)) {
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  while (gsIsSortArrowProd(SortExpr)) {
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  return SortExpr;
}

ATermList gsGetSortExprDomain(ATermAppl SortExpr)
{
  ATermList l = ATmakeList0();
  while (gsIsSortArrow(SortExpr)) {
    l = ATinsert(l, ATgetArgument(SortExpr, 0));
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  while (gsIsSortArrowProd(SortExpr)) {
    ATermList m = ATLgetArgument(SortExpr, 0);
    while (!ATisEmpty(m)) {
      l = ATinsert(l, ATgetFirst(m));
      m = ATgetNext(m);
    }
    SortExpr = ATAgetArgument(SortExpr, 1);
  }
  l = ATreverse(l);
  return l;
}


//Data expressions
//----------------

bool gsIsDataExpr(ATermAppl Term)
{
  return gsIsId(Term)    || gsIsDataVarId(Term)    || gsIsOpId(Term)    ||
    gsIsDataAppl(Term)   || gsIsDataApplProd(Term) || gsIsNumber(Term)  ||
    gsIsListEnum(Term)   || gsIsSetEnum(Term)      || gsIsBagEnum(Term) ||
    gsIsSetBagComp(Term) || gsIsForall(Term)       || gsIsExists(Term)  ||
    gsIsLambda(Term)     || gsIsWhr(Term);
}

ATermAppl gsGetSort(ATermAppl DataExpr)
{
  ATermAppl Result;
  if (gsIsDataAppl(DataExpr)) {
    //DataExpr is a data application; return the result sort of the first
    //argument
    ATermAppl HeadSort = gsGetSort(ATAgetArgument(DataExpr, 0));
    if (gsIsSortArrow(HeadSort))
      Result = ATAgetArgument(HeadSort, 1);
    else
      Result = gsMakeUnknown();
  } else if (gsIsDataVarId(DataExpr) || gsIsOpId(DataExpr) ||
      gsIsNumber(DataExpr) || gsIsListEnum(DataExpr) ||
      gsIsSetEnum(DataExpr) || gsIsBagEnum(DataExpr)) {
    //DataExpr is a data variable, an operation identifier, a number or an
    //enumeration; return its sort
    Result = ATAgetArgument(DataExpr, 1);
  } else if (gsIsDataApplProd(DataExpr)) {
    //DataExpr is a product data application; return the result sort of the
    //first argument
    ATermAppl HeadSort = gsGetSort(ATAgetArgument(DataExpr, 0));
    if (gsIsSortArrowProd(HeadSort))
      Result = ATAgetArgument(HeadSort, 1);
    else
      Result = gsMakeUnknown();
  } else if (gsIsForall(DataExpr) || gsIsExists(DataExpr)) {
      Result = gsMakeSortExprBool();
  } else if (gsIsSetBagComp(DataExpr)) {
    //DataExpr is a set of bag comprehension; depending on the sort of the
    //body, return Set(S) or Bag(S), where S is the sort of the variable
    //declaration
    ATermAppl Var = ATAgetArgument(DataExpr, 0);
    ATermAppl SortBody = gsGetSort(ATAgetArgument(DataExpr, 1));
    if (ATisEqual(SortBody, gsMakeSortExprBool()))
      Result = gsMakeSortSet(gsGetSort(Var));
    else if (ATisEqual(SortBody, gsMakeSortExprNat()))
      Result = gsMakeSortBag(gsGetSort(Var));
    else
      Result = gsMakeUnknown();
  } else if (gsIsLambda(DataExpr)) {
    //DataExpr is a lambda abstraction of the form
    //  lambda x0: S0, ..., xn: Sn. e
    //return S0 -> ... -> Sn -> gsGetSort(e)
    Result = gsGetSort(ATAgetArgument(DataExpr, 1));
    ATermList Vars = ATreverse(ATLgetArgument(DataExpr, 0));
    while (!ATisEmpty(Vars))
    {
      Result = gsMakeSortArrow(gsGetSort(ATAgetFirst(Vars)), Result);
      Vars = ATgetNext(Vars);
    }
  } else if (gsIsWhr(DataExpr)) {
    //DataExpr is a where clause; return the sort of the body
    Result = gsGetSort(ATAgetArgument(DataExpr, 0));
  } else {
    //DataExpr is a data variable or operation identifier of which the sort is
    //not known; return Unknown
    Result = gsMakeUnknown();
  }
  return Result;
}

ATermAppl gsGetDataExprHead(ATermAppl DataExpr)
{
  while (gsIsDataAppl(DataExpr)) {
   DataExpr = ATAgetArgument(DataExpr, 0);
  }
  while (gsIsDataApplProd(DataExpr)) {
   DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return DataExpr;
}

ATermList gsGetDataExprArgs(ATermAppl DataExpr)
{
  ATermList l = ATmakeList0();
  while (gsIsDataAppl(DataExpr)) {
    l = ATinsert(l, ATgetArgument(DataExpr, 1));
    DataExpr = ATAgetArgument(DataExpr, 0);
  }
  while (gsIsDataApplProd(DataExpr)) {
    l = ATconcat(ATLgetArgument(DataExpr, 1), l);
    DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return l;
}


//Creation of names for system operation identifiers

ATermAppl gsMakeOpIdNameTrue() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameTrue;
}

ATermAppl gsMakeOpIdNameFalse() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameFalse;
}

ATermAppl gsMakeOpIdNameNot() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNot;
}

ATermAppl gsMakeOpIdNameAnd() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameAnd;
}

ATermAppl gsMakeOpIdNameOr() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameOr;
}

ATermAppl gsMakeOpIdNameImp() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameImp;
}

ATermAppl gsMakeOpIdNameEq() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEq;
}

ATermAppl gsMakeOpIdNameNeq() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNeq;
}

ATermAppl gsMakeOpIdNameIf() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameIf;
}

ATermAppl gsMakeOpIdNameForall() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameForall;
}

ATermAppl gsMakeOpIdNameExists() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameExists;
}

ATermAppl gsMakeOpIdNameC1() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameC1;
}

ATermAppl gsMakeOpIdNameCDub() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCDub;
}

ATermAppl gsMakeOpIdNameC0() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameC0;
}

ATermAppl gsMakeOpIdNameCNat() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCNat;
}

ATermAppl gsMakeOpIdNameCPair() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCPair;
}

ATermAppl gsMakeOpIdNameCNeg() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCNeg;
}

ATermAppl gsMakeOpIdNameCInt() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCInt;
}

ATermAppl gsMakeOpIdNameCReal() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCReal;
}

ATermAppl gsMakeOpIdNamePos2Nat() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNamePos2Nat;
}

ATermAppl gsMakeOpIdNamePos2Int() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNamePos2Int;
}

ATermAppl gsMakeOpIdNamePos2Real() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNamePos2Real;
}

ATermAppl gsMakeOpIdNameNat2Pos() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNat2Pos;
}

ATermAppl gsMakeOpIdNameNat2Int() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNat2Int;
}

ATermAppl gsMakeOpIdNameNat2Real() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNat2Real;
}

ATermAppl gsMakeOpIdNameInt2Pos() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameInt2Pos;
}

ATermAppl gsMakeOpIdNameInt2Nat() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameInt2Nat;
}

ATermAppl gsMakeOpIdNameInt2Real() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameInt2Real;
}

ATermAppl gsMakeOpIdNameReal2Pos() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameReal2Pos;
}

ATermAppl gsMakeOpIdNameReal2Nat() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameReal2Nat;
}

ATermAppl gsMakeOpIdNameReal2Int() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameReal2Int;
}

ATermAppl gsMakeOpIdNameLTE() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameLTE;
}

ATermAppl gsMakeOpIdNameLT() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameLT;
}

ATermAppl gsMakeOpIdNameGTE() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGTE;
}

ATermAppl gsMakeOpIdNameGT() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGT;
}

ATermAppl gsMakeOpIdNameMax() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameMax;
}

ATermAppl gsMakeOpIdNameMin() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameMin;
}

ATermAppl gsMakeOpIdNameAbs() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameAbs;
}

ATermAppl gsMakeOpIdNameNeg() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameNeg;
}

ATermAppl gsMakeOpIdNameSucc() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSucc;
}

ATermAppl gsMakeOpIdNamePred() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNamePred;
}

ATermAppl gsMakeOpIdNameDub() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameDub;
}

ATermAppl gsMakeOpIdNameAdd() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameAdd;
}

ATermAppl gsMakeOpIdNameAddC() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameAddC;
}

ATermAppl gsMakeOpIdNameSubt() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSubt;
}

ATermAppl gsMakeOpIdNameGTESubt() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGTESubt;
}

ATermAppl gsMakeOpIdNameGTESubtB() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGTESubtB;
}

ATermAppl gsMakeOpIdNameMult() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameMult;
}

ATermAppl gsMakeOpIdNameMultIR() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameMultIR;
}

ATermAppl gsMakeOpIdNameDiv() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameDiv;
}

ATermAppl gsMakeOpIdNameMod() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameMod;
}

ATermAppl gsMakeOpIdNameDivMod() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameDivMod;
}

ATermAppl gsMakeOpIdNameGDivMod() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGDivMod;
}

ATermAppl gsMakeOpIdNameGGDivMod() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameGGDivMod;
}

ATermAppl gsMakeOpIdNameFirst() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameFirst;
}

ATermAppl gsMakeOpIdNameLast() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameLast;
}

ATermAppl gsMakeOpIdNameExp() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameExp;
}

ATermAppl gsMakeOpIdNameEven() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEven;
}

ATermAppl gsMakeOpIdNameEmptyList() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEmptyList;
}

ATermAppl gsMakeOpIdNameListSize() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameListSize;
}

ATermAppl gsMakeOpIdNameCons() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCons;
}

ATermAppl gsMakeOpIdNameSnoc() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSnoc;
}

ATermAppl gsMakeOpIdNameConcat() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameConcat;
}

ATermAppl gsMakeOpIdNameEltAt() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEltAt;
}

ATermAppl gsMakeOpIdNameHead() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameHead;
}

ATermAppl gsMakeOpIdNameTail() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameTail;
}

ATermAppl gsMakeOpIdNameRHead() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameRHead;
}

ATermAppl gsMakeOpIdNameRTail() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameRTail;
}

ATermAppl gsMakeOpIdNameEltIn() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEltIn;
}

ATermAppl gsMakeOpIdNameSetComp() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSetComp;
}

ATermAppl gsMakeOpIdNameEmptySet() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEmptySet;
}

ATermAppl gsMakeOpIdNameSubSetEq() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSubSetEq;
}

ATermAppl gsMakeOpIdNameSubSet() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSubSet;
}

ATermAppl gsMakeOpIdNameSetUnion() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSetUnion;
}

ATermAppl gsMakeOpIdNameSetDiff() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSetDiff;
}

ATermAppl gsMakeOpIdNameSetIntersect() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSetIntersect;
}

ATermAppl gsMakeOpIdNameSetCompl() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSetCompl;
}

ATermAppl gsMakeOpIdNameBagComp() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameBagComp;
}

ATermAppl gsMakeOpIdNameBag2Set() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameBag2Set;
}

ATermAppl gsMakeOpIdNameSet2Bag() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSet2Bag;
}

ATermAppl gsMakeOpIdNameEmptyBag() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameEmptyBag;
}

ATermAppl gsMakeOpIdNameCount() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameCount;
}

ATermAppl gsMakeOpIdNameSubBagEq() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSubBagEq;
}

ATermAppl gsMakeOpIdNameSubBag() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameSubBag;
}

ATermAppl gsMakeOpIdNameBagUnion() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameBagUnion;
}

ATermAppl gsMakeOpIdNameBagDiff() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameBagDiff;
}

ATermAppl gsMakeOpIdNameBagIntersect() {
  assert(ConstructorFunctionsEnabled);
  return gsOpIdNameBagIntersect;
}


//Creation of operation identifiers for system defined operations.

ATermAppl gsMakeOpIdTrue(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameTrue, gsMakeSortExprBool());
} 

ATermAppl gsMakeOpIdFalse(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameFalse, gsMakeSortExprBool());
} 

ATermAppl gsMakeOpIdNot(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNot,
    gsMakeSortArrow(gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdAnd(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameAnd, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdOr(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameOr, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdImp(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameImp, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprBool(), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdEq(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEq, gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdNeq(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNeq, gsMakeSortArrow2(
    SortExpr, SortExpr, gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdIf(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameIf, gsMakeSortArrow3(
    gsMakeSortExprBool(), SortExpr, SortExpr, SortExpr));
} 

ATermAppl gsMakeOpIdForall(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameForall, gsMakeSortArrow(
    gsMakeSortArrow(SortExpr, gsMakeSortExprBool()), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdExists(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameExists, gsMakeSortArrow(
    gsMakeSortArrow(SortExpr, gsMakeSortExprBool()), gsMakeSortExprBool()));
} 

ATermAppl gsMakeOpIdC1(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameC1, gsMakeSortExprPos());
} 

ATermAppl gsMakeOpIdCDub(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCDub, gsMakeSortArrow2(
    gsMakeSortExprBool(), gsMakeSortExprPos(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdC0(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameC0, gsMakeSortExprNat());
} 

ATermAppl gsMakeOpIdCNat(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCNat,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdCPair(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCPair,
    gsMakeSortArrow2(gsMakeSortExprNat(), gsMakeSortExprNat(),
      gsMakeSortExprNatPair()));
} 

ATermAppl gsMakeOpIdCNeg(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCNeg,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdCInt(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCInt,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprInt()));
}

ATermAppl gsMakeOpIdCReal(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCReal,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprReal()));
} 

ATermAppl gsMakeOpIdPos2Nat(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePos2Nat,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdPos2Int(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePos2Int,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdPos2Real(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNamePos2Real,
    gsMakeSortArrow(gsMakeSortExprPos(), gsMakeSortExprReal()));
} 

ATermAppl gsMakeOpIdNat2Pos(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNat2Pos,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdNat2Int(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNat2Int,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdNat2Real(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameNat2Real,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprReal()));
} 

ATermAppl gsMakeOpIdInt2Pos(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameInt2Pos,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdInt2Nat(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameInt2Nat,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdInt2Real(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameInt2Real,
    gsMakeSortArrow(gsMakeSortExprInt(), gsMakeSortExprReal()));
} 

ATermAppl gsMakeOpIdReal2Pos(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameReal2Pos,
    gsMakeSortArrow(gsMakeSortExprReal(), gsMakeSortExprPos()));
} 

ATermAppl gsMakeOpIdReal2Nat(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameReal2Nat,
    gsMakeSortArrow(gsMakeSortExprReal(), gsMakeSortExprNat()));
} 

ATermAppl gsMakeOpIdReal2Int(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameReal2Int,
    gsMakeSortArrow(gsMakeSortExprReal(), gsMakeSortExprInt()));
} 

ATermAppl gsMakeOpIdLTE(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameLTE,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdLT(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameLT,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGTE(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameGTE,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdGT(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameGT,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdMax(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExprLHS));
  assert(IsPNIRSort(SortExprRHS));
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprReal()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprReal())
  );
  return gsMakeOpId(gsOpIdNameMax,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS,
      IntersectPNIRSorts(SortExprLHS, SortExprRHS)));
}

ATermAppl gsMakeOpIdMin(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameMin,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdAbs(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameAbs,
    gsMakeSortArrow(SortExpr,
      (SortExpr == gsMakeSortExprInt())?gsMakeSortExprNat():SortExpr
    )
  );
} 

ATermAppl gsMakeOpIdNeg(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameNeg,
    gsMakeSortArrow(SortExpr,
      UnitePNIRSorts(SortExpr, gsMakeSortExprInt())
    )
  );
} 

ATermAppl gsMakeOpIdSucc(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  ATermAppl ResultSort;
  if (ATisEqual(SortExpr, gsMakeSortExprNat())) {
    ResultSort = gsMakeSortExprPos();
  } else {
    ResultSort = SortExpr;
  }
  return gsMakeOpId(gsOpIdNameSucc, gsMakeSortArrow(SortExpr, ResultSort));
}

ATermAppl gsMakeOpIdPred(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  ATermAppl ResultSort;
  if (ATisEqual(SortExpr, gsMakeSortExprPos())) {
    ResultSort = gsMakeSortExprNat();
  } else if (ATisEqual(SortExpr, gsMakeSortExprNat())) {
    ResultSort = gsMakeSortExprInt();
  } else {
    ResultSort = SortExpr;
  }
  return gsMakeOpId(gsOpIdNamePred, gsMakeSortArrow(SortExpr, ResultSort));
}

ATermAppl gsMakeOpIdDub(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNISort(SortExpr));
  return gsMakeOpId(gsOpIdNameDub,
    gsMakeSortArrow2(gsMakeSortExprBool(), SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdAdd(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExprLHS));
  assert(IsPNIRSort(SortExprLHS));
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprInt()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprInt())
  );
  assert(
    ATisEqual(SortExprLHS, gsMakeSortExprReal()) ==
    ATisEqual(SortExprRHS, gsMakeSortExprReal())
  );
  return gsMakeOpId(gsOpIdNameAdd,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS,
      IntersectPNIRSorts(SortExprLHS, SortExprRHS)
    )
  );
}

ATermAppl gsMakeOpIdAddC(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameAddC,
    gsMakeSortArrow3(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdSubt(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameSubt,
    gsMakeSortArrow2(SortExpr, SortExpr,
      UnitePNIRSorts(SortExpr, gsMakeSortExprInt())
    )
  );
}

ATermAppl gsMakeOpIdGTESubt(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNSort(SortExpr));
  return gsMakeOpId(gsOpIdNameGTESubt,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdGTESubtB(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameGTESubtB,
    gsMakeSortArrow3(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdMult(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameMult,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdMultIR(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameMultIR,
    gsMakeSortArrow4(gsMakeSortExprBool(), gsMakeSortExprPos(),
      gsMakeSortExprPos(), gsMakeSortExprPos(), gsMakeSortExprPos()));
}

ATermAppl gsMakeOpIdDiv(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNISort(SortExpr));
  return gsMakeOpId(gsOpIdNameDiv,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(),
      UnitePNISorts(SortExpr, gsMakeSortExprNat())
    )
  );
}

ATermAppl gsMakeOpIdMod(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNISort(SortExpr));
  return gsMakeOpId(gsOpIdNameMod,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprPos(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdDivMod(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameDivMod,
    gsMakeSortArrow2(gsMakeSortExprPos(), gsMakeSortExprPos(),
      gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdGDivMod(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameGDivMod,
    gsMakeSortArrow3(gsMakeSortExprNatPair(), gsMakeSortExprBool(),
      gsMakeSortExprPos(), gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdGGDivMod(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameGGDivMod,
    gsMakeSortArrow3(gsMakeSortExprNat(), gsMakeSortExprNat(),
      gsMakeSortExprPos(), gsMakeSortExprNatPair()));
}

ATermAppl gsMakeOpIdFirst(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameFirst,
    gsMakeSortArrow(gsMakeSortExprNatPair(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdLast(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameLast,
    gsMakeSortArrow(gsMakeSortExprNatPair(), gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdExp(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  assert(IsPNIRSort(SortExpr));
  return gsMakeOpId(gsOpIdNameExp,
    gsMakeSortArrow2(SortExpr, gsMakeSortExprNat(), SortExpr));
}

ATermAppl gsMakeOpIdEven(void)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEven,
    gsMakeSortArrow(gsMakeSortExprNat(), gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdEmptyList(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEmptyList, SortExpr);
}

ATermAppl gsMakeOpIdListSize(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameListSize,
    gsMakeSortArrow(SortExpr, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdCons(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCons,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, SortExprRHS));

}

ATermAppl gsMakeOpIdSnoc(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSnoc,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, SortExprLHS));
}

ATermAppl gsMakeOpIdConcat(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameConcat,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdEltAt(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEltAt,
    gsMakeSortArrow2(SortExprDom, gsMakeSortExprNat(), SortExprResult));
}

ATermAppl gsMakeOpIdHead(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameHead,
    gsMakeSortArrow(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdTail(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameTail,
    gsMakeSortArrow(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdRHead(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameRHead,
    gsMakeSortArrow(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdRTail(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameRTail,
    gsMakeSortArrow(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetComp(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSetComp, gsMakeSortArrow(
    gsMakeSortArrow(SortExprDom, gsMakeSortExprBool()), SortExprResult));
}

ATermAppl gsMakeOpIdEmptySet(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEmptySet, SortExpr);
}

ATermAppl gsMakeOpIdEltIn(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEltIn,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdSubSetEq(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubSetEq,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdSubSet(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubSet,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdSetUnion(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSetUnion,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetDiff(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSetDiff,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetIntersect(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSetIntersect,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdSetCompl(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSetCompl,
    gsMakeSortArrow(SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBagComp(ATermAppl SortExprDom, ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameBagComp, gsMakeSortArrow(
    gsMakeSortArrow(SortExprDom, gsMakeSortExprNat()), SortExprResult));
}

ATermAppl gsMakeOpIdEmptyBag(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameEmptyBag, SortExpr);
}

ATermAppl gsMakeOpIdCount(ATermAppl SortExprLHS, ATermAppl SortExprRHS)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameCount,
    gsMakeSortArrow2(SortExprLHS, SortExprRHS, gsMakeSortExprNat()));
}

ATermAppl gsMakeOpIdSubBagEq(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubBagEq,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdSubBag(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSubBag,
    gsMakeSortArrow2(SortExpr, SortExpr, gsMakeSortExprBool()));
}

ATermAppl gsMakeOpIdBagUnion(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameBagUnion,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBagDiff(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameBagDiff,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBagIntersect(ATermAppl SortExpr)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameBagIntersect,
    gsMakeSortArrow2(SortExpr, SortExpr, SortExpr));
}

ATermAppl gsMakeOpIdBag2Set(ATermAppl SortExprDom,
  ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameBag2Set,
    gsMakeSortArrow(SortExprDom, SortExprResult));
}

ATermAppl gsMakeOpIdSet2Bag(ATermAppl SortExprDom,
  ATermAppl SortExprResult)
{
  assert(ConstructorFunctionsEnabled);
  return gsMakeOpId(gsOpIdNameSet2Bag,
    gsMakeSortArrow(SortExprDom, SortExprResult));
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

ATermAppl gsMakeDataExprForall(ATermAppl DataExpr)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(ATAgetArgument(ExprSort, 1), gsMakeSortExprBool()));
  return gsMakeDataAppl(gsMakeOpIdForall(ATAgetArgument(ExprSort, 0)),
    DataExpr);
}

ATermAppl gsMakeDataExprExists(ATermAppl DataExpr)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(ATAgetArgument(ExprSort, 1), gsMakeSortExprBool()));
  return gsMakeDataAppl(gsMakeOpIdExists(ATAgetArgument(ExprSort, 0)),
    DataExpr);
}

ATermAppl gsMakeDataExprEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  assert(!gsIsUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdEq(ExprSort), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprNeq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  ATermAppl ExprSort = gsGetSort(DataExprLHS);
  assert(!gsIsUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdNeq(ExprSort), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprIf(ATermAppl DataExprCond, ATermAppl DataExprThen,
  ATermAppl DataExprElse)
{
  assert(ATisEqual(gsGetSort(DataExprCond), gsMakeSortIdBool())); 
  ATermAppl ExprSort = gsGetSort(DataExprThen);
  assert(!gsIsUnknown(ExprSort));
  assert(ATisEqual(ExprSort, gsGetSort(DataExprElse)));
  return gsMakeDataAppl3(gsMakeOpIdIf(ExprSort), DataExprCond, DataExprThen,
    DataExprElse);
}

ATermAppl gsMakeDataExprC1(void)
{
  return gsMakeOpIdC1();
}

ATermAppl gsMakeDataExprCDub(ATermAppl DataExprBit, ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdCDub(), DataExprBit, DataExprPos);
}

ATermAppl gsMakeDataExprC0(void)
{
  return gsMakeOpIdC0();
}

ATermAppl gsMakeDataExprCNat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl(gsMakeOpIdCNat(), DataExpr);
}

ATermAppl gsMakeDataExprCPair(ATermAppl DataExprFst, ATermAppl DataExprLst)
{
  assert(ATisEqual(gsGetSort(DataExprFst), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprLst), gsMakeSortExprNat()));
  return gsMakeDataAppl2(gsMakeOpIdCPair(), DataExprFst, DataExprLst);
}

ATermAppl gsMakeDataExprCNeg(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl(gsMakeOpIdCNeg(), DataExpr);
}

ATermAppl gsMakeDataExprCInt(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl(gsMakeOpIdCInt(), DataExpr);
}

ATermAppl gsMakeDataExprCReal(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl(gsMakeOpIdCReal(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl(gsMakeOpIdPos2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl(gsMakeOpIdPos2Int(), DataExpr);
}

ATermAppl gsMakeDataExprPos2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprPos()));
  return gsMakeDataAppl(gsMakeOpIdPos2Real(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl(gsMakeOpIdNat2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl(gsMakeOpIdNat2Int(), DataExpr);
}

ATermAppl gsMakeDataExprNat2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl(gsMakeOpIdNat2Real(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl(gsMakeOpIdInt2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl(gsMakeOpIdInt2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprInt2Real(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprInt()));
  return gsMakeDataAppl(gsMakeOpIdInt2Real(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Pos(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl(gsMakeOpIdReal2Pos(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Nat(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl(gsMakeOpIdReal2Nat(), DataExpr);
}

ATermAppl gsMakeDataExprReal2Int(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprReal()));
  return gsMakeDataAppl(gsMakeOpIdReal2Int(), DataExpr);
}

ATermAppl gsMakeDataExprLTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdLTE(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprLT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdLT(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGTE(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGT(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGT(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMax(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdMax(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMin(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdMin(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprAbs(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdAbs(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprNeg(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdNeg(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprSucc(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdSucc(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprPred(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdPred(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprDub(ATermAppl DataExprBit, ATermAppl DataExprNum)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  return gsMakeDataAppl2(gsMakeOpIdDub(gsGetSort(DataExprNum)),
    DataExprBit, DataExprNum);
}

ATermAppl gsMakeDataExprAdd(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdAdd(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprAddC(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdAddC(), DataExprBit, DataExprLHS,
    DataExprRHS);
}

ATermAppl gsMakeDataExprSubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubt(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTESubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdGTESubt(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGTESubtB(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGTESubtB(), DataExprBit, DataExprLHS,
    DataExprRHS);
}

ATermAppl gsMakeDataExprMult(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(gsMakeOpIdMult(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMultIR(ATermAppl DataExprBit, ATermAppl DataExprIR,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprBit), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprIR),  gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl4(gsMakeOpIdMultIR(), DataExprBit, DataExprIR,
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprDiv(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdDiv(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdMod(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprDivMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsMakeSortExprPos()));
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprPos()));
  return gsMakeDataAppl2(gsMakeOpIdDivMod(), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprGDivMod(ATermAppl DataExprPair, ATermAppl DataExprBool,
  ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprPair), gsMakeSortExprNatPair()));
  assert(ATisEqual(gsGetSort(DataExprBool), gsMakeSortExprBool()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGDivMod(), DataExprPair, DataExprBool,
    DataExprPos);
}

ATermAppl gsMakeDataExprGGDivMod(ATermAppl DataExprNat1, ATermAppl DataExprNat2,
  ATermAppl DataExprPos)
{
  assert(ATisEqual(gsGetSort(DataExprNat1), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprNat2), gsMakeSortExprNat()));
  assert(ATisEqual(gsGetSort(DataExprPos), gsMakeSortExprPos()));
  return gsMakeDataAppl3(gsMakeOpIdGGDivMod(), DataExprNat1, DataExprNat2,
    DataExprPos);
}

ATermAppl gsMakeDataExprFirst(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNatPair()));
  return gsMakeDataAppl(gsMakeOpIdFirst(), DataExpr);
}

ATermAppl gsMakeDataExprLast(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNatPair()));
  return gsMakeDataAppl(gsMakeOpIdLast(), DataExpr);
}

ATermAppl gsMakeDataExprExp(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprNat()));
  return gsMakeDataAppl2(gsMakeOpIdExp(gsGetSort(DataExprLHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprEven(ATermAppl DataExpr)
{
  assert(ATisEqual(gsGetSort(DataExpr), gsMakeSortExprNat()));
  return gsMakeDataAppl(gsMakeOpIdEven(), DataExpr);
}

ATermAppl gsMakeDataExprEmptyList(ATermAppl SortExpr)
{
  return gsMakeOpIdEmptyList(SortExpr);
}

ATermAppl gsMakeDataExprCons(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdCons(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprListSize(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdListSize(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprSnoc(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdSnoc(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprConcat(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdConcat(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprEltAt(ATermAppl DataExprLHS, ATermAppl DataExprRHS,
  ATermAppl SortExpr)
{
  assert(ATisEqual(gsGetSort(DataExprRHS), gsMakeSortExprNat()));
  return gsMakeDataAppl2(
    gsMakeOpIdEltAt(gsGetSort(DataExprLHS), SortExpr),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprHead(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdHead(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprTail(ATermAppl DataExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdTail(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprRHead(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdRHead(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprRTail(ATermAppl DataExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdRTail(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprEltIn(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdEltIn(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetComp(ATermAppl DataExpr, ATermAppl SortExprResult)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(!gsIsUnknown(ExprSort));
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(ATAgetArgument(ExprSort, 1), gsMakeSortExprBool()));
  //ExprSort is of the form S -> Bool
  return gsMakeDataAppl(
    gsMakeOpIdSetComp(ATAgetArgument(ExprSort, 0), SortExprResult), DataExpr);
}

ATermAppl gsMakeDataExprEmptySet(ATermAppl SortExpr)
{
  return gsMakeOpIdEmptySet(SortExpr);
}

ATermAppl gsMakeDataExprSubSetEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubSetEq(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSubSet(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubSet(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetUnion(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetUnion(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetDiff(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSetIntersect(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSetCompl(ATermAppl DataExpr)
{
  return gsMakeDataAppl(gsMakeOpIdSetCompl(gsGetSort(DataExpr)), DataExpr);
}

ATermAppl gsMakeDataExprBagComp(ATermAppl DataExpr, ATermAppl SortExprResult)
{
  ATermAppl ExprSort = gsGetSort(DataExpr);
  assert(!gsIsUnknown(ExprSort));
  assert(gsIsSortArrow(ExprSort));
  assert(ATisEqual(ATAgetArgument(ExprSort, 1), gsMakeSortExprNat()));
  //ExprSort is of the form S -> Nat
  return gsMakeDataAppl(
    gsMakeOpIdBagComp(ATAgetArgument(ExprSort, 0), SortExprResult), DataExpr);
}

ATermAppl gsMakeDataExprEmptyBag(ATermAppl SortExpr)
{
  return gsMakeOpIdEmptyBag(SortExpr);
}

ATermAppl gsMakeDataExprCount(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return gsMakeDataAppl2(
    gsMakeOpIdCount(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)),
    DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSubBagEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubBagEq(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprSubBag(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdSubBag(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagUnion(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagUnion(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagDiff(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBagInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(ATisEqual(gsGetSort(DataExprLHS), gsGetSort(DataExprRHS)));
  return gsMakeDataAppl2(
    gsMakeOpIdBagIntersect(gsGetSort(DataExprLHS)), DataExprLHS, DataExprRHS);
}

ATermAppl gsMakeDataExprBag2Set(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdBag2Set(gsGetSort(DataExpr), SortExpr), DataExpr);
}

ATermAppl gsMakeDataExprSet2Bag(ATermAppl DataExpr, ATermAppl SortExpr)
{
  return gsMakeDataAppl(
    gsMakeOpIdSet2Bag(gsGetSort(DataExpr), SortExpr), DataExpr);
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

ATermAppl gsMakeDataAppl4(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4)
{
  return gsMakeDataApplList(DataExpr,
    ATmakeList4((ATerm) DataExprArg1, (ATerm) DataExprArg2,
      (ATerm) DataExprArg3, (ATerm) DataExprArg4));
}

ATermAppl gsMakeDataApplList(ATermAppl DataExpr,
  ATermList DataExprArgs)
{
  ATermAppl Result = DataExpr;
  ATermList l = DataExprArgs;
  while (!ATisEmpty(l))
  {
    Result = gsMakeDataAppl(Result, ATAgetFirst(l));
    l = ATgetNext(l);
  }
  return Result;
}

ATermAppl gsMakeDataExprPos(char *p)
{
  assert(strlen(p) > 0);
  if (!strcmp(p, "1")) {
    return gsMakeDataExprC1();
  } else {
    char *d = gsStringDiv2(p);
    if (gsStringMod2(p) == 0) {
      return gsMakeDataExprCDub(gsMakeDataExprFalse(), gsMakeDataExprPos(d));
    } else {
      return gsMakeDataExprCDub(gsMakeDataExprTrue(), gsMakeDataExprPos(d));
    }
    free(d);
  }
}

ATermAppl gsMakeDataExprPos_int(int p)
{
  assert(p > 0);  
  DECL_A(s,char,NrOfChars(p)+1);
  sprintf(s, "%d", p);
  ATermAppl a = gsMakeDataExprPos(s);
  FREE_A(s);
  return a;
}

ATermAppl gsMakeDataExprNat(char *n)
{
  if (!strcmp(n, "0")) {
    return gsMakeDataExprC0();
  } else {
    return gsMakeDataExprCNat(gsMakeDataExprPos(n));
  }
}

ATermAppl gsMakeDataExprNat_int(int n)
{
  assert(n >= 0);  
  DECL_A(s,char,NrOfChars(n)+1);
  sprintf(s, "%d", n);
  ATermAppl a = gsMakeDataExprNat(s);
  FREE_A(s);
  return a;
}

ATermAppl gsMakeDataExprInt(char *z)
{
  if (!strncmp(z, "-", 1)) {
    return gsMakeDataExprCNeg(gsMakeDataExprPos(z+1));
  } else {
    return gsMakeDataExprCInt(gsMakeDataExprNat(z));
  }
}

ATermAppl gsMakeDataExprInt_int(int z)
{
  DECL_A(s,char,NrOfChars(z)+1);
  sprintf(s, "%d", z);
  ATermAppl a = gsMakeDataExprInt(s);
  FREE_A(s);
  return a;
}

bool gsIsPosConstant(const ATermAppl PosExpr)
{
  if (gsIsOpId(PosExpr)) {
    return ATisEqual(PosExpr, gsMakeOpIdC1());
  } else if (gsIsDataAppl(PosExpr))  {
    ATermAppl Head = gsGetDataExprHead(PosExpr);
    ATermList Args = gsGetDataExprArgs(PosExpr);
    if (ATisEqual(Head, gsMakeOpIdCDub()) && ATgetLength(Args) == 2) {
      ATermAppl ArgBool = ATAelementAt(Args, 0);
      return  
        (ATisEqual(ArgBool, gsMakeOpIdTrue()) || 
         ATisEqual(ArgBool, gsMakeOpIdFalse())
        ) && gsIsPosConstant(ATAelementAt(Args, 1));
    } else return false;
  } else return false;
}

bool gsIsNatConstant(const ATermAppl NatExpr)
{
  if (gsIsOpId(NatExpr)) {
    return ATisEqual(NatExpr, gsMakeOpIdC0());
  } else if (gsIsDataAppl(NatExpr)) {
    ATermAppl Head = gsGetDataExprHead(NatExpr);
    ATermList Args = gsGetDataExprArgs(NatExpr);
    if (ATisEqual(Head, gsMakeOpIdCNat()) && ATgetLength(Args) == 1) {
      return gsIsPosConstant(ATAelementAt(Args, 0));
    } else return false;
  } else return false;
}

bool gsIsIntConstant(const ATermAppl IntExpr)
{
  if (gsIsDataAppl(IntExpr)) {
    ATermAppl Head = gsGetDataExprHead(IntExpr);
    ATermList Args = gsGetDataExprArgs(IntExpr);
    if (ATgetLength(Args) == 1) {
      if (ATisEqual(Head, gsMakeOpIdCInt())) {
        return gsIsNatConstant(ATAelementAt(Args, 0));
      } else if (ATisEqual(Head, gsMakeOpIdCNeg())) {
        return gsIsPosConstant(ATAelementAt(Args, 0));
      } else return false;
    } else return false;
  } else return false;
}

char *gsPosValue(const ATermAppl PosConstant)
{
  assert(gsIsPosConstant(PosConstant));
  char *Result = "";
  if (gsIsOpId(PosConstant)) {
    //PosConstant is 1
    Result = (char *) malloc(2 * sizeof(char));
    Result = strcpy(Result, "1");
  } else {
    //PosConstant is of the form cDub(b)(p), where b and p are boolean and
    //positive constants, respectively
    ATermList Args = gsGetDataExprArgs(PosConstant);
    int Inc = (ATisEqual(ATAelementAt(Args, 0), gsMakeDataExprTrue()))?1:0;
    char *PosValue = gsPosValue(ATAelementAt(Args, 1));
    Result = gsStringDub(PosValue, Inc);
    free(PosValue);
  }
  return Result;
}

int gsPosValue_int(const ATermAppl PosConstant)
{
  char *s = gsPosValue(PosConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}

char *gsNatValue(const ATermAppl NatConstant)
{
  assert(gsIsNatConstant(NatConstant));
  char *Result = "";
  if (gsIsOpId(NatConstant)) {
    //NatConstant is 0
    Result = (char *) malloc(2 * sizeof(char));
    Result = strcpy(Result, "0");
  } else {
    //NatConstant is a positive constant
    Result = gsPosValue(ATAgetArgument(NatConstant, 1));
  }
  return Result;
}

int gsNatValue_int(const ATermAppl NatConstant)
{
  char *s = gsNatValue(NatConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}

char *gsIntValue(const ATermAppl IntConstant)
{
  assert(gsIsIntConstant(IntConstant));
  char *Result = "";
  if (ATisEqual(ATAgetArgument(IntConstant, 0), gsMakeOpIdCInt())) {
    //IntExpr is a natural number
    Result = gsNatValue(ATAgetArgument(IntConstant, 1));
  } else {
    //IntExpr is the negation of a positive number
    char *PosValue = gsPosValue(ATAgetArgument(IntConstant, 1));    
    Result = (char *) malloc((strlen(PosValue)+2) * sizeof(char));
    Result = strcpy(Result, "-");
    Result = strcat(Result, PosValue);
    free(PosValue);
  }
  return Result;
}

int gsIntValue_int(const ATermAppl IntConstant)
{
  char *s = gsIntValue(IntConstant);
  int n = strtol(s, NULL, 10);
  free(s);
  return n;
}


//Multiactions
//------------

ATermAppl gsSortMultAct(ATermAppl MultAct)
{
  assert(gsIsMultAct(MultAct));
  ATermList l = ATLgetArgument(MultAct,0);
  unsigned int len = ATgetLength(l);
  DECL_A(acts,ATerm,len);
  for (unsigned int i=0; !ATisEmpty(l); l=ATgetNext(l),i++)
  {
    acts[i] = ATgetFirst(l);
  }
  //l is empty

  for (unsigned int i=1; i<len; i++)
  {
    unsigned int j = i;
    // XXX comparison is fast but does not define a unique result (i.e. the
    // result is dependent on the specific run of a program)
    while ( acts[j] < acts[j-1] )
    {
      ATerm t = acts[j];
      acts[j] = acts[j-1];
      acts[j-1] = t;
    }
  }

  //l is empty
  for (unsigned int i=0; i<len; i++)
  {
    l = ATinsert(l,acts[len-i-1]);
  }
  FREE_A(acts);
  return gsMakeMultAct(l);
}


//Process expressions
//-------------------

bool gsIsProcExpr(ATermAppl Term)
{
  return
    gsIsParamId(Term) || gsIsAction(Term) || gsIsProcess(Term) ||
    gsIsDelta(Term)   || gsIsTau (Term)   || gsIsSum(Term)     ||
    gsIsBlock(Term)   || gsIsHide(Term)   || gsIsRename(Term)  ||
    gsIsComm(Term)    || gsIsAllow(Term)  || gsIsSync(Term)    ||
    gsIsAtTime(Term)  || gsIsSeq(Term)    || gsIsCond(Term)    ||
    gsIsBInit(Term)   || gsIsMerge(Term)  || gsIsLMerge(Term)  ||
    gsIsChoice(Term);
}


//Mu-calculus formulas
//--------------------

bool gsIsStateFrm(ATermAppl Term)
{
  return
    gsIsDataExpr(Term) || gsIsStateTrue(Term) || gsIsStateFalse(Term) ||
    gsIsStateNot(Term) || gsIsStateAnd(Term) || gsIsStateOr(Term) ||
    gsIsStateImp(Term) || gsIsStateForall(Term) || gsIsStateExists(Term) ||
    gsIsStateMust(Term) || gsIsStateMay(Term) || gsIsStateDelay(Term) ||
    gsIsStateDelayTimed(Term) || gsIsStateVar(Term) || gsIsStateNu(Term) ||
    gsIsStateMu(Term);
}

bool gsIsRegFrm(ATermAppl Term)
{
  return
    gsIsActFrm(Term) || gsIsRegNil(Term) || gsIsRegSeq(Term) ||
    gsIsRegAlt(Term) || gsIsRegTrans(Term) || gsIsRegTransOrNil(Term);
}

bool gsIsActFrm(ATermAppl Term)
{
  return 
    gsIsMultAct(Term) || gsIsDataExpr(Term) || gsIsActTrue(Term) ||
    gsIsActFalse(Term) || gsIsActNot(Term) || gsIsActAnd(Term) ||
    gsIsActOr(Term) || gsIsActImp(Term) || gsIsActForall(Term) ||
    gsIsActExists(Term) || gsIsActAt(Term);
}


//Local declarations
//------------------

bool IsPNSort(ATermAppl SortExpr)
{
  return
    ATisEqual(SortExpr, gsMakeSortExprPos()) ||
    ATisEqual(SortExpr, gsMakeSortExprNat());
}

bool IsPNISort(ATermAppl SortExpr)
{
  return
    IsPNSort(SortExpr) ||
    ATisEqual(SortExpr, gsMakeSortExprInt());
}

bool IsPNIRSort(ATermAppl SortExpr)
{
  return
    IsPNISort(SortExpr) ||
    ATisEqual(SortExpr, gsMakeSortExprReal());
}

ATermAppl IntersectPNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNSort(SortExpr1));
  assert(IsPNSort(SortExpr2));
  ATermAppl seNat = gsMakeSortExprNat();
  if (ATisEqual(SortExpr1, seNat)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seNat)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both of sort Pos
    return gsMakeSortExprPos();
  }
}

ATermAppl IntersectPNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNISort(SortExpr1));
  assert(IsPNISort(SortExpr2));
  ATermAppl seInt = gsMakeSortExprInt();
  if (ATisEqual(SortExpr1, seInt)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seInt)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both PN sorts
    return IntersectPNSorts(SortExpr1, SortExpr2);
  }
}

ATermAppl IntersectPNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNIRSort(SortExpr1));
  assert(IsPNIRSort(SortExpr2));
  ATermAppl seReal = gsMakeSortExprReal();
  if (ATisEqual(SortExpr1, seReal)) {
    //SortExpr1 is the biggest type, return SortExpr2
    return SortExpr2;
  } else if (ATisEqual(SortExpr2, seReal)) {
    //SortExpr2 is the biggest type, return SortExpr1
    return SortExpr1;
  } else {
    //SortExpr1 and SortExpr2 are both PNI sorts
    return IntersectPNISorts(SortExpr1, SortExpr2);
  }
}

ATermAppl UnitePNSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNSort(SortExpr1));
  assert(IsPNSort(SortExpr2));
  ATermAppl seNat = gsMakeSortExprNat();
  if (ATisEqual(SortExpr1, seNat) || ATisEqual(SortExpr2, seNat)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seNat;
  } else {
    //SortExpr1 and SortExpr2 are both of sort Pos
    return gsMakeSortExprPos();
  }
}

ATermAppl UnitePNISorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNISort(SortExpr1));
  assert(IsPNISort(SortExpr2));
  ATermAppl seInt = gsMakeSortExprInt();
  if (ATisEqual(SortExpr1, seInt) || ATisEqual(SortExpr2, seInt)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seInt;
  } else {
    //SortExpr1 and SortExpr2 are both PN sorts
    return UnitePNSorts(SortExpr1, SortExpr2);
  }
}

ATermAppl UnitePNIRSorts(ATermAppl SortExpr1, ATermAppl SortExpr2)
{
  assert(IsPNIRSort(SortExpr1));
  assert(IsPNIRSort(SortExpr2));
  ATermAppl seReal = gsMakeSortExprReal();
  if (ATisEqual(SortExpr1, seReal) || ATisEqual(SortExpr2, seReal)) {
    //SortExpr1 or SortExpr2 is the biggest type, return it
    return seReal;
  } else {
    //SortExpr1 and SortExpr2 are both PNI sorts
    return UnitePNISorts(SortExpr1, SortExpr2);
  }
}

// pbes functions
ATermAppl gsMakeMu()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunMu);
}

ATermAppl gsMakeNu()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunNu);
}

ATermAppl gsMakePBES(ATermList PBEqn_0, ATermAppl PropVarInst_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBES, (ATerm) PBEqn_0, (ATerm) PropVarInst_1);
}

ATermAppl gsMakePBESAnd(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESAnd, (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

ATermAppl gsMakePBESExists(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESExists, (ATerm) DataVarId_0, (ATerm) PBExpr_1);
}

ATermAppl gsMakePBESFalse()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunPBESFalse);
}

ATermAppl gsMakePBESForall(ATermList DataVarId_0, ATermAppl PBExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESForall, (ATerm) DataVarId_0, (ATerm) PBExpr_1);
}

ATermAppl gsMakePBESImp(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESImp, (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

ATermAppl gsMakePBESNot(ATermAppl PBExpr_0)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunPBESNot, (ATerm) PBExpr_0);
}

ATermAppl gsMakePBESOr(ATermAppl PBExpr_0, ATermAppl PBExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESOr, (ATerm) PBExpr_0, (ATerm) PBExpr_1);
}

ATermAppl gsMakePBESTrue()
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunPBESTrue);
}

ATermAppl gsMakePBEqn(ATermAppl FixPoint_0, ATermAppl PropVarDecl_1, ATermAppl PBExpr_2)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunPBEqn, (ATerm) FixPoint_0, (ATerm) PropVarDecl_1, (ATerm) PBExpr_2);
}

ATermAppl gsMakePropVarDecl(ATermAppl String_0, ATermList DataExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPropVarDecl, (ATerm) String_0, (ATerm) DataExpr_1);
}

ATermAppl gsMakePropVarInst(ATermAppl String_0, ATermList DataExpr_1)
{
  assert(ConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPropVarInst, (ATerm) String_0, (ATerm) DataExpr_1);
}

bool gsIsMu(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunMu;
}

bool gsIsNu(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunNu;
}

bool gsIsPBES(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBES;
}

bool gsIsPBESAnd(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESAnd;
}

bool gsIsPBESExists(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESExists;
}

bool gsIsPBESFalse(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESFalse;
}

bool gsIsPBESForall(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESForall;
}

bool gsIsPBESImp(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESImp;
}

bool gsIsPBESNot(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESNot;
}

bool gsIsPBESOr(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESOr;
}

bool gsIsPBESTrue(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBESTrue;
}

bool gsIsPBEqn(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPBEqn;
}

bool gsIsPropVarDecl(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPropVarDecl;
}

bool gsIsPropVarInst(ATermAppl term) {
  assert(ConstructorFunctionsEnabled);
  return ATgetAFun(term) == gsAFunPropVarInst;
}

} // extern "C"
