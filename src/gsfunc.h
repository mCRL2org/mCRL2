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
//To eliminate ridiculous type casts in the rest of the code, we introduce
//wrappers around functions ATelementAt and ATgetArgument.
//This is caused by a bad interface design of the ATerm library

ATermAppl ATAelementAt(ATermList List, int Index);
ATermList ATLelementAt(ATermList List, int Index);
ATermAppl ATAgetArgument(ATermAppl Appl, int Nr);
ATermList ATLgetArgument(ATermAppl Appl, int Nr);
ATermAppl ATAgetFirst(ATermList List);
ATermList ATLgetFirst(ATermList List);
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
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult);
ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult);
ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecName); 
//Pre: RecName may be NULL
ATermAppl gsMakeStructProj(ATermAppl ProjName, ATermList SortExprs);
//Pre: ProjName may be NULL
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
ATermAppl gsMakeRestrict(ATermList ActNames, ATermAppl ProcExpr);
ATermAppl gsMakeHide(ATermList ActNames, ATermAppl ProcExpr);
ATermAppl gsMakeRename(ATermList RenameExprs, ATermAppl ProcExpr);
ATermAppl gsMakeComm(ATermList CommExprs, ATermAppl ProcExpr);
ATermAppl gsMakeAllow(ATermList MultActNames, ATermAppl ProcExpr);
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
//Pre: ActName may be NULL

//Recognisers of all constructor elements of the internal ATerm structure.
bool gsIsSpecV1(ATermAppl Term);
bool gsIsSortSpec(ATermAppl Term);
bool gsIsConsSpec(ATermAppl Term);
bool gsIsMapSpec(ATermAppl Term);
bool gsIsDataEqnSpec(ATermAppl Term);
bool gsIsActSpec(ATermAppl Term);
bool gsIsProcEqnSpec(ATermAppl Term);
bool gsIsInit(ATermAppl Term);
bool gsIsSortId(ATermAppl Term);
bool gsIsSortRef(ATermAppl Term);
bool gsIsOpId(ATermAppl Term);
bool gsIsDataEqn(ATermAppl Term);
bool gsIsDataVarId(ATermAppl Term);
bool gsIsNil(ATermAppl Term);
bool gsIsActId(ATermAppl Term);
bool gsIsProcEqn(ATermAppl Term);
bool gsIsProcVarId(ATermAppl Term);
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
bool gsIsAction(ATermAppl Term);
bool gsIsProcess(ATermAppl Term);
bool gsIsDelta(ATermAppl Term);
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

ATermAppl gsMakeOpIdOne(void);
//Ret: Operation identifier for `1' of sort Pos

ATermAppl gsMakeOpIdDoublePos(void);
//Ret: Operation identifier for `.0', i.e. double a positive number

ATermAppl gsMakeOpIdDoublePosPlusOne(void);
//Ret: Operation identifier for `.1', i.e. double a positive number and add `1'

ATermAppl gsMakeOpIdZero(void);
//Ret: Operation identifier for `0' of sort Nat

ATermAppl gsMakeOpIdPosAsNat(void);
//Ret: Operation identifier for the creation of a natural number from a
//     positive number

ATermAppl gsMakeOpIdNegatePos(void);
//Ret: Operation identifier for the negation of a positive number

ATermAppl gsMakeOpIdNatAsInt(void);
//Ret: Operation identifier for the creation of an integer from a natural
//     number

ATermAppl gsMakeOpIdLT(ATermAppl SortId);
//Pre: SortId is Pos, Nat or Int
//Ret: Operation identifier for `less than' on SortId

ATermAppl gsMakeOpIdGT(ATermAppl SortId);
//Pre: SortId is Pos, Nat or Int
//Ret: Operation identifier for `greater than' on SortId

ATermAppl gsMakeOpIdLTE(ATermAppl SortId);
//Pre: SortId is Pos, Nat or Int
//Ret: Operation identifier for `less than or equal' on SortId

ATermAppl gsMakeOpIdGTE(ATermAppl SortId);
//Pre: SortId is Pos, Nat or Int
//Ret: Operation identifier for `greater than or equal' on SortId


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

ATermAppl gsMakeDataExprOne(void);
//Ret: Data expression for `1' of sort Pos

ATermAppl gsMakeDataExprDoublePos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos, which we denote by e 
//Ret: Data expression for `.0(e)', i.e. double positive number e

ATermAppl gsMakeDataExprDoublePosPlusOne(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos, which we denote by e 
//Ret: Data expression for `.1(e)', i.e. double positive number e and add `1'

ATermAppl gsMakeDataExprZero(void);
//Ret: Data expression for `0' of sort Nat

ATermAppl gsMakeDataExprPosAsNat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: DataExpr as a data expression of sort Nat

ATermAppl gsMakeDataExprNegatePos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the negation of DataExpr

ATermAppl gsMakeDataExprNatAsInt(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: DataExpr as a data expression of sort Int

ATermAppl gsMakeDataExprLT(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `less than' of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprGT(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `greater than' of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprLTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `less than or equal' of DataExprLHS and
//     DataExprRHS

ATermAppl gsMakeDataExprGTE(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the `greater than or equal' of DataExprLHS and
//     DataExprRHS


//Auxiliary functions concerning data expressions 
ATermAppl gsMakeDataApplList(ATermAppl DataExpr, ATermList DataExprArgs);
//Pre: DataExpr is a data expression, which we denote by e.
//     DataExprArgs is of the form [e_0, ..., e_n], where n is a natural
//     number and each e_i, 0 <= i <= n, is a data expression.
//Ret: Internal representation of the data expression e(e_0)...(e_n).

ATermAppl gsMakeDataExprPos(char *p);
//Pre: p is of the form "[1-9][0-9]*"
//Ret: data expression of sort Pos that is a representation of p

ATermAppl gsMakeDataExprNat(char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: data expression of sort Nat that is a representation of n

ATermAppl gsMakeDataExprInt(char *z);
//Pre: z is of the form "0 | -? [1-9][0-9]*"
//Ret: data expression of sort Int that is a representation of z


#ifdef __cplusplus
}
#endif
