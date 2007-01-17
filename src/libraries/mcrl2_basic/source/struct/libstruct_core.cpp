#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "libstruct_core.h"
#include "liblowlevel.h"

extern "C" {

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

//Enabling core constructor functions
//------------------------------
//Constant AFun's for each constructor element of the internal ATerm structure.
//sort expressions
static AFun gsAFunSortList;
static AFun gsAFunSortSet;
static AFun gsAFunSortBag;
static AFun gsAFunSortStruct;
static AFun gsAFunSortArrowProd;
static AFun gsAFunSortArrow;
static AFun gsAFunSortId;
static AFun gsAFunStructCons;
static AFun gsAFunStructProj;
static AFun gsAFunNil;
static AFun gsAFunUnknown;
//data expressions
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
static AFun gsAFunDataVarId;
static AFun gsAFunOpId;
static AFun gsAFunBagEnumElt;
static AFun gsAFunWhrDecl;
//data specifications
static AFun gsAFunDataSpec;
static AFun gsAFunSortSpec;
static AFun gsAFunConsSpec;
static AFun gsAFunMapSpec;
static AFun gsAFunDataEqnSpec;
static AFun gsAFunSortRef;
static AFun gsAFunDataEqn;
//multi-actions
static AFun gsAFunMultAct;
static AFun gsAFunParamId;
static AFun gsAFunAction;
static AFun gsAFunActId;
//process expressions
static AFun gsAFunProcess;
static AFun gsAFunDelta;
static AFun gsAFunTau;
static AFun gsAFunSum;
static AFun gsAFunBlock;
static AFun gsAFunHide;
static AFun gsAFunRename;
static AFun gsAFunComm;
static AFun gsAFunAllow;
static AFun gsAFunSync;
static AFun gsAFunAtTime;
static AFun gsAFunSeq;
static AFun gsAFunIfThen;
static AFun gsAFunIfThenElse;
static AFun gsAFunBInit;
static AFun gsAFunMerge;
static AFun gsAFunLMerge;
static AFun gsAFunChoice;
static AFun gsAFunProcVarId;
static AFun gsAFunMultActName;
static AFun gsAFunRenameExpr;
static AFun gsAFunCommExpr;
//mCRL2 specifications
static AFun gsAFunSpecV1;
static AFun gsAFunActSpec;
static AFun gsAFunProcEqnSpec;
static AFun gsAFunLPE;
static AFun gsAFunProcEqn;
static AFun gsAFunLPESummand;
static AFun gsAFunAssignment;
static AFun gsAFunInit;
static AFun gsAFunLPEInit;
//mu-calculus formulas
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
static AFun gsAFunStateYaled;
static AFun gsAFunStateYaledTimed;
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
//PBES's
static AFun gsAFunPBES;
static AFun gsAFunPropVarInst;
static AFun gsAFunPBEqn;
static AFun gsAFunMu;
static AFun gsAFunNu;
static AFun gsAFunPropVarDecl;
static AFun gsAFunPBESTrue;
static AFun gsAFunPBESFalse;
static AFun gsAFunPBESAnd;
static AFun gsAFunPBESOr;
static AFun gsAFunPBESForall;
static AFun gsAFunPBESExists;

//Indicates if gsEnableConstructorFunctions has been called
static bool CoreConstructorFunctionsEnabled = false;

bool gsCoreConstructorFunctionsEnabled(void)
{
  return CoreConstructorFunctionsEnabled;
}

void gsEnableCoreConstructorFunctions(void)
{
  if (!CoreConstructorFunctionsEnabled) {
    //create constructor AFun's
    //sort expressions
    gsAFunSortList         = ATmakeAFun("SortList", 1, ATfalse);
    gsAFunSortSet          = ATmakeAFun("SortSet", 1, ATfalse);
    gsAFunSortBag          = ATmakeAFun("SortBag", 1, ATfalse);
    gsAFunSortStruct       = ATmakeAFun("SortStruct", 1, ATfalse);
    gsAFunSortArrowProd    = ATmakeAFun("SortArrowProd", 2, ATfalse);
    gsAFunSortArrow        = ATmakeAFun("SortArrow", 2, ATfalse);
    gsAFunSortId           = ATmakeAFun("SortId", 1, ATfalse);
    gsAFunStructCons       = ATmakeAFun("StructCons", 3, ATfalse);
    gsAFunStructProj       = ATmakeAFun("StructProj", 2, ATfalse);
    gsAFunUnknown          = ATmakeAFun("Unknown", 0, ATfalse);
    gsAFunNil              = ATmakeAFun("Nil", 0, ATfalse);
    //data expressions
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
    gsAFunDataVarId        = ATmakeAFun("DataVarId", 2, ATfalse);
    gsAFunOpId             = ATmakeAFun("OpId", 2, ATfalse);
    gsAFunBagEnumElt       = ATmakeAFun("BagEnumElt", 2, ATfalse);
    gsAFunWhrDecl          = ATmakeAFun("WhrDecl", 2, ATfalse);
    //data specifications
    gsAFunDataSpec         = ATmakeAFun("DataSpec", 4, ATfalse);
    gsAFunSortSpec         = ATmakeAFun("SortSpec", 1, ATfalse);
    gsAFunConsSpec         = ATmakeAFun("ConsSpec", 1, ATfalse);
    gsAFunMapSpec          = ATmakeAFun("MapSpec", 1, ATfalse);
    gsAFunDataEqnSpec      = ATmakeAFun("DataEqnSpec", 1, ATfalse);
    gsAFunSortRef          = ATmakeAFun("SortRef", 2, ATfalse);
    //multi-actions
    gsAFunMultAct          = ATmakeAFun("MultAct", 1, ATfalse);
    gsAFunParamId          = ATmakeAFun("ParamId", 2, ATfalse);
    gsAFunAction           = ATmakeAFun("Action", 2, ATfalse);
    gsAFunActId            = ATmakeAFun("ActId", 2, ATfalse);
    //process expressions
    gsAFunProcess          = ATmakeAFun("Process", 2, ATfalse);
    gsAFunDelta            = ATmakeAFun("Delta", 0, ATfalse);
    gsAFunTau              = ATmakeAFun("Tau", 0, ATfalse);
    gsAFunSum              = ATmakeAFun("Sum", 2, ATfalse);
    gsAFunBlock            = ATmakeAFun("Block", 2, ATfalse);
    gsAFunHide             = ATmakeAFun("Hide", 2, ATfalse);
    gsAFunRename           = ATmakeAFun("Rename", 2, ATfalse);
    gsAFunComm             = ATmakeAFun("Comm", 2, ATfalse);
    gsAFunAllow            = ATmakeAFun("Allow", 2, ATfalse);
    gsAFunSync             = ATmakeAFun("Sync", 2, ATfalse);
    gsAFunAtTime           = ATmakeAFun("AtTime", 2, ATfalse);
    gsAFunSeq              = ATmakeAFun("Seq", 2, ATfalse);
    gsAFunIfThen           = ATmakeAFun("IfThen", 2, ATfalse);
    gsAFunIfThenElse       = ATmakeAFun("IfThenElse", 3, ATfalse);
    gsAFunBInit            = ATmakeAFun("BInit", 2, ATfalse);
    gsAFunMerge            = ATmakeAFun("Merge", 2, ATfalse);
    gsAFunLMerge           = ATmakeAFun("LMerge", 2, ATfalse);
    gsAFunChoice           = ATmakeAFun("Choice", 2, ATfalse);
    gsAFunProcVarId        = ATmakeAFun("ProcVarId", 2, ATfalse);
    gsAFunMultActName      = ATmakeAFun("MultActName", 1, ATfalse);
    gsAFunRenameExpr       = ATmakeAFun("RenameExpr", 2, ATfalse);
    gsAFunCommExpr         = ATmakeAFun("CommExpr", 2, ATfalse);
    //mCRL2 specifications
    gsAFunSpecV1           = ATmakeAFun("SpecV1", 4, ATfalse);
    gsAFunDataEqn          = ATmakeAFun("DataEqn", 4, ATfalse);
    gsAFunActSpec          = ATmakeAFun("ActSpec", 1, ATfalse);
    gsAFunProcEqnSpec      = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
    gsAFunLPE              = ATmakeAFun("LPE", 3, ATfalse);
    gsAFunProcEqn          = ATmakeAFun("ProcEqn", 4, ATfalse);
    gsAFunLPESummand       = ATmakeAFun("LPESummand", 5, ATfalse);
    gsAFunAssignment       = ATmakeAFun("Assignment", 2, ATfalse);
    gsAFunInit             = ATmakeAFun("Init", 2, ATfalse);
    gsAFunLPEInit          = ATmakeAFun("LPEInit", 2, ATfalse);
    //mu-calculus formulas
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
    gsAFunStateYaled       = ATmakeAFun("StateYaled", 0, ATfalse);
    gsAFunStateYaledTimed  = ATmakeAFun("StateYaledTimed", 1, ATfalse);
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
    // pbes
    gsAFunPBES             = ATmakeAFun("PBES", 3, ATfalse);
    gsAFunPropVarInst      = ATmakeAFun("PropVarInst", 2, ATfalse);
    gsAFunPBEqn            = ATmakeAFun("PBEqn", 3, ATfalse);
    gsAFunMu               = ATmakeAFun("Mu", 0, ATfalse);
    gsAFunNu               = ATmakeAFun("Nu", 0, ATfalse);
    gsAFunPropVarDecl      = ATmakeAFun("PropVarDecl", 2, ATfalse);
    gsAFunPBESTrue         = ATmakeAFun("PBESTrue", 0, ATfalse);
    gsAFunPBESFalse        = ATmakeAFun("PBESFalse", 0, ATfalse);
    gsAFunPBESAnd          = ATmakeAFun("PBESAnd", 2, ATfalse);
    gsAFunPBESOr           = ATmakeAFun("PBESOr", 2, ATfalse);
    gsAFunPBESForall       = ATmakeAFun("PBESForall", 2, ATfalse);
    gsAFunPBESExists       = ATmakeAFun("PBESExists", 2, ATfalse);

    //protect constructor AFun's
    //sort expressions
    ATprotectAFun(gsAFunSortList);
    ATprotectAFun(gsAFunSortSet);
    ATprotectAFun(gsAFunSortBag);
    ATprotectAFun(gsAFunSortStruct);
    ATprotectAFun(gsAFunSortArrowProd);
    ATprotectAFun(gsAFunSortArrow);
    ATprotectAFun(gsAFunSortId);
    ATprotectAFun(gsAFunStructCons);
    ATprotectAFun(gsAFunStructProj);
    ATprotectAFun(gsAFunNil);
    ATprotectAFun(gsAFunUnknown);
    //data expressions
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
    ATprotectAFun(gsAFunDataVarId);
    ATprotectAFun(gsAFunOpId);
    ATprotectAFun(gsAFunBagEnumElt);
    ATprotectAFun(gsAFunWhrDecl);
    //data specifications
    ATprotectAFun(gsAFunDataSpec);
    ATprotectAFun(gsAFunSortSpec);
    ATprotectAFun(gsAFunConsSpec);
    ATprotectAFun(gsAFunMapSpec);
    ATprotectAFun(gsAFunDataEqnSpec);
    ATprotectAFun(gsAFunSortRef);
    ATprotectAFun(gsAFunDataEqn);
    //multi-actions
    ATprotectAFun(gsAFunMultAct);
    ATprotectAFun(gsAFunParamId);
    ATprotectAFun(gsAFunAction);
    ATprotectAFun(gsAFunActId);
    //process expressions
    ATprotectAFun(gsAFunProcess);
    ATprotectAFun(gsAFunDelta);
    ATprotectAFun(gsAFunTau);
    ATprotectAFun(gsAFunSum);
    ATprotectAFun(gsAFunBlock);
    ATprotectAFun(gsAFunHide);
    ATprotectAFun(gsAFunRename);
    ATprotectAFun(gsAFunComm);
    ATprotectAFun(gsAFunAllow);
    ATprotectAFun(gsAFunSync);
    ATprotectAFun(gsAFunAtTime);
    ATprotectAFun(gsAFunSeq);
    ATprotectAFun(gsAFunIfThen);
    ATprotectAFun(gsAFunIfThenElse);
    ATprotectAFun(gsAFunBInit);
    ATprotectAFun(gsAFunMerge);
    ATprotectAFun(gsAFunLMerge);
    ATprotectAFun(gsAFunChoice);
    ATprotectAFun(gsAFunProcVarId);
    ATprotectAFun(gsAFunMultActName);
    ATprotectAFun(gsAFunRenameExpr);
    ATprotectAFun(gsAFunCommExpr);
    //mCRL2 specifications
    ATprotectAFun(gsAFunSpecV1);
    ATprotectAFun(gsAFunActSpec);
    ATprotectAFun(gsAFunProcEqnSpec);
    ATprotectAFun(gsAFunLPE);
    ATprotectAFun(gsAFunProcEqn);
    ATprotectAFun(gsAFunLPESummand);
    ATprotectAFun(gsAFunAssignment);
    ATprotectAFun(gsAFunInit);
    ATprotectAFun(gsAFunLPEInit);
    //mu-calculus formulas
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
    ATprotectAFun(gsAFunStateYaled);
    ATprotectAFun(gsAFunStateYaledTimed);
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
    //PBES's
    ATprotectAFun(gsAFunPBES);
    ATprotectAFun(gsAFunPropVarInst);
    ATprotectAFun(gsAFunPBEqn);
    ATprotectAFun(gsAFunMu);
    ATprotectAFun(gsAFunNu);
    ATprotectAFun(gsAFunPropVarDecl);
    ATprotectAFun(gsAFunPBESTrue);
    ATprotectAFun(gsAFunPBESFalse);
    ATprotectAFun(gsAFunPBESAnd);
    ATprotectAFun(gsAFunPBESOr);
    ATprotectAFun(gsAFunPBESForall);
    ATprotectAFun(gsAFunPBESExists);

    CoreConstructorFunctionsEnabled = true;
  }
}

//Creation of all constructor elements of the internal structure
//--------------------------------------------------------------

//sort expressions

ATermAppl gsMakeSortList(ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortList, (ATerm) SortExpr);
}

