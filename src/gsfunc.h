#include <stdbool.h>
#include "aterm2.h"

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

//String manipulation
//-------------------
//
//Re-implementation of strdup (because it is not part of the C99 standard)
#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
extern char *strdup(const char *s);
#endif

//Exception handling macro's
//--------------------------
//
//When these are used the label 'finally' should be declared.
//If a value 'x' should be returned, the variable 'result' has to be declared, 
//and the type of 'x' should be convertible to that of 'result'.
//If a message, should be printed, the message has to be able to be used by
//function printf.

#define throw                      goto finally
//model C++ throw by a 'goto finally' statement

#define ThrowV(x)                  Result = x; throw
//store x in result and throw an exception

#define ThrowM0(s)                 fprintf(stderr, s); throw
//print message s and throw an exception

#define ThrowVM0(x, s)             fprintf(stderr, s); ThrowV(x)
//print message s and throw an exception with value x

#define ThrowVM1(x, s, a1)         fprintf(stderr, s, a1); ThrowV(x)
//print message s with argument a1, and throw an exception with value x

#define ThrowVM2(x, s, a1, a2)     fprintf(stderr, s, a1, a2); ThrowV(x)
//print message s with argument a1 and a2,  and throw an exception with value x

#define ThrowVM3(x, s, a1, a2, a3) fprintf(stderr, s, a1, a2, a3); ThrowV(x)
//print message s with argument a1, a2 and a3,  and throw an exception with
//value x

//ATmake extensions
//-----------------
//
//Implements the making of all elements of the structure of ATerms to improve 
//readability and to minimise type casts in the rest of the code

