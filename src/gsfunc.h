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

ATermAppl gsMakeId(char *s);
ATermAppl gsMakeNumber(char *s);
ATermAppl gsMakeSpec(ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl EqnSpec, ATermAppl ActSpec, ATermAppl ProcSpec, ATermAppl Init);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeSortDeclStandard(ATermList Ids);
ATermAppl gsMakeSortDeclRef(ATermAppl Id, ATermAppl SortExpr);
ATermAppl gsMakeSortDeclStruct(ATermAppl Id, ATermList ConstrDecls);
ATermAppl gsMakeStructDeclCons(ATermAppl ConsId, ATermList ProjDecls,
  ATermAppl NilOrId);
ATermAppl gsMakeNil();
ATermAppl gsMakeStructDeclProj(ATermAppl NilOrId, ATermList SortExprs);
ATermAppl gsMakeIds(ATermList Ids);
ATermAppl gsMakeConsSpec(ATermList OpDecls);
ATermAppl gsMakeMapSpec(ATermList OpDecls);
ATermAppl gsMakeIdsDecl(ATermList Ids, ATermAppl SortExpr);
ATermAppl gsMakeIdDecl(ATermAppl Id, ATermAppl SortExpr);
ATermAppl gsMakeEqnSpec(ATermList EqnSects);
ATermAppl gsMakeEqnSect(ATermList VarDecls, ATermList EqnDecls);
ATermAppl gsMakeEqnDecl(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeActSpec(ATermList ActDecls);
ATermAppl gsMakeActDecl(ATermList Ids, ATermList SortExprs);
ATermAppl gsMakeProcSpec(ATermList ProcDecls);
ATermAppl gsMakeProcDecl(ATermAppl Id, ATermList ProcVars, ATermAppl ProcExpr);
ATermAppl gsMakeInit(ATermAppl ProcExpr);
ATermAppl gsMakeSortBool();
ATermAppl gsMakeSortPos();
ATermAppl gsMakeSortNat();
ATermAppl gsMakeSortInt();
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortArrow(ATermList Domain, ATermAppl SortExpr);
ATermAppl gsMakeTrue();
ATermAppl gsMakeFalse();
ATermAppl gsMakeEmptyList();
ATermAppl gsMakeEmptySetBag();
ATermAppl gsMakeListEnum(ATermList DataExprs);
ATermAppl gsMakeSetEnum(ATermList DataExprs);
ATermAppl gsMakeBagEnum(ATermList BagEnumElts);
ATermAppl gsMakeSetBagComp(ATermAppl IdDecl, ATermAppl DataExpr);
ATermAppl gsMakeFuncApp(ATermAppl DataExpr, ATermList FuncArgs);
ATermAppl gsMakeNotOrCompl(ATermAppl DataExpr);
ATermAppl gsMakeNeg(ATermAppl DataExpr);
ATermAppl gsMakeSize(ATermAppl DataExpr);
ATermAppl gsMakeForall(ATermAppl IdDecl, ATermAppl DataExpr);
ATermAppl gsMakeExists(ATermAppl IdDecl, ATermAppl DataExpr);
ATermAppl gsMakeListAt(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMultOrIntersect(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeDiv(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMod(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeAddOrUnion(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeSubtOrDiff(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeLTOrPropSubset(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeGT(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeLTEOrSubset(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeGTE(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeIn(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeCons(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeSnoc(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeConcat(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeEq(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeNeq(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeAnd(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeOr(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeImp(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeLambda(ATermAppl IdDecl, ATermAppl DataExpr);
ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls);
ATermAppl gsMakeWhrDecl(ATermAppl Id, ATermAppl DataExpr);
ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity);
ATermAppl gsMakeActProcRef(ATermAppl Id, ATermList ActProcArgs);
ATermAppl gsMakeDelta();
ATermAppl gsMakeTau();
ATermAppl gsMakeSum(ATermAppl IdDecl, ATermAppl ProcExpr);
ATermAppl gsMakeRestrict(ATermList MAIdSet, ATermAppl ProcExpr);
ATermAppl gsMakeAllow(ATermList MAIdSet, ATermAppl ProcExpr);
ATermAppl gsMakeHide(ATermList MAIdSet, ATermAppl ProcExpr);
ATermAppl gsMakeRename(ATermList RenExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeComm(ATermList CommExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeSync(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl DataExpr);
ATermAppl gsMakeSeq(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeCond(ATermAppl DataExpr, ATermAppl ProcExpr);
ATermAppl gsMakeBInit(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMerge(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeLMerge(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeChoice(ATermAppl OpLeft, ATermAppl OpRight);
ATermAppl gsMakeMAId(ATermList Ids);
ATermAppl gsMakeMAIdSet(ATermList MAIds);
ATermAppl gsMakeCommExpr(ATermAppl MAId, ATermAppl TauOrId);
ATermAppl gsMakeCommExprSet(ATermList CommExprs);
ATermAppl gsMakeRenExpr(ATermAppl FromId, ATermAppl ToId);
ATermAppl gsMakeRenExprSet(ATermList RenExprs);

//strcmp extensions
//------------------
//
//Implements the comparison of the function names of all elements of the 
//structure of ATerms to improve readability

bool gsIsId(char *s);
bool gsIsNumber(char *s);
bool gsIsSpec(char *s);
bool gsIsSortSpec(char *s);
bool gsIsSortDeclStandard(char *s);
bool gsIsSortDeclRef(char *s);
bool gsIsSortDeclStruct(char *s);
bool gsIsStructDeclCons(char *s);
bool gsIsNil(char *s);
bool gsIsStructDeclProj(char *s);
bool gsIsIds(char *s);
bool gsIsConsSpec(char *s);
bool gsIsMapSpec(char *s);
bool gsIsIdsDecl(char *s);
bool gsIsIdDecl(char *s);
bool gsIsEqnSpec(char *s);
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
bool gsIsTrue(char *s);
bool gsIsFalse(char *s);
bool gsIsEmptyList(char *s);
bool gsIsEmptySetBag(char *s);
bool gsIsListEnum(char *s);
bool gsIsSetEnum(char *s);
bool gsIsBagEnum(char *s);
bool gsIsSetBagComp(char *s);
bool gsIsFuncApp(char *s);
bool gsIsNotOrCompl(char *s);
bool gsIsNeg(char *s);
bool gsIsSize(char *s);
bool gsIsForall(char *s);
bool gsIsExists(char *s);
bool gsIsListAt(char *s);
bool gsIsMultOrIntersect(char *s);
bool gsIsDiv(char *s);
bool gsIsMod(char *s);
bool gsIsAddOrUnion(char *s);
bool gsIsSubtOrDiff(char *s);
bool gsIsLTOrPropSubset(char *s);
bool gsIsGT(char *s);
bool gsIsLTEOrSubset(char *s);
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
bool gsIsLambda(char *s);
bool gsIsWhr(char *s);
bool gsIsWhrDecl(char *s);
bool gsIsBagEnumElt(char *s);
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
bool gsIsMAId(char *s);
bool gsIsMAIdSet(char *s);
bool gsIsCommExpr(char *s);
bool gsIsCommExprSet(char *s);
bool gsIsRenExpr(char *s);
bool gsIsRenExprSet(char *s);

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