ATermAppl gsMakeSortSet(ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSet, (ATerm) SortExpr);
}

ATermAppl gsMakeSortBag(ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortBag, (ATerm) SortExpr);
}

ATermAppl gsMakeSortStruct(ATermList StructConss)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortStruct, (ATerm) StructConss);
}

ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrowProd, (ATerm) SortExprsDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortArrow, (ATerm) SortExprDomain,
    (ATerm) SortExprResult);
}

ATermAppl gsMakeSortId(ATermAppl Name)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortId, (ATerm) Name);
}

ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecNameOrNil)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStructCons, (ATerm) ConsName, (ATerm) StructProjs,
    (ATerm) RecNameOrNil);
}

ATermAppl gsMakeStructProj(ATermAppl ProjNameOrNil, ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStructProj, (ATerm) ProjNameOrNil,
    (ATerm) SortExpr);
}

ATermAppl gsMakeNil()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunNil);
}

ATermAppl gsMakeUnknown()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunUnknown);
}

//data expressions

ATermAppl gsMakeId(ATermAppl Name)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunId, (ATerm) Name);
}

ATermAppl gsMakeDataApplProd(ATermAppl DataExpr, ATermList DataArgs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataApplProd, (ATerm) DataExpr, (ATerm) DataArgs);
}