ATermAppl gsMakeIdString(char *s);
ATermAppl gsMakeNumberString(char *s);
ATermAppl gsMakeNil();
ATermAppl gsMakeUnknown();
ATermAppl gsMakeSortId(ATermAppl Name);
ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprOrUnknowns);
ATermAppl gsMakeProcId(ATermAppl Name);
ATermAppl gsMakeDataVar(ATermAppl Name, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeNumber(ATermAppl NumberString, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeSpec(ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl EqnSpec, ATermAppl ActSpec, ATermAppl ProcSpec, ATermAppl Init);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeSortDeclStandard(ATermAppl Name);
ATermAppl gsMakeSortDeclRef(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeConsSpec(ATermList OpDecls);
ATermAppl gsMakeMapSpec(ATermList OpDecls);
ATermAppl gsMakeOpDecl(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeEqnSpec(ATermList EqnSects);
ATermAppl gsMakeEqnSect(ATermList DataVarDecls, ATermList EqnDecls);
ATermAppl gsMakeDataVarDecl(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeEqnDecl(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeActSpec(ATermList ActDecls);
ATermAppl gsMakeActDecl(ATermAppl Name, ATermList SortExprs);
ATermAppl gsMakeProcSpec(ATermList ProcDecls);
ATermAppl gsMakeProcDecl(ATermAppl Name, ATermList DataVarDecls, ATermAppl ProcExpr);
ATermAppl gsMakeInit(ATermAppl ProcExpr);
ATermAppl gsMakeSortBool();
ATermAppl gsMakeSortPos();
ATermAppl gsMakeSortNat();
ATermAppl gsMakeSortInt();
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortArrow(ATermList Domain, ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecNameOrNil);
ATermAppl gsMakeStructProj(ATermAppl ProjNameOrNil, ATermList SortExprs);
ATermAppl gsMakeDataVarOpId(ATermAppl Name);
ATermAppl gsMakeDataAppl(ATermAppl DataExpr, ATermList DataArgs);
ATermAppl gsMakeLambda(ATermList IdsDecls, ATermAppl DataExpr);
ATermAppl gsMakeForall(ATermList IdsDecls, ATermAppl DataExpr);
ATermAppl gsMakeExists(ATermList IdsDecls, ATermAppl DataExpr);
ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls);
ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr);
ATermAppl gsMakeListEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeSetEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeBagEnum(ATermList BagEnumElts, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity);
ATermAppl gsMakeSetBagComp(ATermAppl IdDecl, ATermAppl DataExpr);
ATermAppl gsMakeEmptyList(ATermAppl SortExprOrUnknown);
ATermAppl gsMakeEmptySetBag(ATermAppl SortExprOrUnknown);
ATermAppl gsMakeTrue();
ATermAppl gsMakeFalse();
ATermAppl gsMakeExclam();
ATermAppl gsMakeHash();
ATermAppl gsMakeDot();
ATermAppl gsMakeStar();
ATermAppl gsMakeDiv();
ATermAppl gsMakeMod();
ATermAppl gsMakePlus();
ATermAppl gsMakeMinus();
ATermAppl gsMakeLT();
ATermAppl gsMakeGT();
ATermAppl gsMakeLTE();
ATermAppl gsMakeGTE();
ATermAppl gsMakeIn();
ATermAppl gsMakeCons();
ATermAppl gsMakeSnoc();
ATermAppl gsMakeConcat();
ATermAppl gsMakeEq();
ATermAppl gsMakeNeq();
ATermAppl gsMakeAnd();
ATermAppl gsMakeOr();
ATermAppl gsMakeImp();
ATermAppl gsMakeActProcRef(ATermAppl Name, ATermList DataExprs);
ATermAppl gsMakeAct(ATermAppl ActId, ATermList DataExprs);
ATermAppl gsMakeProcRef(ATermAppl ProcId, ATermList DataExprs);
ATermAppl gsMakeDelta();
ATermAppl gsMakeTau();
ATermAppl gsMakeSum(ATermList IdsDecls, ATermAppl ProcExpr);
ATermAppl gsMakeRestrict(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeHide(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeRename(ATermList RenExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeComm(ATermList CommExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeSync(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl DataExpr);
ATermAppl gsMakeSeq(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeCond(ATermAppl DataExpr, ATermAppl PEThen, ATermAppl PEElse);
ATermAppl gsMakeBInit(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMerge(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeLMerge(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeChoice(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMultActName(ATermList ActNames);
ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActNameOrNil);
ATermAppl gsMakeRenExpr(ATermAppl FromName, ATermAppl ToName);

//strcmp extensions
//------------------
//
//Implements the comparison of the function names of all elements of the 
//structure of ATerms to improve readability

bool gsIsNil(char *s);
bool gsIsUnknown(char *s);
bool gsIsSortId(char *s);
bool gsIsOpId(char *s);
bool gsIsActId(char *s);
bool gsIsProcId(char *s);
bool gsIsDataVar(char *s);
bool gsIsNumber(char *s);
bool gsIsSpec(char *s);
bool gsIsSortSpec(char *s);
bool gsIsSortDeclStandard(char *s);
bool gsIsSortDeclRef(char *s);
bool gsIsConsSpec(char *s);
bool gsIsMapSpec(char *s);
bool gsIsOpDecl(char *s);
bool gsIsEqnSpec(char *s);
bool gsIsDataVarDecl(char *s);
bool gsIsEqnDecl(char *s);
bool gsIsActSpec(char *s);
bool gsIsActDecl(char *s);
bool gsIsProcSpec(char *s);
bool gsIsProcDecl(char *s);
bool gsIsInit(char *s);
bool gsIsSortBool(char *s);
bool gsIsSortPos(char *s);
bool gsIsSortNat(char *s);
bool gsIsSortInt(char *s);
bool gsIsSortList(char *s);
bool gsIsSortSet(char *s);
bool gsIsSortBag(char *s);
bool gsIsSortArrow(char *s);
bool gsIsSortStruct(char *s);
bool gsIsStructCons(char *s);
bool gsIsStructProj(char *s);
bool gsIsDataAppl(char *s);
bool gsIsLambda(char *s);
bool gsIsForall(char *s);
bool gsIsExists(char *s);
bool gsIsWhr(char *s);
bool gsIsWhrDecl(char *s);
bool gsIsListEnum(char *s);
bool gsIsSetEnum(char *s);
bool gsIsBagEnum(char *s);
bool gsIsBagEnumElt(char *s);
bool gsIsSetBagComp(char *s);
bool gsIsEmptyList(char *s);
bool gsIsEmptySetBag(char *s);
bool gsIsTrue(char *s);
bool gsIsFalse(char *s);
bool gsIsExclam(char *s);
bool gsIsHash(char *s);
bool gsIsDot(char *s);
bool gsIsStar(char *s);
bool gsIsDiv(char *s);
bool gsIsMod(char *s);
bool gsIsPlus(char *s);
bool gsIsMinus(char *s);
bool gsIsLT(char *s);
bool gsIsGT(char *s);
bool gsIsLTE(char *s);
bool gsIsGTE(char *s);
bool gsIsIn(char *s);
bool gsIsCons(char *s);
bool gsIsSnoc(char *s);
bool gsIsConcat(char *s);
bool gsIsEq(char *s);
bool gsIsNeq(char *s);
bool gsIsAnd(char *s);
bool gsIsOr(char *s);
bool gsIsImp(char *s);
bool gsIsActProcRef(char *s);
bool gsIsDelta(char *s);
bool gsIsTau(char *s);
bool gsIsSum(char *s);
bool gsIsRestrict(char *s);
bool gsIsAllow(char *s);
bool gsIsHide(char *s);
bool gsIsRename(char *s);
bool gsIsComm(char *s);
bool gsIsSync(char *s);
bool gsIsAtTime(char *s);
bool gsIsSeq(char *s);
bool gsIsCond(char *s);
bool gsIsBInit(char *s);
bool gsIsMerge(char *s);
bool gsIsLMerge(char *s);
bool gsIsChoice(char *s);
bool gsIsMultActName(char *s);
bool gsIsCommExpr(char *s);
bool gsIsRenExpr(char *s);

//ATerm library work arounds
//--------------------------
//
//To eliminate ridiculous type casts in the rest of the code, we introducde
//wrappers around functions ATelementAt and ATgetArgument.
//This is caused by a bad interface design of the ATerm library

ATermAppl ATAelementAt(ATermList list, int index);
ATermList ATLelementAt(ATermList list, int index);
ATermInt  ATIelementAt(ATermList list, int index);
ATermAppl ATAgetArgument(ATermAppl appl, int nr);
ATermList ATLgetArgument(ATermAppl appl, int nr);
ATermInt  ATIgetArgument(ATermAppl appl, int nr);

#ifdef __cplusplus
}
#endif
