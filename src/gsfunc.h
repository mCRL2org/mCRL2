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

//ATerm library work arounds
//--------------------------
//
//To eliminate ridiculous type casts in the rest of the code, we introduce
//wrappers around functions ATelementAt and ATgetArgument.
//This is caused by a bad interface design of the ATerm library

ATermAppl ATAelementAt(ATermList list, int index);
ATermList ATLelementAt(ATermList list, int index);
ATermInt  ATIelementAt(ATermList list, int index);
ATermAppl ATAgetArgument(ATermAppl appl, int nr);
ATermList ATLgetArgument(ATermAppl appl, int nr);
ATermInt  ATIgetArgument(ATermAppl appl, int nr);
char *ATSgetArgument(ATermAppl appl, int nr);
ATermAppl ATAgetFirst(ATermList list);
ATermList ATLgetNext(ATermList list);
ATermList ATinsertA(ATermList list, ATermAppl appl);

//Initialisation of constructor functions
void gsEnableConstructorFunctions(void);
//Pre : the ATerm has been initialised
//Post: the constructor creator and recogniser functions are enabled

//Constructor creators
//--------------------
//
//The creation of all constructor elements of the internal ATerm structure to
//improve readability and to minimise type casts in the rest of the code

ATermAppl gsMakeSpecV1(
  ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl DataEqnSpec, ATermAppl ActSpec, ATermAppl ProcEqnSpec,
  ATermAppl Init);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeConsSpec(ATermList OpIds);
ATermAppl gsMakeMapSpec(ATermList OpIds);
ATermAppl gsMakeDataEqnSpec(ATermList DataEqns);
ATermAppl gsMakeActSpec(ATermList ActIds);
ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns);
ATermAppl gsMakeInit(ATermAppl ProcExpr);
ATermAppl gsMakeSortId(ATermAppl Name);
ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl DataCond,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeNil();
ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs);
ATermAppl gsMakeProcEqn(ATermAppl ProcVarId, ATermList DataVarIds,
  ATermAppl ProcExp);