ATermAppl gsMakeDataAppl(ATermAppl DataExpr, ATermAppl DataArg)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataAppl, (ATerm) DataExpr, (ATerm) DataArg);
}

ATermAppl gsMakeNumber(ATermAppl Number, ATermAppl SortExprOrUnknown)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunNumber, (ATerm) Number, (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeListEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunListEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetEnum, (ATerm) DataExprs,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeBagEnum(ATermList BagEnumElts, ATermAppl SortExprOrUnknown)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnum, (ATerm) BagEnumElts,
    (ATerm) SortExprOrUnknown);
}

ATermAppl gsMakeSetBagComp(ATermAppl DataVarId, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSetBagComp, (ATerm) DataVarId, (ATerm) DataExpr);
}

ATermAppl gsMakeForall(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunForall, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeExists(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunExists, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeLambda(ATermList DataVarIds, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLambda, (ATerm) DataVarIds, (ATerm) DataExpr);
}

ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhr, (ATerm) DataExpr, (ATerm) WhrDecls);
}

ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunDataVarId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunOpId, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBagEnumElt, (ATerm) DataExpr, (ATerm) Multiplicity);
}

ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunWhrDecl, (ATerm) Name, (ATerm) DataExpr);
}

//data specifications

ATermAppl gsMakeDataSpec(ATermAppl SortSpec, ATermAppl ConsSpec,
  ATermAppl MapSpec, ATermAppl DataEqnSpec)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunDataSpec, (ATerm) SortSpec, (ATerm) ConsSpec,
    (ATerm) MapSpec, (ATerm) DataEqnSpec);
}

