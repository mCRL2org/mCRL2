#include <stdbool.h>
#include <stdarg.h>
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

#define throw             goto finally
//model C++ throw by a 'goto finally' statement

#define ThrowV(x)         Result = x; throw
//store x in result and throw an exception

#define ThrowM(...)       gsErrorMsg(__VA_ARGS__); throw
//print error message supplied by the first parameter with the remaining
//parameters as arguments

#define ThrowVM(x, ...)   gsErrorMsg(__VA_ARGS__); ThrowV(x)
//print error message supplied by the first parameter with the remaining
//parameters as arguments, and throw an exception with value x

//Message printing
//----------------

void gsSetQuietMsg(void);
//Post: Printing of warnings, verbose information and extended debugging
//      information during program execution is disabled.

void gsSetNormalMsg(void);
//Post: Printing of warnings during program execution is enabled. Printing of
//      verbose information and extended debugging information is disabled.

void gsSetVerboseMsg(void);
//Post: Printing of warnings and verbose information during program execution
//      is enabled. Printing of extended debugging information is disabled.

void gsSetDebugMsg(void);
//Post: Printing of warnings, verbose information and extended debugging
//      information during program executation is enabled.

inline void gsErrorMsg(char *Format, ...);
//Post: "error: " is printed to stderr followed by Format, where the remaining
//      parameters are used as ATprintf arguments to Format.

inline void gsWarningMsg(char *Format, ...);
//Post: If the printing of warning messages is enabled, "warning: " is printed
//      to stderr followed by Format, where the remaining parameters are used
//      as ATprintf arguments to Format.

inline void gsVerboseMsg(char *Format, ...);
//Post: If the printing of verbose information is enabled, Format is printed to
//      stderr, where the remaining parameters are used as ATprintf arguments
//      to Format.

#define gsDebugMsg(...)   gsDebugMsgFunc(__func__, __VA_ARGS__)
//Post: If the printing of debug messages is enabled, the name of the current
//      function is printed to stderr, followed by the first parameter with the
//      remaining parameters as ATprintf arguments.

inline void gsDebugMsgFunc(const char *FuncName, char *Format, ...);
//Post: If the printing of debug messages is enabled, the name of FuncName is
//      printed to stderr, followed by Format where  the remaining parameters
//      are used as ATprintf arguments to Format.

//ATerm library work arounds
//--------------------------
//
//To eliminate downcasts in the rest of the code, we introduce wrappers around
//ATerm library functions.
//This is caused by a bad interface design of the ATerm library.

ATermAppl ATAelementAt(ATermList List, int Index);
ATermList ATLelementAt(ATermList List, int Index);
ATermAppl ATAgetArgument(ATermAppl Appl, int Nr);
ATermList ATLgetArgument(ATermAppl Appl, int Nr);
ATermAppl ATAgetFirst(ATermList List);
ATermList ATLgetFirst(ATermList List);
ATermAppl ATAtableGet(ATermTable Table, ATerm Key);
ATermList ATLtableGet(ATermTable Table, ATerm Key);
void      ATprotectAppl(ATermAppl *PAppl);
void      ATprotectList(ATermAppl *PList);
void      ATunprotectAppl(ATermAppl *PAppl);
void      ATunprotectList(ATermAppl *PList);

//Functions for the internal ATerm structure
//------------------------------------------

ATermAppl gsString2ATermAppl(char *s);
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL

char *gsATermAppl2String(ATermAppl term);
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise

void gsEnableConstructorFunctions(void);
//Pre : the ATerm has been initialised
//Post: the constructor creator and recogniser functions are enabled

//Creation of all constructor elements of the internal ATerm structure.
ATermAppl gsMakeSpecV1(
  ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl DataEqnSpec, ATermAppl ActSpec, ATermAppl ProcEqnSpec,
  ATermAppl Init);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeConsSpec(ATermList OpIds);
ATermAppl gsMakeMapSpec(ATermList OpIds);
ATermAppl gsMakeDataEqnSpec(ATermList DataEqns);
ATermAppl gsMakeActSpec(ATermList ActIds);
ATermAppl gsMakeSortId(ATermAppl Name);
ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl BoolExprOrNil,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeNil();
ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs);
ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns);
ATermAppl gsMakeLPE(ATermList DataVarIds, ATermList LPESummands);
ATermAppl gsMakeProcEqn(ATermAppl ProcVarId, ATermList DataVarIds,
  ATermAppl ProcExp);