ATermAppl gsMakeProcVarId(ATermAppl Name,ATermList SortExprs);
ATermAppl gsMakeSortBool();
ATermAppl gsMakeSortPos();
ATermAppl gsMakeSortNat();
ATermAppl gsMakeSortInt();
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult);
ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult);
ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecName);
//RecName may be NULL
ATermAppl gsMakeStructProj(ATermAppl ProjName, ATermList SortExprs);
//ProjName may be NULL
ATermAppl gsMakeDataVarIdOpId(ATermAppl Name);
ATermAppl gsMakeDataApplProd(ATermAppl DataExpr, ATermList DataArgs);
ATermAppl gsMakeDataAppl(ATermAppl DataExpr, ATermAppl DataArg);
ATermAppl gsMakeNumber(ATermAppl Number, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeListEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeSetEnum(ATermList DataExprs, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeBagEnum(ATermList BagEnumElts, ATermAppl SortExprOrUnknown);
ATermAppl gsMakeSetBagComp(ATermAppl DataVarId, ATermAppl DataExpr);
ATermAppl gsMakeForall(ATermList DataVarIds, ATermAppl DataExpr);
ATermAppl gsMakeExists(ATermList DataVarIds, ATermAppl DataExpr);
ATermAppl gsMakeLambda(ATermList DataVarIds, ATermAppl DataExpr);
ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList WhrDecls);
ATermAppl gsMakeUnknown();
ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity);
ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr);
ATermAppl gsMakeActionProcess(ATermAppl Name, ATermList DataExprs);
ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs);
ATermAppl gsMakeProcess(ATermAppl ProcVarId, ATermList DataExprs);
ATermAppl gsMakeDelta();
ATermAppl gsMakeTau();
ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr);
ATermAppl gsMakeRestrict(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeHide(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeRename(ATermList RenameExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeComm(ATermList CommExprSet, ATermAppl ProcExpr);
ATermAppl gsMakeSync(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl DataExpr);
ATermAppl gsMakeSeq(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeCond(ATermAppl DataExpr, ATermAppl ProcExprThen,
  ATermAppl ProcExprElse);
ATermAppl gsMakeBInit(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeLMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeChoice(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeMultActName(ATermList ActNames);
ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName);
ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActName);
//ActName may be NULL

//Constructor recognisers
//-----------------------
//
//Implements the comparison of the function names of all constructor elements
//of the internal ATerm structure to improve readability

bool gsIsSpecV1(ATermAppl term);
bool gsIsSortSpec(ATermAppl term);
bool gsIsConsSpec(ATermAppl term);
bool gsIsMapSpec(ATermAppl term);
bool gsIsDataEqnSpec(ATermAppl term);
bool gsIsActSpec(ATermAppl term);
bool gsIsProcEqnSpec(ATermAppl term);
bool gsIsInit(ATermAppl term);
bool gsIsSortId(ATermAppl term);
bool gsIsSortRef(ATermAppl term);
bool gsIsOpId(ATermAppl term);
bool gsIsDataEqn(ATermAppl term);
bool gsIsDataVarId(ATermAppl term);
bool gsIsNil(ATermAppl term);
bool gsIsActId(ATermAppl term);
bool gsIsProcEqn(ATermAppl term);
bool gsIsProcVarId(ATermAppl term);
bool gsIsSortBool(ATermAppl term);
bool gsIsSortPos(ATermAppl term);
bool gsIsSortNat(ATermAppl term);
bool gsIsSortInt(ATermAppl term);
bool gsIsSortList(ATermAppl term);
bool gsIsSortSet(ATermAppl term);
bool gsIsSortBag(ATermAppl term);
bool gsIsSortStruct(ATermAppl term);
bool gsIsSortArrowProd(ATermAppl term);
bool gsIsSortArrow(ATermAppl term);
bool gsIsStructCons(ATermAppl term);
bool gsIsStructProj(ATermAppl term);
bool gsIsDataVarIdOpId(ATermAppl term);
bool gsIsDataApplProd(ATermAppl term);
bool gsIsDataAppl(ATermAppl term);
bool gsIsNumber(ATermAppl term);
bool gsIsListEnum(ATermAppl term);
bool gsIsSetEnum(ATermAppl term);
bool gsIsBagEnum(ATermAppl term);
bool gsIsSetBagComp(ATermAppl term);
bool gsIsForall(ATermAppl term);
bool gsIsExists(ATermAppl term);
bool gsIsLambda(ATermAppl term);
bool gsIsWhr(ATermAppl term);
bool gsIsUnknown(ATermAppl term);
bool gsIsBagEnumElt(ATermAppl term);
bool gsIsWhrDecl(ATermAppl term);
bool gsIsActionProcess(ATermAppl term);
bool gsIsAction(ATermAppl term);
bool gsIsProcess(ATermAppl term);
bool gsIsDelta(ATermAppl term);
bool gsIsTau(ATermAppl term);
bool gsIsSum(ATermAppl term);
bool gsIsRestrict(ATermAppl term);
bool gsIsAllow(ATermAppl term);
bool gsIsHide(ATermAppl term);
bool gsIsRename(ATermAppl term);
bool gsIsComm(ATermAppl term);
bool gsIsSync(ATermAppl term);
bool gsIsAtTime(ATermAppl term);
bool gsIsSeq(ATermAppl term);
bool gsIsCond(ATermAppl term);
bool gsIsBInit(ATermAppl term);
bool gsIsMerge(ATermAppl term);
bool gsIsLMerge(ATermAppl term);
bool gsIsChoice(ATermAppl term);
bool gsIsMultActName(ATermAppl term);
bool gsIsRenameExpr(ATermAppl term);
bool gsIsCommExpr(ATermAppl term);

ATermAppl gsString2ATermAppl(char *s);
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL

//Creation of data expressions for system defined operations. If possible,
//types are checked.
ATermAppl gsMakeDataExprTrue();
ATermAppl gsMakeDataExprFalse();
ATermAppl gsMakeDataExprNot(ATermAppl DataExpr);
ATermAppl gsMakeDataExprAnd(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataExprOr(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataExprImp(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataExprEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataExprNeq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataExprIf(ATermAppl DataExprCond, ATermAppl DataExprThen,
  ATermAppl DataExprIf);

ATermAppl gsMakeDataExprPos(int p);
//Pre: p > 0
//Ret: data expression of sort Pos that is a representation of p

ATermAppl gsMakeDataExprNat(int n);
//Pre: n >= 0
//Ret: data expression of sort Nat that is a representation of n

ATermAppl gsMakeDataExprInt(int z);
//Pre: true
//Ret: data expression of sort Int that is a representation of z

#ifdef __cplusplus
}
#endif