ATermAppl gsMakeSortSpec(ATermList SortDecls)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunSortSpec, (ATerm) SortDecls);
}

ATermAppl gsMakeConsSpec(ATermList OpIds)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunConsSpec, (ATerm) OpIds);
}

ATermAppl gsMakeMapSpec(ATermList OpIds)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMapSpec, (ATerm) OpIds);
}

ATermAppl gsMakeDataEqnSpec(ATermList DataEqns)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunDataEqnSpec, (ATerm) DataEqns);
}

ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSortRef, (ATerm) Name, (ATerm) SortExpr);
}

ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl BoolExprOrNil,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunDataEqn, (ATerm) DataVarIds, (ATerm) BoolExprOrNil,
    (ATerm) DataExprLHS, (ATerm) DataExprRHS);
}

//multi-actions

ATermAppl gsMakeMultAct(ATermList Actions)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMultAct, (ATerm) Actions);
}

ATermAppl gsMakeParamId(ATermAppl Name, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunParamId, (ATerm) Name, (ATerm) DataExprs);
}

ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAction, (ATerm) ActId, (ATerm) DataExprs);
}

ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActId, (ATerm) Name, (ATerm) SortExprs);
}

//process expressions

ATermAppl gsMakeProcess(ATermAppl ProcId, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcess, (ATerm) ProcId, (ATerm) DataExprs);
}