ATermAppl gsMakeProcVarId(ATermAppl Name,ATermList SortExprs);
ATermAppl gsMakeLPESummand(ATermList DataVarIds, ATermAppl BoolExpr,
  ATermAppl MultActOrDelta, ATermAppl TimeExprOrNil, ATermList Assignments);
ATermAppl gsMakeMultAct(ATermList Actions);
ATermAppl gsMakeDelta();
ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs);
ATermAppl gsMakeAssignment(ATermAppl DataVarId, ATermAppl DataExprOrNil);
ATermAppl gsMakeInit(ATermAppl ProcExpr);
ATermAppl gsMakeLPEInit(ATermList DataExprs);
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult);
ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult);
ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecNameOrNil); 
ATermAppl gsMakeStructProj(ATermAppl ProjNameOrNil, ATermList SortExprs);
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
ATermAppl gsMakeProcess(ATermAppl ProcVarId, ATermList DataExprs);
ATermAppl gsMakeTau();
ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr);
ATermAppl gsMakeRestrict(ATermList ActNames, ATermAppl ProcExpr);
ATermAppl gsMakeHide(ATermList ActNames, ATermAppl ProcExpr);
ATermAppl gsMakeRename(ATermList RenameExprs, ATermAppl ProcExpr);
ATermAppl gsMakeComm(ATermList CommExprs, ATermAppl ProcExpr);
ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr);
ATermAppl gsMakeSync(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl TimeExpr);
ATermAppl gsMakeSeq(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeCond(ATermAppl BoolExpr, ATermAppl ProcExprThen,
  ATermAppl ProcExprElse);
ATermAppl gsMakeBInit(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeLMerge(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeChoice(ATermAppl ProcExprLHS, ATermAppl ProcExprRHS);
ATermAppl gsMakeMultActName(ATermList ActNames);
ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName);
ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActNameOrNil);

//Recognisers of all constructor elements of the internal ATerm structure.
bool gsIsSpecV1(ATermAppl Term);
bool gsIsSortSpec(ATermAppl Term);
bool gsIsConsSpec(ATermAppl Term);
bool gsIsMapSpec(ATermAppl Term);
bool gsIsDataEqnSpec(ATermAppl Term);
bool gsIsActSpec(ATermAppl Term);
bool gsIsSortId(ATermAppl Term);
bool gsIsSortRef(ATermAppl Term);
bool gsIsOpId(ATermAppl Term);
bool gsIsDataEqn(ATermAppl Term);
bool gsIsDataVarId(ATermAppl Term);
bool gsIsNil(ATermAppl Term);
bool gsIsActId(ATermAppl Term);
bool gsIsProcEqnSpec(ATermAppl Term);
bool gsIsLPE(ATermAppl Term);
bool gsIsProcEqn(ATermAppl Term);
bool gsIsProcVarId(ATermAppl Term);
bool gsIsLPESummand(ATermAppl Term);
bool gsIsMultAct(ATermAppl Term);
bool gsIsDelta(ATermAppl Term);
bool gsIsAction(ATermAppl Term);
bool gsIsAssignment(ATermAppl Term);
bool gsIsInit(ATermAppl Term);
bool gsIsLPEInit(ATermAppl Term);
bool gsIsSortList(ATermAppl Term);
bool gsIsSortSet(ATermAppl Term);
bool gsIsSortBag(ATermAppl Term);
bool gsIsSortStruct(ATermAppl Term);
bool gsIsSortArrowProd(ATermAppl Term);
bool gsIsSortArrow(ATermAppl Term);
bool gsIsStructCons(ATermAppl Term);
bool gsIsStructProj(ATermAppl Term);
bool gsIsDataVarIdOpId(ATermAppl Term);
bool gsIsDataApplProd(ATermAppl Term);
bool gsIsDataAppl(ATermAppl Term);
bool gsIsNumber(ATermAppl Term);
bool gsIsListEnum(ATermAppl Term);
bool gsIsSetEnum(ATermAppl Term);
bool gsIsBagEnum(ATermAppl Term);
bool gsIsSetBagComp(ATermAppl Term);
bool gsIsForall(ATermAppl Term);
bool gsIsExists(ATermAppl Term);
bool gsIsLambda(ATermAppl Term);
bool gsIsWhr(ATermAppl Term);
bool gsIsUnknown(ATermAppl Term);
bool gsIsBagEnumElt(ATermAppl Term);
bool gsIsWhrDecl(ATermAppl Term);
bool gsIsActionProcess(ATermAppl Term);
bool gsIsProcess(ATermAppl Term);
bool gsIsTau(ATermAppl Term);
bool gsIsSum(ATermAppl Term);
bool gsIsRestrict(ATermAppl Term);
bool gsIsHide(ATermAppl Term);
bool gsIsRename(ATermAppl Term);
bool gsIsComm(ATermAppl Term);
bool gsIsAllow(ATermAppl Term);
bool gsIsSync(ATermAppl Term);
bool gsIsAtTime(ATermAppl Term);
bool gsIsSeq(ATermAppl Term);
bool gsIsCond(ATermAppl Term);
bool gsIsBInit(ATermAppl Term);
bool gsIsMerge(ATermAppl Term);
bool gsIsLMerge(ATermAppl Term);
bool gsIsChoice(ATermAppl Term);
bool gsIsMultActName(ATermAppl Term);
bool gsIsRenameExpr(ATermAppl Term);
bool gsIsCommExpr(ATermAppl Term);


//Creation of sort identifiers for system defined sorts.
ATermAppl gsMakeSortIdBool(void);
//Ret: Sort identifier for `Bool'

ATermAppl gsMakeSortIdPos(void);
//Ret: Sort identifier for `Pos'

ATermAppl gsMakeSortIdNat(void);
//Ret: Sort identifier for `Nat'

ATermAppl gsMakeSortIdInt(void);
//Ret: Sort identifier for `Int'


//Creation of sort expressions for system defined sorts.
ATermAppl gsMakeSortExprBool(void);
//Ret: Sort expression for `Bool'

ATermAppl gsMakeSortExprPos(void);
//Ret: Sort expression for `Pos'

ATermAppl gsMakeSortExprNat(void);
//Ret: Sort expression for `Nat'

ATermAppl gsMakeSortExprInt(void);
//Ret: Sort expression for `Int'


//Auxiliary functions concerning sort expressions
ATermAppl gsMakeSortArrow2(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprResult);
//Pre: SortExprDom1, SortExprDom2 and SortExprResult are sort expressions
//Ret: Internal representation of the sort expression
//     SortExprDom1 -> SortExprDom2 -> SortExprResult, where -> is right
//     associative.

ATermAppl gsMakeSortArrow3(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprResult);
//Pre: SortExprDom1, SortExprDom2, SortExprDom3 and SortExprResult are sort
//     expressions
//Ret: Internal representation of the sort expression
//     SortExprDom1 -> SortExprDom2 -> SortExprDom3 -> SortExprResult,
//     where -> is right associative.

ATermAppl gsMakeSortArrow4(ATermAppl SortExprDom1, ATermAppl SortExprDom2,
  ATermAppl SortExprDom3, ATermAppl SortExprDom4, ATermAppl SortExprResult);
//Pre: SortExprDom1, SortExprDom2, SortExprDom3, SortExprDom4 and
//     SortExprResult are sort expressions
//Ret: Internal representation of the sort expression
//     SortExprDom1 -> SortExprDom2 -> SortExprDom3 -> SortExprDom4 ->
//       SortExprResult, where -> is right associative.

ATermAppl gsMakeSortArrowList(ATermList SortExprDomain,
  ATermAppl SortExprResult);
//Pre: SortExprDomain is of the form [e_0, ..., e_n], where n is a natural
//     number and each e_i, 0 <= i <= n, is a sort expression.
//     SortExprResult is a sort expression, which we denote by e.
//Ret: Internal representation of the sort expression e_0 -> ... -> e_n -> e,
//     where -> is right associative.

ATermAppl gsGetSort(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: if DataExpr is a DataVarId, OpId, or DataApp, return its sort
//     return Unknown, otherwise

int gsMaxDomainLength(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: the maximum number of sort expressions that can serve as a domain

//Creation of operation identifiers for system defined operations.
ATermAppl gsMakeOpIdTrue(void);
//Ret: Operation identifier for `true'

ATermAppl gsMakeOpIdFalse(void);
//Ret: Operation identifier for `false'

ATermAppl gsMakeOpIdNot(void);
//Ret: Operation identifier for logical negation

ATermAppl gsMakeOpIdAnd(void);
//Ret: Operation identifier for conjunction

ATermAppl gsMakeOpIdOr(void);
//Ret: Operation identifier for disjunction

ATermAppl gsMakeOpIdImp(void);
//Ret: Operation identifier for implication

ATermAppl gsMakeOpIdEq(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the equality of terms of sort SortExpr

ATermAppl gsMakeOpIdNeq(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the inequality of terms of sort SortExpr

ATermAppl gsMakeOpIdIf(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the conditional of terms of sort SortExpr

ATermAppl gsMakeOpIdForall(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the universal quantification over sort SortExpr

ATermAppl gsMakeOpIdExists(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the existential quantification over sort
//     SortExpr

ATermAppl gsMakeOpId1(void);
//Ret: Operation identifier for the Pos constructor `1'

ATermAppl gsMakeOpIdCDub(void);
//Ret: Operation identifier for the Pos constructor `double and add a bit'

ATermAppl gsMakeOpId0(void);
//Ret: Operation identifier for the Nat constructor `0'

ATermAppl gsMakeOpIdCNat(void);
//Ret: Operation identifier for the creation of a natural number from a
//     positive number

ATermAppl gsMakeOpIdCNeg(void);
//Ret: Operation identifier for the negation of a positive number

ATermAppl gsMakeOpIdCInt(void);
//Ret: Operation identifier for the creation of an integer from a natural
//     number

ATermAppl gsMakeOpIdPos2Nat(void);
//Ret: Operation identifier for the conversion of Pos to Nat

ATermAppl gsMakeOpIdPos2Int(void);
//Ret: Operation identifier for the conversion of Pos to Int

ATermAppl gsMakeOpIdNat2Pos(void);
//Ret: Operation identifier for the conversion of Nat to Pos

ATermAppl gsMakeOpIdNat2Int(void);
//Ret: Operation identifier for the conversion of Nat to Int

ATermAppl gsMakeOpIdInt2Pos(void);
//Ret: Operation identifier for the conversion of Int to Pos

ATermAppl gsMakeOpIdInt2Nat(void);
//Ret: Operation identifier for the conversion of Int to Nat

ATermAppl gsMakeOpIdLTE(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `less than or equal' on SortExpr

ATermAppl gsMakeOpIdLT(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `less than' on SortExpr

ATermAppl gsMakeOpIdGTE(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `greater than or equal' on SortExpr

ATermAppl gsMakeOpIdGT(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `greater than' on SortExpr

ATermAppl gsMakeOpIdMax(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `maximum' on SortExpr

ATermAppl gsMakeOpIdMin(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `minimum' on SortExpr

ATermAppl gsMakeOpIdAbs(void);
//Ret: Operation identifier for `absolute value' of sort Int -> Nat

ATermAppl gsMakeOpIdNeg(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for negation

ATermAppl gsMakeOpIdSucc(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for the successor with argument sort SortExpr
//     The corresponding result sort is:
//     - Pos, if SortExpr is Pos
//     - Pos, if SortExpr is Nat
//     - Int, if SortExpr is Int

ATermAppl gsMakeOpIdPred(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for the predecessor with argument sort SortExpr
//     The corresponding result sort is:
//     - Nat, if SortExpr is Pos
//     - Int, if SortExpr is Nat
//     - Int, if SortExpr is Int

ATermAppl gsMakeOpIdDub(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for '2*n + |b|', where n is of sort SortExpr and
//     b is of sort Bool

ATermAppl gsMakeOpIdAdd(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for addition on SortExpr

ATermAppl gsMakeOpIdAddC();
//Ret: Operation identifier for the addition of two positive numbers and a
//     (carry) bit, which is of sort Bool -> Pos -> Pos -> Pos

ATermAppl gsMakeOpIdSubt(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for subtraction on SortExpr

ATermAppl gsMakeOpIdSubtB();
//Ret: Operation identifier for the subtraction of a positive number and a
//     (borrow) bit from a positive number, which is of sort
//     Bool -> Pos -> Pos -> Pos

ATermAppl gsMakeOpIdDouble(void);
//Ret: Operation identifier for `double', which has sort Int -> Int

ATermAppl gsMakeOpIdMult(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for multiplication on SortExpr

ATermAppl gsMakeOpIdMultIR();
//Ret: Operation identifier for multiplication of two positive numbers, plus an
//     optional intermediate positive result, which is of sort
//     Bool -> Pos -> Pos -> Pos -> Pos

ATermAppl gsMakeOpIdDiv(ATermAppl SortExpr);
//Pre: SortExpr is Nat or Int
//Ret: Operation identifier for `quotient after division', which has sort
//     e -> Pos -> e, where e stands for SortExpr

ATermAppl gsMakeOpIdMod(ATermAppl SortExpr);
//Pre: SortExpr is Nat or Int
//Ret: Operation identifier for `remainder after division', which has sort
//     e -> Pos -> Nat, where e stands for SortExpr

ATermAppl gsMakeOpIdExp(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for exponentiation, which has sort s -> Nat -> s,
//     where s stands for SortExpr

ATermAppl gsMakeOpIdEven();
//Ret: Operation identifier for 'even' of sort Nat -> Bool


//Creation of data expressions for system defined operations.
ATermAppl gsMakeDataExprTrue(void);
//Ret: Data expression for `true'

ATermAppl gsMakeDataExprFalse(void);
//Ret: Data expression for `false'

ATermAppl gsMakeDataExprNot(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for the negation of DataExpr

ATermAppl gsMakeDataExprAnd(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for the conjunction of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprOr(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for the disjunction of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprImp(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for `DataExprLHS implies DataExprRHS'

ATermAppl gsMakeDataExprEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort, and
//     must be different from Unknown
//Ret: Data expression for the equality of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprNeq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort, and
//     must be different from Unknown
//Ret: Data expression for the inequality of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprIf(ATermAppl DataExprCond, ATermAppl DataExprThen,
  ATermAppl DataExprIf);
//Pre: DataExprCond is a data expression of sort Bool
//     DataExprThen and DataExprIf are data expressions of the same sort, and
//     must be different from Unknown
//Ret: Data expression for `if(DataExprCond, DataExprThen, DataExprIf)'

ATermAppl gsMakeDataExpr1(void);
//Ret: Data expression for `1' of sort Pos

ATermAppl gsMakeDataExprCDub(ATermAppl DataExprBit, ATermAppl DataExprPos);
//Pre: DataExprBit and DataExprPos are data expressions of sort Bool and Pos,
//     respectively, which we denote by b and p
//Ret: Data expression for `2*p + |b|', where '|b|' is defined as:
//       |true|  = 1
//       |false| = 0

ATermAppl gsMakeDataExpr0(void);
//Ret: Data expression for `0' of sort Nat

ATermAppl gsMakeDataExprCNat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: DataExpr as a data expression of sort Nat

ATermAppl gsMakeDataExprCNeg(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the negation of DataExpr

ATermAppl gsMakeDataExprCInt(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: DataExpr as a data expression of sort Int

ATermAppl gsMakeDataExprPos2Nat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the conversion of DataExpr to Nat

ATermAppl gsMakeDataExprPos2Int(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the conversion of DataExpr to Int

ATermAppl gsMakeDataExprNat2Pos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: Data expression for the conversion of DataExpr to Pos

ATermAppl gsMakeDataExprNat2Int(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: Data expression for the conversion of DataExpr to Int

ATermAppl gsMakeDataExprInt2Pos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the conversion of DataExpr to Pos

ATermAppl gsMakeDataExprInt2Nat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the conversion of DataExpr to Nat

ATermAppl gsMakeDataExprLTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `less than or equal' of DataExprLHS and
//     DataExprRHS

ATermAppl gsMakeDataExprLT(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `less than' of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprGTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `greater than or equal' of DataExprLHS and
//     DataExprRHS

ATermAppl gsMakeDataExprGT(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `greater than' of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprMax(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the maximum of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprMin(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the minimum of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprAbs(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the absolute value of DataExpr, which has sort Nat

ATermAppl gsMakeDataExprNeg(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos, Nat or Int 
//Ret: Data expression for the negation of DataExpr, which has sort Int

ATermAppl gsMakeDataExprSucc(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos, Nat or Int
//Ret: Data expression for the successor of DataExpr
//     The result has sort:
//     - Pos, if DataExpr has sort Pos
//     - Pos, if DataExpr has sort Nat
//     - Int, if DataExpr has sort Int

ATermAppl gsMakeDataExprPred(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos, Nat or Int
//Ret: Data expression for the predecessor of DataExpr
//     The result has sort:
//     - Nat, if DataExpr has sort Pos
//     - Int, if DataExpr has sort Nat
//     - Int, if DataExpr has sort Int

ATermAppl gsMakeDataExprDub(ATermAppl DataExprBit, ATermAppl DataExprNum);
//Pre: DataExprbit and DataExprNum are data expressions of sort Pos, Nat or
//     Int, and Bool, respectively, which we denote by n and b
//Ret: Data expression for '2*n + |b|'. The result has sort:
//     - Pos, if DataExpr has sort Pos
//     - Pos, if DataExpr has sort Nat
//     - Int, if DataExpr has sort Int

ATermAppl gsMakeDataExprAdd(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the addition of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprAddC(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS);
//Pre: DataExprBit, DataExprLHS and DataExprRHS are data expressions of sort
//     Bool, Pos and Pos, respectively, which we denote by b, p and q.
//Ret: Data expression for 'p + q + |b|', i.e. addition with carry.

ATermAppl gsMakeDataExprSubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the subtraction of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprSubtB(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS);
//Pre: DataExprBit, DataExprLHS and DataExprRHS are data expressions of sort
//     Bool, Pos and Pos, respectively, which we denote by b, p and q.
//Ret: Data expression for 'p - (q + |b|)', i.e. subtraction with borrow.

ATermAppl gsMakeDataExprMult(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the multiplication of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprMultIR(ATermAppl DataExprBit, ATermAppl DataExprIR,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprBit, DataExprIR, DataExprLHS and DataExprRHS are data
//     expressions of sort Bool, Pos, Pos, and Pos, respectively, which we
//     denote by b, p, q and r.
//Ret: Data expression for '|b|*p + q*r', i.e. multiplication which allows
//     for the storage of intermediate results.

ATermAppl gsMakeDataExprDiv(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS is a data expression of sort Nat or Int
//     DataExprRHS is a data expression of sort Pos
//Ret: Data expression for the quotient after division of DataExprLHS and
//     DataExprRHS, of which the sort is equal to that of DataExprLHS

ATermAppl gsMakeDataExprMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS is a data expression of sort Pos, Nat or Int
//     DataExprRHS is a data expression of sort Nat
//Ret: Data expression for the remainder after division of DataExprLHS and
//     DataExprRHS, which is of sort Nat

ATermAppl gsMakeDataExprExp(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS is a data expression of sort Nat or Int
//     DataExprRHS is a data expression of sort Pos
//Ret: Data expression for the exponentiation of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprEven(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat, which we denote by n
//Ret: Data expression for 'even(n)', of sort Bool


//Auxiliary functions concerning data expressions 
ATermAppl gsMakeDataAppl2(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2);
//Pre: DataExpr, DataExprArg1 and DataExprArg2 are data expressions
//Ret: Internal representation of the data expression
//     DataExpr(DataExprArg1)(DataExprArg2)

ATermAppl gsMakeDataAppl3(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3);
//Pre: DataExpr, DataExprArg1, DataExprArg2 and DataExprArg3 are data
//     expressions
//Ret: Internal representation of the data expression
//     DataExpr(DataExprArg1)(DataExprArg2)(DataExprArg3)

ATermAppl gsMakeDataAppl4(ATermAppl DataExpr, ATermAppl DataExprArg1,
  ATermAppl DataExprArg2, ATermAppl DataExprArg3, ATermAppl DataExprArg4);
//Pre: DataExpr, DataExprArg1, DataExprArg2, DataExprArg3 and DataExprArg4 are
//     data expressions
//Ret: Internal representation of the data expression
//     DataExpr(DataExprArg1)(DataExprArg2)(DataExprArg3)(DataExprArg4)

ATermAppl gsMakeDataApplList(ATermAppl DataExpr, ATermList DataExprArgs);
//Pre: DataExpr is a data expression, which we denote by e.
//     DataExprArgs is of the form [e_0, ..., e_n], where n is a natural
//     number and each e_i, 0 <= i <= n, is a data expression.
//Ret: Internal representation of the data expression e(e_0)...(e_n).

ATermAppl gsMakeDataExprPos(char *p);
//Pre: p is of the form "[1-9][0-9]*"
//Ret: data expression of sort Pos that is a representation of p

ATermAppl gsMakeDataExprPos_int(int p);
//Pre: p > 0
//Ret: data expression of sort Pos that is a representation of p

ATermAppl gsMakeDataExprNat(char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: data expression of sort Nat that is a representation of n

ATermAppl gsMakeDataExprNat_int(int n);
//Pre: n >= 0
//Ret: data expression of sort Nat that is a representation of n

ATermAppl gsMakeDataExprInt(char *z);
//Pre: z is of the form "0 | -? [1-9][0-9]*"
//Ret: data expression of sort Int that is a representation of z

ATermAppl gsMakeDataExprInt_int(int z);
//Ret: data expression of sort Int that is a representation of z

bool gsIsPosConstant(const ATermAppl PosExpr);
//Pre: PosExpr is a data expression of sort Pos
//Ret: PosExpr is built from constructors only

char *gsPosValue(const ATermAppl PosConstant);
//Pre: PosConstant is a data expression of sort Pos built from constructors only
//Ret: The value of PosExpr
//     Note that the result is created with malloc, so it has to be freed

int gsPosValue_int(const ATermAppl PosConstant);
//Pre: PosConstant is a data expression of sort Pos built from constructors only
//Ret: The value of PosExpr

bool gsIsNatConstant(const ATermAppl NatExpr);
//Pre: NatExpr is a data expression of sort Nat
//Ret: NatExpr is built from constructors only

char *gsNatValue(const ATermAppl NatConstant);
//Pre: NatConstant is a data expression of sort Nat built from constructors only
//Ret: The value of NatExpr
//     Note that the result is created with malloc, so it has to be freed

int gsNatValue_int(const ATermAppl NatConstant);
//Pre: NatConstant is a data expression of sort Nat built from constructors only
//Ret: The value of NatExpr

bool gsIsIntConstant(const ATermAppl IntExpr);
//Pre: IntExpr is a data expression of sort Int
//Ret: IntExpr is built from constructors only

char *gsIntValue(const ATermAppl IntConstant);
//Pre: IntConstant is a data expression of sort Int built from constructors only
//Ret: The value of IntExpr
//     Note that the result is created with malloc, so it has to be freed

int gsIntValue_int(const ATermAppl IntConstant);
//Pre: IntConstant is a data expression of sort Int built from constructors only
//Ret: The value of IntExpr

ATermAppl gsGetDataExprHead(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//ret: the head of the data expression

ATermList gsGetDataExprArgs(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: the arguments of the data expression

bool gsIsOpIdPrefix(ATermAppl Term);
//Ret: DataExpr is a prefix operation identifier

bool gsIsOpIdInfix(ATermAppl Term);
//Ret: DataExpr is an infix operation identifier

int gsPrecOpIdInfix(ATermAppl OpIdInfix);
//Pre: OpIdInfix is an infix operation identifier
//Ret: Precedence of the operation itself

int gsPrecOpIdInfixLeft(ATermAppl OpIdInfix);
//Pre: OpIdInfix is an infix operation identifier
//Ret: Precedence of the left argument of the operation

int gsPrecOpIdInfixRight(ATermAppl OpIdInfix);
//Pre: OpIdInfix is an infix operation identifier
//Ret: Precedence of the right argument of the operation

//low level functions
//-------------------

char *gsStringDiv2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the smallest string representation of n div 2
//     Note that the result is created with malloc, so it has to be freed
     
int gsStringMod2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the value of n mod 2

char *gsStringDub(const char *n, const int inc);
//Pre: n is of the form "0 | [1-9][0-9]*"
//     0 <= inc <= 1
//Ret: the smallest string representation of 2*n + inc,
//     Note that the result is created with malloc, so it has to be freed

#ifdef __cplusplus
}
#endif