ATermAppl gsMakeDelta()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunDelta);
}

ATermAppl gsMakeTau()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunTau);
}

ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSum, (ATerm) DataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeBlock(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBlock, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeHide(ATermList ActNames, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunHide, (ATerm) ActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeRename(ATermList RenameExprs, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRename, (ATerm) RenameExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeComm(ATermList CommExprs, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunComm, (ATerm) CommExprs, (ATerm) ProcExpr);
}

ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAllow, (ATerm) MultActNames, (ATerm) ProcExpr);
}

ATermAppl gsMakeSync(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSync, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl TimeExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAtTime, (ATerm) ProcExpr, (ATerm) TimeExpr);
}

ATermAppl gsMakeSeq(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunSeq, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeIfThen(ATermAppl BoolExprIf, ATermAppl ProcExprThen)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunIfThen, (ATerm) BoolExprIf, (ATerm) ProcExprThen);
}

ATermAppl gsMakeIfThenElse(ATermAppl BoolExprIf, ATermAppl ProcExprThen,
  ATermAppl ProcExprElse)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunIfThenElse, (ATerm) BoolExprIf, (ATerm) ProcExprThen,
    (ATerm) ProcExprElse);
}

ATermAppl gsMakeBInit(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunBInit, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeLMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLMerge, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeChoice(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunChoice, (ATerm) ProcExprLHS, (ATerm) ProcExprRHS);
}

ATermAppl gsMakeProcVarId(ATermAppl Name, ATermList SortExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunProcVarId, (ATerm) Name, (ATerm) SortExprs);
}

ATermAppl gsMakeMultActName(ATermList ActNames)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunMultActName, (ATerm) ActNames);
}

ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRenameExpr, (ATerm) FromName, (ATerm) ToName);
}

ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActNameOrNil)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunCommExpr, (ATerm) MultActName, (ATerm) ActNameOrNil);
}

//mCRL2 specifications

ATermAppl gsMakeSpecV1(ATermAppl DataSpec, ATermAppl ActSpec,
  ATermAppl ProcEqnSpec, ATermAppl Init)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunSpecV1, (ATerm) DataSpec, (ATerm) ActSpec,
    (ATerm) ProcEqnSpec, (ATerm) Init);
}

ATermAppl gsMakeActSpec(ATermList ActIds)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunActSpec, (ATerm) ActIds);
}

ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunProcEqnSpec, (ATerm) ProcEqns);
}

ATermAppl gsMakeLPE(ATermList GlobDataVarIds, ATermList ProcDataVarIds,
  ATermList LPESummands)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunLPE, (ATerm) GlobDataVarIds, (ATerm) ProcDataVarIds,
    (ATerm) LPESummands);
}

ATermAppl gsMakeProcEqn(ATermList GlobDataVarIds, ATermAppl ProcVarId,
  ATermList ProcDataVarIds, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl4(gsAFunProcEqn, (ATerm) GlobDataVarIds, (ATerm) ProcVarId,
    (ATerm) ProcDataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeLPESummand(ATermList DataVarIds, ATermAppl BoolExpr,
  ATermAppl MultiAction, ATermAppl TimeExprOrNil, ATermList Assignments)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl5(gsAFunLPESummand, (ATerm) DataVarIds, (ATerm) BoolExpr,
    (ATerm) MultiAction, (ATerm) TimeExprOrNil, (ATerm) Assignments);
}

ATermAppl gsMakeAssignment(ATermAppl DataVarId, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunAssignment, (ATerm) DataVarId, (ATerm) DataExpr);
}

ATermAppl gsMakeInit(ATermList GlobDataVarIds, ATermAppl ProcExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunInit, (ATerm) GlobDataVarIds, (ATerm) ProcExpr);
}

ATermAppl gsMakeLPEInit(ATermList GlobDataVarIds, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunLPEInit, (ATerm) GlobDataVarIds, (ATerm) DataExprs);
}

//mu-calculus formulas

ATermAppl gsMakeStateTrue()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateTrue);
}

ATermAppl gsMakeStateFalse()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateFalse);
}

ATermAppl gsMakeStateNot(ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunStateNot, (ATerm) StateFrm);
}

ATermAppl gsMakeStateAnd(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateAnd, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateOr(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateOr, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateImp(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateImp, (ATerm) StateFrmLHS, (ATerm) StateFrmRHS);
}

ATermAppl gsMakeStateForall(ATermList DataVarIds, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateForall, (ATerm) DataVarIds, (ATerm) StateFrm);
}

ATermAppl gsMakeStateExists(ATermList DataVarIds, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateExists, (ATerm) DataVarIds, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMust(ATermAppl RegFrm, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateMust, (ATerm) RegFrm, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMay(ATermAppl RegFrm, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateMay, (ATerm) RegFrm, (ATerm) StateFrm);
}

ATermAppl gsMakeStateYaled()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateYaled);
}

ATermAppl gsMakeStateYaledTimed(ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunStateYaledTimed, (ATerm) DataExpr);
}

ATermAppl gsMakeStateDelay()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunStateDelay);
}

ATermAppl gsMakeStateDelayTimed(ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunStateDelayTimed, (ATerm) DataExpr);
}

ATermAppl gsMakeStateVar(ATermAppl VarName, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunStateVar, (ATerm) VarName, (ATerm) DataExprs);
}

ATermAppl gsMakeStateNu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStateNu, (ATerm) VarName, (ATerm) DataVarIdInits, (ATerm) StateFrm);
}

ATermAppl gsMakeStateMu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunStateMu, (ATerm) VarName, (ATerm) DataVarIdInits, (ATerm) StateFrm);
}

ATermAppl gsMakeDataVarIdInit(ATermAppl VarName, ATermAppl SortExpr, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunDataVarIdInit, (ATerm) VarName, (ATerm) SortExpr, (ATerm) DataExpr);
}

ATermAppl gsMakeRegNil()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunRegNil);
}

ATermAppl gsMakeRegSeq(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRegSeq, (ATerm) RegFrmLHS, (ATerm) RegFrmRHS);
}

ATermAppl gsMakeRegAlt(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunRegAlt, (ATerm) RegFrmLHS, (ATerm) RegFrmRHS);
}

ATermAppl gsMakeRegTrans(ATermAppl RegFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunRegTrans, (ATerm) RegFrm);
}

ATermAppl gsMakeRegTransOrNil(ATermAppl RegFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunRegTransOrNil, (ATerm) RegFrm);
}

ATermAppl gsMakeActTrue()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunActTrue);
}

ATermAppl gsMakeActFalse()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunActFalse);
}

ATermAppl gsMakeActNot(ATermAppl ActFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl1(gsAFunActNot, (ATerm) ActFrm);
}

ATermAppl gsMakeActAnd(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActAnd, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActOr(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActOr, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActImp(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActImp, (ATerm) ActFrmLHS, (ATerm) ActFrmRHS);
}

ATermAppl gsMakeActForall(ATermList DataVarIds, ATermAppl ActFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActForall, (ATerm) DataVarIds, (ATerm) ActFrm);
}

ATermAppl gsMakeActExists(ATermList DataVarIds, ATermAppl ActFrm)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActExists, (ATerm) DataVarIds, (ATerm) ActFrm);
}

ATermAppl gsMakeActAt(ATermAppl ActFrm, ATermAppl DataExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunActAt, (ATerm) ActFrm, (ATerm) DataExpr);
}

//PBES's

ATermAppl gsMakePBES(ATermAppl DataSpec, ATermList PBEqn, ATermAppl PropVarInst)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunPBES, (ATerm) DataSpec, (ATerm) PBEqn, (ATerm) PropVarInst);
}

ATermAppl gsMakePropVarInst(ATermAppl Name, ATermList DataExprs)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPropVarInst, (ATerm) Name, (ATerm) DataExprs);
}

ATermAppl gsMakePBEqn(ATermAppl FixPoint, ATermAppl PropVarDecl, ATermAppl PBExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl3(gsAFunPBEqn, (ATerm) FixPoint, (ATerm) PropVarDecl, (ATerm) PBExpr);
}

ATermAppl gsMakePropVarDecl(ATermAppl Name, ATermList DataVarIds)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPropVarDecl, (ATerm) Name, (ATerm) DataVarIds);
}

ATermAppl gsMakeMu()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunMu);
}

ATermAppl gsMakeNu()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunNu);
}

ATermAppl gsMakePBESTrue()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunPBESTrue);
}

ATermAppl gsMakePBESFalse()
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl0(gsAFunPBESFalse);
}

ATermAppl gsMakePBESAnd(ATermAppl PBExprLHS, ATermAppl PBExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESAnd, (ATerm) PBExprLHS, (ATerm) PBExprRHS);
}

ATermAppl gsMakePBESOr(ATermAppl PBExprLHS, ATermAppl PBExprRHS)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESOr, (ATerm) PBExprLHS, (ATerm) PBExprRHS);
}

ATermAppl gsMakePBESForall(ATermList DataVarId, ATermAppl PBExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESForall, (ATerm) DataVarId, (ATerm) PBExpr);
}

ATermAppl gsMakePBESExists(ATermList DataVarId, ATermAppl PBExpr)
{
  assert(CoreConstructorFunctionsEnabled);
  return ATmakeAppl2(gsAFunPBESExists, (ATerm) DataVarId, (ATerm) PBExpr);
}


//Recognisers of all constructor elements of the internal structure
//-----------------------------------------------------------------

//sort expressions
bool gsIsSortList(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortList;
}

bool gsIsSortSet(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSet;
}

bool gsIsSortBag(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortBag;
}

bool gsIsSortStruct(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortStruct;
}

bool gsIsSortArrowProd(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrowProd;
}

bool gsIsSortArrow(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortArrow;
}

bool gsIsSortId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortId;
}

bool gsIsStructCons(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructCons;
}

bool gsIsStructProj(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStructProj;
}

bool gsIsNil(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNil;
}

bool gsIsUnknown(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunUnknown;
}

//data expressions
bool gsIsId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunId;
}

bool gsIsDataApplProd(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataApplProd;
}

bool gsIsDataAppl(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataAppl;
}

bool gsIsNumber(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNumber;
}

bool gsIsListEnum(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunListEnum;
}

bool gsIsSetEnum(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetEnum;
}

bool gsIsBagEnum(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnum;
}

bool gsIsSetBagComp(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSetBagComp;
}

bool gsIsForall(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunForall;
}

bool gsIsExists(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunExists;
}

bool gsIsLambda(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLambda;
}

bool gsIsWhr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhr;
}

bool gsIsDataVarId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarId;
}

bool gsIsOpId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunOpId;
}

bool gsIsBagEnumElt(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBagEnumElt;
}

bool gsIsWhrDecl(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunWhrDecl;
}

//data specifications
bool gsIsDataSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataSpec;
}

bool gsIsSortSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortSpec;
}

bool gsIsConsSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunConsSpec;
}

bool gsIsMapSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMapSpec;
}

bool gsIsDataEqnSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqnSpec;
}

bool gsIsSortRef(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSortRef;
}

bool gsIsDataEqn(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataEqn;
}

//multi-actions
bool gsIsMultAct(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMultAct;
}

bool gsIsParamId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunParamId;
}

bool gsIsAction(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAction;
}

bool gsIsActId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActId;
}

//process expressions
bool gsIsProcess(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcess;
}

bool gsIsDelta(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDelta;
}

bool gsIsTau(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunTau;
}

bool gsIsSum(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSum;
}

bool gsIsBlock(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBlock;
}

bool gsIsHide(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunHide;
}

bool gsIsRename(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRename;
}

bool gsIsComm(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunComm;
}

bool gsIsAllow(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAllow;
}

bool gsIsSync(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSync;
}

bool gsIsAtTime(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAtTime;
}

bool gsIsSeq(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSeq;
}

bool gsIsIfThen(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunIfThen;
}

bool gsIsIfThenElse(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunIfThenElse;
}

bool gsIsBInit(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunBInit;
}

bool gsIsMerge(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMerge;
}

bool gsIsLMerge(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLMerge;
}

bool gsIsChoice(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunChoice;
}

bool gsIsProcVarId(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcVarId;
}

bool gsIsMultActName(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMultActName;
}

bool gsIsRenameExpr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRenameExpr;
}

bool gsIsCommExpr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunCommExpr;
}

//mCRL2 specifications
bool gsIsSpecV1(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunSpecV1;
}

bool gsIsActSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActSpec;
}

bool gsIsProcEqnSpec(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqnSpec;
}

bool gsIsLPE(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPE;
}

bool gsIsProcEqn(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunProcEqn;
}

bool gsIsLPESummand(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPESummand;
}

bool gsIsAssignment(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunAssignment;
}

bool gsIsInit(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunInit;
}

bool gsIsLPEInit(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunLPEInit;
}

//mu-calculus formulas
bool gsIsStateTrue(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateTrue;
}

bool gsIsStateFalse(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateFalse;
}

bool gsIsStateNot(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateNot;
}

bool gsIsStateAnd(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateAnd;
}

bool gsIsStateOr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateOr;
}

bool gsIsStateImp(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateImp;
}

bool gsIsStateForall(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateForall;
}

bool gsIsStateExists(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateExists;
}

bool gsIsStateMust(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMust;
}

bool gsIsStateMay(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMay;
}

bool gsIsStateYaled(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateYaled;
}

bool gsIsStateYaledTimed(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateYaledTimed;
}

bool gsIsStateDelay(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateDelay;
}

bool gsIsStateDelayTimed(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateDelayTimed;
}

bool gsIsStateVar(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateVar;
}

bool gsIsStateNu(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateNu;
}

bool gsIsStateMu(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunStateMu;
}

bool gsIsDataVarIdInit(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunDataVarIdInit;
}

bool gsIsRegNil(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegNil;
}

bool gsIsRegSeq(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegSeq;
}

bool gsIsRegAlt(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegAlt;
}

bool gsIsRegTrans(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegTrans;
}

bool gsIsRegTransOrNil(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunRegTransOrNil;
}

bool gsIsActTrue(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActTrue;
}

bool gsIsActFalse(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActFalse;
}

bool gsIsActNot(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActNot;
}

bool gsIsActAnd(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActAnd;
}

bool gsIsActOr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActOr;
}

bool gsIsActImp(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActImp;
}

bool gsIsActForall(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActForall;
}

bool gsIsActExists(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActExists;
}

bool gsIsActAt(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunActAt;
}

//PBES's
bool gsIsPBES(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBES;
}

bool gsIsPropVarInst(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPropVarInst;
}

bool gsIsPBEqn(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBEqn;
}

bool gsIsPropVarDecl(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPropVarDecl;
}

bool gsIsMu(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunMu;
}

bool gsIsNu(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunNu;
}

bool gsIsPBESTrue(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESTrue;
}

bool gsIsPBESFalse(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESFalse;
}

bool gsIsPBESAnd(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESAnd;
}

bool gsIsPBESOr(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESOr;
}

bool gsIsPBESForall(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESForall;
}

bool gsIsPBESExists(ATermAppl Term) {
  assert(CoreConstructorFunctionsEnabled);
  return ATgetAFun(Term) == gsAFunPBESExists;
}

} // extern "C"
