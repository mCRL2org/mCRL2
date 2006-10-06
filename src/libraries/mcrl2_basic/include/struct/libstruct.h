#ifndef MCRL_LIBSTRUCT_H
#define MCRL_LIBSTRUCT_H

//This file describes the functions that can be used for the internal ATerm
//structure.

#include <stdbool.h>
#include <aterm2.h>

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

ATermAppl gsString2ATermAppl(const char *s);
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL

char *gsATermAppl2String(ATermAppl term);
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise


//Enabling constructor functions
//------------------------------

bool gsConstructorFunctionsEnabled(void);
//Ret:  the constructor creator and recogniser functions are enabled

//gsEnableConstrucorFunctions() should be called before calling any of the
//remaining functions in this file.
void gsEnableConstructorFunctions(void);
//Post: the constructor creator and recogniser functions are enabled


//Creation of all constructor elements of the internal structure
//--------------------------------------------------------------

//sort expressions
ATermAppl gsMakeSortList(ATermAppl SortExpr);
ATermAppl gsMakeSortSet(ATermAppl SortExpr);
ATermAppl gsMakeSortBag(ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult);
ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult);
ATermAppl gsMakeSortId(ATermAppl Name);
ATermAppl gsMakeStructCons(ATermAppl ConsName, ATermList StructProjs,
  ATermAppl RecNameOrNil); 
ATermAppl gsMakeStructProj(ATermAppl ProjNameOrNil, ATermAppl SortExpr);
ATermAppl gsMakeNil();
ATermAppl gsMakeUnknown();
//data expressions
ATermAppl gsMakeId(ATermAppl Name);
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
ATermAppl gsMakeDataVarId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeOpId(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity);
ATermAppl gsMakeWhrDecl(ATermAppl Name, ATermAppl DataExpr);
//multi-actions
ATermAppl gsMakeMultAct(ATermList Actions);
ATermAppl gsMakeParamId(ATermAppl Name, ATermList DataExprs);
ATermAppl gsMakeAction(ATermAppl ActId, ATermList DataExprs);
ATermAppl gsMakeActId(ATermAppl Name, ATermList SortExprs);
//process expressions
ATermAppl gsMakeProcess(ATermAppl ProcVarId, ATermList DataExprs);
ATermAppl gsMakeDelta();
ATermAppl gsMakeTau();
ATermAppl gsMakeSum(ATermList DataVarIds, ATermAppl ProcExpr);
ATermAppl gsMakeBlock(ATermList ActNames, ATermAppl ProcExpr);
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
ATermAppl gsMakeProcVarId(ATermAppl Name,ATermList SortExprs);
ATermAppl gsMakeMultActName(ATermList ActNames);
ATermAppl gsMakeRenameExpr(ATermAppl FromName, ATermAppl ToName);
ATermAppl gsMakeCommExpr(ATermAppl MultActName, ATermAppl ActNameOrNil);
//mCRL2 specifications
ATermAppl gsMakeSpecV1(
  ATermAppl SortSpec, ATermAppl ConsSpec, ATermAppl MapSpec,
  ATermAppl DataEqnSpec, ATermAppl ActSpec, ATermAppl ProcEqnSpec,
  ATermAppl Init);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeConsSpec(ATermList OpIds);
ATermAppl gsMakeMapSpec(ATermList OpIds);
ATermAppl gsMakeDataEqnSpec(ATermList DataEqns);
ATermAppl gsMakeActSpec(ATermList ActIds);
ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl BoolExprOrNil,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS);
ATermAppl gsMakeProcEqnSpec(ATermList ProcEqns);
ATermAppl gsMakeLPE(ATermList GlobDataVarIds, ATermList ProcDataVarIds,
  ATermList LPESummands);
ATermAppl gsMakeProcEqn(ATermList GlobDataVarIds, ATermAppl ProcVarId,
  ATermList ProcDataVarIds, ATermAppl ProcExp);
ATermAppl gsMakeLPESummand(ATermList DataVarIds, ATermAppl BoolExpr,
  ATermAppl MultActOrDelta, ATermAppl TimeExprOrNil, ATermList Assignments);
ATermAppl gsMakeAssignment(ATermAppl DataVarId, ATermAppl DataExpr);
ATermAppl gsMakeInit(ATermList GlobDataVarIds, ATermAppl ProcExpr);
ATermAppl gsMakeLPEInit(ATermList GlobDataVarIds, ATermList DataExprs);
//mu-calculus formulas
ATermAppl gsMakeStateTrue();
ATermAppl gsMakeStateFalse();
ATermAppl gsMakeStateNot(ATermAppl StateFrm);
ATermAppl gsMakeStateAnd(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS);
ATermAppl gsMakeStateOr(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS);
ATermAppl gsMakeStateImp(ATermAppl StateFrmLHS, ATermAppl StateFrmRHS);
ATermAppl gsMakeStateForall(ATermList DataVarIds, ATermAppl StateFrm);
ATermAppl gsMakeStateExists(ATermList DataVarIds, ATermAppl StateFrm);
ATermAppl gsMakeStateMust(ATermAppl RegFrm, ATermAppl StateFrm);
ATermAppl gsMakeStateMay(ATermAppl RegFrm, ATermAppl StateFrm);
ATermAppl gsMakeStateYaled();
ATermAppl gsMakeStateYaledTimed(ATermAppl DataExpr);
ATermAppl gsMakeStateDelay();
ATermAppl gsMakeStateDelayTimed(ATermAppl DataExpr);
ATermAppl gsMakeStateVar(ATermAppl VarName, ATermList DataExprs);
ATermAppl gsMakeStateNu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm);
ATermAppl gsMakeStateMu(ATermAppl VarName, ATermList DataVarIdInits, ATermAppl StateFrm);
ATermAppl gsMakeDataVarIdInit(ATermAppl VarName, ATermAppl SortExpr, ATermAppl DataExpr);
ATermAppl gsMakeRegNil();
ATermAppl gsMakeRegSeq(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS);
ATermAppl gsMakeRegAlt(ATermAppl RegFrmLHS, ATermAppl RegFrmRHS);
ATermAppl gsMakeRegTrans(ATermAppl RegFrm);
ATermAppl gsMakeRegTransOrNil(ATermAppl RegFrm);
ATermAppl gsMakeActTrue();
ATermAppl gsMakeActFalse();
ATermAppl gsMakeActNot(ATermAppl ActFrm);
ATermAppl gsMakeActAnd(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS);
ATermAppl gsMakeActOr(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS);
ATermAppl gsMakeActImp(ATermAppl ActFrmLHS, ATermAppl ActFrmRHS);
ATermAppl gsMakeActForall(ATermList DataVarIds, ATermAppl ActFrm);
ATermAppl gsMakeActExists(ATermList DataVarIds, ATermAppl ActFrm);
ATermAppl gsMakeActAt(ATermAppl ActFrm, ATermAppl DataExpr);
//PBES's
ATermAppl gsMakePBES(ATermAppl DataSpec, ATermList PBEqn, ATermAppl PropVarInst);
ATermAppl gsMakePropVarInst(ATermAppl Name, ATermList DataExprs);
ATermAppl gsMakePBEqn(ATermAppl FixPoint, ATermAppl PropVarDecl, ATermAppl PBExpr);
ATermAppl gsMakeMu();
ATermAppl gsMakeNu();
ATermAppl gsMakePropVarDecl(ATermAppl Name, ATermList DataVarIds);
ATermAppl gsMakePBESTrue();
ATermAppl gsMakePBESFalse();
ATermAppl gsMakePBESAnd(ATermAppl PBExprLHS, ATermAppl PBExprRHS);
ATermAppl gsMakePBESOr(ATermAppl PBExprLHS, ATermAppl PBExprRHS);
ATermAppl gsMakePBESForall(ATermList DataVarId, ATermAppl PBExpr);
ATermAppl gsMakePBESExists(ATermList DataVarId, ATermAppl PBExpr);
ATermAppl gsMakeDataSpec(ATermAppl SortSpec, ATermAppl ConsSpec,
            ATermAppl MapSpec, ATermAppl DataEqnSpec);


//Recognisers of all constructor elements of the internal structure
//-----------------------------------------------------------------

//sort expressions
bool gsIsSortList(ATermAppl Term);
bool gsIsSortSet(ATermAppl Term);
bool gsIsSortBag(ATermAppl Term);
bool gsIsSortStruct(ATermAppl Term);
bool gsIsSortArrowProd(ATermAppl Term);
bool gsIsSortArrow(ATermAppl Term);
bool gsIsSortId(ATermAppl Term);
bool gsIsStructCons(ATermAppl Term);
bool gsIsStructProj(ATermAppl Term);
bool gsIsNil(ATermAppl Term);
bool gsIsUnknown(ATermAppl Term);
//data expressions
bool gsIsId(ATermAppl Term);
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
bool gsIsDataVarId(ATermAppl Term);
bool gsIsOpId(ATermAppl Term);
bool gsIsBagEnumElt(ATermAppl Term);
bool gsIsWhrDecl(ATermAppl Term);
//multi-actions
bool gsIsMultAct(ATermAppl Term);
bool gsIsParamId(ATermAppl Term);
bool gsIsAction(ATermAppl Term);
bool gsIsActId(ATermAppl Term);
//process expressions
bool gsIsProcess(ATermAppl Term);
bool gsIsDelta(ATermAppl Term);
bool gsIsTau(ATermAppl Term);
bool gsIsSum(ATermAppl Term);
bool gsIsBlock(ATermAppl Term);
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
bool gsIsProcVarId(ATermAppl Term);
bool gsIsMultActName(ATermAppl Term);
bool gsIsRenameExpr(ATermAppl Term);
bool gsIsCommExpr(ATermAppl Term);
//mCRL2 specifications
bool gsIsSpecV1(ATermAppl Term);
bool gsIsSortSpec(ATermAppl Term);
bool gsIsConsSpec(ATermAppl Term);
bool gsIsMapSpec(ATermAppl Term);
bool gsIsDataEqnSpec(ATermAppl Term);
bool gsIsActSpec(ATermAppl Term);
bool gsIsSortRef(ATermAppl Term);
bool gsIsDataEqn(ATermAppl Term);
bool gsIsProcEqnSpec(ATermAppl Term);
bool gsIsLPE(ATermAppl Term);
bool gsIsProcEqn(ATermAppl Term);
bool gsIsLPESummand(ATermAppl Term);
bool gsIsAssignment(ATermAppl Term);
bool gsIsInit(ATermAppl Term);
bool gsIsLPEInit(ATermAppl Term);
//mu-calculus formulas
bool gsIsStateTrue(ATermAppl Term);
bool gsIsStateFalse(ATermAppl Term);
bool gsIsStateNot(ATermAppl Term);
bool gsIsStateAnd(ATermAppl Term);
bool gsIsStateOr(ATermAppl Term);
bool gsIsStateImp(ATermAppl Term);
bool gsIsStateForall(ATermAppl Term);
bool gsIsStateExists(ATermAppl Term);
bool gsIsStateMust(ATermAppl Term);
bool gsIsStateMay(ATermAppl Term);
bool gsIsStateYaled(ATermAppl Term);
bool gsIsStateYaledTimed(ATermAppl Term);
bool gsIsStateDelay(ATermAppl Term);
bool gsIsStateDelayTimed(ATermAppl Term);
bool gsIsStateVar(ATermAppl Term);
bool gsIsStateNu(ATermAppl Term);
bool gsIsStateMu(ATermAppl Term);
bool gsIsDataVarIdInit(ATermAppl Term);
bool gsIsRegNil(ATermAppl Term);
bool gsIsRegSeq(ATermAppl Term);
bool gsIsRegAlt(ATermAppl Term);
bool gsIsRegTrans(ATermAppl Term);
bool gsIsRegTransOrNil(ATermAppl Term);
bool gsIsActTrue(ATermAppl Term);
bool gsIsActFalse(ATermAppl Term);
bool gsIsActNot(ATermAppl Term);
bool gsIsActAnd(ATermAppl Term);
bool gsIsActOr(ATermAppl Term);
bool gsIsActImp(ATermAppl Term);
bool gsIsActForall(ATermAppl Term);
bool gsIsActExists(ATermAppl Term);
bool gsIsActAt(ATermAppl Term);
//PBES's
bool gsIsPBES(ATermAppl Term);
bool gsIsPropVarInst(ATermAppl Term);
bool gsIsPBEqn(ATermAppl Term);
bool gsIsMu(ATermAppl Term);
bool gsIsNu(ATermAppl Term);
bool gsIsPropVarDecl(ATermAppl Term);
bool gsIsPBESTrue(ATermAppl Term);
bool gsIsPBESFalse(ATermAppl Term);
bool gsIsPBESAnd(ATermAppl Term);
bool gsIsPBESOr(ATermAppl Term);
bool gsIsPBESForall(ATermAppl Term);
bool gsIsPBESExists(ATermAppl Term);
bool gsIsDataSpec(ATermAppl Term);

//Sort expressions
//----------------

bool gsIsSortExpr(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a sort expression

bool gsIsSortExprOrUnknown(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a sort expression or unknown

//Creation of names for system sort identifiers

ATermAppl gsMakeSortIdNameBool();
ATermAppl gsMakeSortIdNamePos();
ATermAppl gsMakeSortIdNameNat();
ATermAppl gsMakeSortIdNameNatPair();
ATermAppl gsMakeSortIdNameInt();
ATermAppl gsMakeSortIdNameReal();


//Creation of sort identifiers for system defined sorts

ATermAppl gsMakeSortIdBool(void);
//Ret: Sort identifier for `Bool'

ATermAppl gsMakeSortIdPos(void);
//Ret: Sort identifier for `Pos'

ATermAppl gsMakeSortIdNat(void);
//Ret: Sort identifier for `Nat'

ATermAppl gsMakeSortIdNatPair(void);
//Ret: Sort identifier for `NatPair'

ATermAppl gsMakeSortIdInt(void);
//Ret: Sort identifier for `Int'

ATermAppl gsMakeSortIdReal(void);
//Ret: Sort identifier for `Real'


//Creation of sort expressions for system defined sorts

ATermAppl gsMakeSortExprBool(void);
//Ret: Sort expression for `Bool'

ATermAppl gsMakeSortExprPos(void);
//Ret: Sort expression for `Pos'

ATermAppl gsMakeSortExprNat(void);
//Ret: Sort expression for `Nat'

ATermAppl gsMakeSortExprNatPair(void);
//Ret: Sort expression for `NatPair'

ATermAppl gsMakeSortExprInt(void);
//Ret: Sort expression for `Int'

ATermAppl gsMakeSortExprReal(void);
//Ret: Sort expression for `Real'


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

ATermAppl gsGetSortExprResult(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//ret: the result of the sort expression

ATermList gsGetSortExprDomain(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: the domain of the sort expression


//Data expressions
//----------------

bool gsIsDataExpr(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a data expression

ATermAppl gsGetSort(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: the sort of DataExpr, if the sort can be inferred from the sort
//     information in DataExpr
//     Unknown, otherwise

ATermAppl gsGetDataExprHead(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//ret: the head of the data expression

ATermList gsGetDataExprArgs(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: the arguments of the data expression


//Creation of names for system operation identifiers

ATermAppl gsMakeOpIdNameTrue();
ATermAppl gsMakeOpIdNameFalse();
ATermAppl gsMakeOpIdNameNot();
ATermAppl gsMakeOpIdNameAnd();
ATermAppl gsMakeOpIdNameOr();
ATermAppl gsMakeOpIdNameImp();
ATermAppl gsMakeOpIdNameEq();
ATermAppl gsMakeOpIdNameNeq();
ATermAppl gsMakeOpIdNameIf();
ATermAppl gsMakeOpIdNameForall();
ATermAppl gsMakeOpIdNameExists();
ATermAppl gsMakeOpIdNameC1();
ATermAppl gsMakeOpIdNameCDub();
ATermAppl gsMakeOpIdNameC0();
ATermAppl gsMakeOpIdNameCNat();
ATermAppl gsMakeOpIdNameCPair();
ATermAppl gsMakeOpIdNameCNeg();
ATermAppl gsMakeOpIdNameCInt();
ATermAppl gsMakeOpIdNameCReal();
ATermAppl gsMakeOpIdNamePos2Nat();
ATermAppl gsMakeOpIdNamePos2Int();
ATermAppl gsMakeOpIdNamePos2Real();
ATermAppl gsMakeOpIdNameNat2Pos();
ATermAppl gsMakeOpIdNameNat2Int();
ATermAppl gsMakeOpIdNameNat2Real();
ATermAppl gsMakeOpIdNameInt2Pos();
ATermAppl gsMakeOpIdNameInt2Nat();
ATermAppl gsMakeOpIdNameInt2Real();
ATermAppl gsMakeOpIdNameReal2Pos();
ATermAppl gsMakeOpIdNameReal2Nat();
ATermAppl gsMakeOpIdNameReal2Int();
ATermAppl gsMakeOpIdNameLTE();
ATermAppl gsMakeOpIdNameLT();
ATermAppl gsMakeOpIdNameGTE();
ATermAppl gsMakeOpIdNameGT();
ATermAppl gsMakeOpIdNameMax();
ATermAppl gsMakeOpIdNameMin();
ATermAppl gsMakeOpIdNameAbs();
ATermAppl gsMakeOpIdNameNeg();
ATermAppl gsMakeOpIdNameSucc();
ATermAppl gsMakeOpIdNamePred();
ATermAppl gsMakeOpIdNameDub();
ATermAppl gsMakeOpIdNameAdd();
ATermAppl gsMakeOpIdNameAddC();
ATermAppl gsMakeOpIdNameSubt();
ATermAppl gsMakeOpIdNameGTESubt();
ATermAppl gsMakeOpIdNameGTESubtB();
ATermAppl gsMakeOpIdNameMult();
ATermAppl gsMakeOpIdNameMultIR();
ATermAppl gsMakeOpIdNameDiv();
ATermAppl gsMakeOpIdNameMod();
ATermAppl gsMakeOpIdNameFirst();
ATermAppl gsMakeOpIdNameLast();
ATermAppl gsMakeOpIdNameDivMod();
ATermAppl gsMakeOpIdNameGDivMod();
ATermAppl gsMakeOpIdNameGGDivMod();
ATermAppl gsMakeOpIdNameExp();
ATermAppl gsMakeOpIdNameEven();
ATermAppl gsMakeOpIdNameEmptyList();
ATermAppl gsMakeOpIdNameListSize();
ATermAppl gsMakeOpIdNameCons();
ATermAppl gsMakeOpIdNameSnoc();
ATermAppl gsMakeOpIdNameConcat();
ATermAppl gsMakeOpIdNameEltAt();
ATermAppl gsMakeOpIdNameHead();
ATermAppl gsMakeOpIdNameTail();
ATermAppl gsMakeOpIdNameRHead();
ATermAppl gsMakeOpIdNameRTail();
ATermAppl gsMakeOpIdNameEltIn();
ATermAppl gsMakeOpIdNameSetComp();
ATermAppl gsMakeOpIdNameEmptySet();
ATermAppl gsMakeOpIdNameSubSetEq();
ATermAppl gsMakeOpIdNameSubSet();
ATermAppl gsMakeOpIdNameSetUnion();
ATermAppl gsMakeOpIdNameSetDiff();
ATermAppl gsMakeOpIdNameSetIntersect();
ATermAppl gsMakeOpIdNameSetCompl();
ATermAppl gsMakeOpIdNameBagComp();
ATermAppl gsMakeOpIdNameBag2Set();
ATermAppl gsMakeOpIdNameSet2Bag();
ATermAppl gsMakeOpIdNameEmptyBag();
ATermAppl gsMakeOpIdNameCount();
ATermAppl gsMakeOpIdNameSubBagEq();
ATermAppl gsMakeOpIdNameSubBag();
ATermAppl gsMakeOpIdNameBagUnion();
ATermAppl gsMakeOpIdNameBagDiff();
ATermAppl gsMakeOpIdNameBagIntersect();

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

ATermAppl gsMakeOpIdC1(void);
//Ret: Operation identifier for the Pos constructor `1'

ATermAppl gsMakeOpIdCDub(void);
//Ret: Operation identifier for the Pos constructor `double and add a bit'

ATermAppl gsMakeOpIdC0(void);
//Ret: Operation identifier for the Nat constructor `0'

ATermAppl gsMakeOpIdCNat(void);
//Ret: Operation identifier for the creation of a natural number from a
//     positive number

ATermAppl gsMakeOpIdCPair(void);
//Ret: Operation identifier for the creation of a pair of natural numbers

ATermAppl gsMakeOpIdCNeg(void);
//Ret: Operation identifier for the negation of a positive number

ATermAppl gsMakeOpIdCInt(void);
//Ret: Operation identifier for the creation of an integer from a natural
//     number

ATermAppl gsMakeOpIdCReal(void);
//Ret: Operation identifier for the creation of a real from an integer

ATermAppl gsMakeOpIdPos2Nat(void);
//Ret: Operation identifier for the conversion of Pos to Nat

ATermAppl gsMakeOpIdPos2Int(void);
//Ret: Operation identifier for the conversion of Pos to Int

ATermAppl gsMakeOpIdPos2Real(void);
//Ret: Operation identifier for the conversion of Pos to Real

ATermAppl gsMakeOpIdNat2Pos(void);
//Ret: Operation identifier for the conversion of Nat to Pos

ATermAppl gsMakeOpIdNat2Int(void);
//Ret: Operation identifier for the conversion of Nat to Int

ATermAppl gsMakeOpIdNat2Real(void);
//Ret: Operation identifier for the conversion of Nat to Real

ATermAppl gsMakeOpIdInt2Pos(void);
//Ret: Operation identifier for the conversion of Int to Pos

ATermAppl gsMakeOpIdInt2Nat(void);
//Ret: Operation identifier for the conversion of Int to Nat

ATermAppl gsMakeOpIdInt2Real(void);
//Ret: Operation identifier for the conversion of Int to Real

ATermAppl gsMakeOpIdReal2Pos(void);
//Ret: Operation identifier for the conversion of Real to Pos

ATermAppl gsMakeOpIdReal2Nat(void);
//Ret: Operation identifier for the conversion of Real to Nat

ATermAppl gsMakeOpIdReal2Int(void);
//Ret: Operation identifier for the conversion of Real to Int

ATermAppl gsMakeOpIdLTE(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `less than or equal' on SortExpr

ATermAppl gsMakeOpIdLT(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `less than' on SortExpr

ATermAppl gsMakeOpIdGTE(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `greater than or equal' on SortExpr

ATermAppl gsMakeOpIdGT(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `greater than' on SortExpr

ATermAppl gsMakeOpIdMax(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExprLHS, SortExprRHS is Pos, Nat or Int, or SortExprLHS and
//     SortExprRHS are both Real
//Ret: Operation identifier for `maximum' on SortExprLHS and SortExprRHS. The
//     result sort is the intersection of SortExprLHS and SortExprRHS.

ATermAppl gsMakeOpIdMin(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `minimum' on SortExpr

ATermAppl gsMakeOpIdAbs(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for `absolute value' on SortExpr. The result sort
//     is:
//     - SortExpr, if SortExpr is Pos, Nat or Real
//     - Nat, if SortExpr is Int

ATermAppl gsMakeOpIdNeg(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for negation. The result sort is union of SortExpr
//     and Int.

ATermAppl gsMakeOpIdSucc(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for the successor with argument sort SortExpr
//     The corresponding result sort is:
//     - Pos, if SortExpr is Nat
//     - SortExpr, otherwise

ATermAppl gsMakeOpIdPred(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for the predecessor with argument sort SortExpr
//     The corresponding result sort is:
//     - Nat, if SortExpr is Pos
//     - the union of SortExpr and Int, otherwise

ATermAppl gsMakeOpIdDub(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for '2*n + |b|', where n is of sort SortExpr and
//     b is of sort Bool

ATermAppl gsMakeOpIdAdd(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExprLHS, SortExprRHS is Pos or Nat, or SortExprLHS and SortExprRHS
//     are both equal to Int or Real.
//Ret: Operation identifier for addition on SortExprLHS and SortExprRHS which
//     has the intersection of SortExprLHS and SortExprRHS as a result sort.

ATermAppl gsMakeOpIdAddC();
//Ret: Operation identifier for the addition of two positive numbers and a
//     (carry) bit, which is of sort Bool -> Pos -> Pos -> Pos

ATermAppl gsMakeOpIdSubt(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for subtraction on SortExpr, which has the union of
//     SortExpr and Int as a result sort.

ATermAppl gsMakeOpIdGTESubt(ATermAppl SortExpr);
//Pre: SortExpr is Pos or Nat
//Ret: Operation identifier for subtraction 'x - y', where x >= y and x,y are
//     both of sort Pos or Nat
//     The identifier is of sort -> SortExpr -> SortExpr -> Nat

ATermAppl gsMakeOpIdGTESubtB();
//Ret: Operation identifier for 'p - (q + |b|)', i.e. subtraction with borrow,
//     where p >= q + |b|.
//     The identifier is of sort Bool -> Pos -> Pos -> Nat

ATermAppl gsMakeOpIdMult(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for multiplication on SortExpr

ATermAppl gsMakeOpIdMultIR();
//Ret: Operation identifier for multiplication of two positive numbers, plus an
//     optional intermediate positive result, which is of sort
//     Bool -> Pos -> Pos -> Pos -> Pos

ATermAppl gsMakeOpIdDiv(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `quotient after division', which has sort
//     SortExpr -> Pos -> S', where S' stands for the union of SortExpr and Nat

ATermAppl gsMakeOpIdMod(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat or Int
//Ret: Operation identifier for `remainder after division', which has sort
//     SortExpr -> Pos -> Nat

ATermAppl gsMakeOpIdDivMod();
//Ret: Operation identifier for `quotient and remainder after division',
//     which has sort Pos -> Pos -> NatPair
//     Specification:
//     divmod(p, q) = <p div q, p mod q>

ATermAppl gsMakeOpIdGDivMod();
//Ret: Operation identifier for `generalised quotient and remainder after division',
//     which has sort NatPair -> Bool -> Pos -> NatPair
//     Specification:
//       gdivmod(< m, n >, b, p)  =  if  l <  p  ->  < 2*m  , l >
//                                   []  l >= p  ->  < 2*m+1, l-p >
//                                   fi whr l = 2*n + b end

ATermAppl gsMakeOpIdGGDivMod();
//Ret: Operation identifier for `generalised generalised quotient and remainder after division',
//     which has sort Nat -> Nat -> Pos -> NatPair
//     Specification:
//       ggdivmod(m, n, p)  =  if  m <  p  ->  < 2*n  , m >
//                             []  m >= p  ->  < 2*n+1, m-p >
//                             fi

ATermAppl gsMakeOpIdFirst();
//Ret: Operation identifier for 'first' of sort NatPair -> Nat

ATermAppl gsMakeOpIdLast();
//Ret: Operation identifier for 'last' of sort NatPair -> Nat

ATermAppl gsMakeOpIdExp(ATermAppl SortExpr);
//Pre: SortExpr is Pos, Nat, Int or Real
//Ret: Operation identifier for exponentiation, which has sort S -> Nat -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdEven();
//Ret: Operation identifier for 'even' of sort Nat -> Bool

ATermAppl gsMakeOpIdEmptyList(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the empty list of sort SortExpr

ATermAppl gsMakeOpIdCons(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExprLHS and SortExprRHS are sort expressions
//Ret: Operation identifier for 'list cons (element at the head of a list)' of
//     sort S -> T -> T, where S and T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdListSize(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for list size of sort SortExpr -> Nat

ATermAppl gsMakeOpIdSnoc(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExprLHS and SortExprRHS are a sort expressions
//Ret: Operation identifier for 'list snoc (element at the tail of a list' of
//     sort S -> T -> S, where S and T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdConcat(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'list concatenation' of sort S -> S -> S, where
//     S stands for SortExpr

ATermAppl gsMakeOpIdEltAt(ATermAppl SortExprDom, ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for 'element at position', which has sort
//     S -> Nat -> T, where S and T stand for SortExprDom and SortExprResult

ATermAppl gsMakeOpIdHead(ATermAppl SortExprDom, ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for 'head', which has sort S -> T, where S and
//     T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdTail(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'tail', which has sort S -> S, where S
//     stands for SortExpr

ATermAppl gsMakeOpIdRHead(ATermAppl SortExprDom, ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for 'right head', which has sort S -> T, where S
//     and T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdRTail(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'right tail', which has sort S -> S, where S
//     stands for SortExpr

ATermAppl gsMakeOpIdEltIn(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExprLHS and SortExprRHS are sort expressions
//Ret: Operation identifier for 'element test', which has sort
//     S -> T -> Bool, where S and T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdSetComp(ATermAppl SortExprDom, ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for set comprehension of sort (S -> Bool) -> T,
//     where S and T stand for SortExprDom and SortExprResult
     
ATermAppl gsMakeOpIdEmptySet(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the empty set of sort SortExpr

ATermAppl gsMakeOpIdSubSetEq(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'subset or equal', which has sort
//     S -> S -> Bool, where S stands for SortExpr

ATermAppl gsMakeOpIdSubSet(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'proper subset', which has sort S -> S -> Bool,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdSetUnion(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'set union', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdSetDiff(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'set difference', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdSetIntersect(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'set intersection', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdSetCompl(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'set complement', which has sort S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdBagComp(ATermAppl SortExprDom, ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for bag comprehension of sort (S -> Nat) -> T,
//     where S and T stand for SortExprDom and SortExprResult

ATermAppl gsMakeOpIdEmptyBag(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for the empty bag of sort SortExpr

ATermAppl gsMakeOpIdCount(ATermAppl SortExprLHS, ATermAppl SortExprRHS);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'bag multiplicity', which has sort
//     S -> T -> Nat, where S and T stand for SortExprLHS and SortExprRHS

ATermAppl gsMakeOpIdSubBagEq(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'subbag or equal', which has sort
//     S -> S -> Bool, where S stands for SortExpr

ATermAppl gsMakeOpIdSubBag(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'proper subbag', which has sort S -> S -> Bool,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdBagUnion(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'bag union', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdBagDiff(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'bag difference', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdBagIntersect(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Operation identifier for 'bag intersection', which has sort S -> S -> S,
//     where S stands for SortExpr

ATermAppl gsMakeOpIdBag2Set(ATermAppl SortExprDom,
  ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for bag to set conversion of sort S -> T,
//     where S and T stand for SortExprDom and SortExprResult
     
ATermAppl gsMakeOpIdSet2Bag(ATermAppl SortExprDom,
  ATermAppl SortExprResult);
//Pre: SortExprDom and SortExprResult are sort expressions
//Ret: Operation identifier for set to bag conversion of sort S -> T,
//     where S and T stand for SortExprDom and SortExprResult
     

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

ATermAppl gsMakeDataExprForall(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort S -> Bool
//Ret: Data expression for the universal quantification over DataExpr

ATermAppl gsMakeDataExprExists(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort S -> Bool
//Ret: Data expression for the existential quantification over DataExpr

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

ATermAppl gsMakeDataExprC1(void);
//Ret: Data expression for `1' of sort Pos

ATermAppl gsMakeDataExprCDub(ATermAppl DataExprBit, ATermAppl DataExprPos);
//Pre: DataExprBit and DataExprPos are data expressions of sort Bool and Pos,
//     respectively, which we denote by b and p
//Ret: Data expression for `2*p + |b|', where '|b|' is defined as:
//       |true|  = 1
//       |false| = 0

ATermAppl gsMakeDataExprC0(void);
//Ret: Data expression for `0' of sort Nat

ATermAppl gsMakeDataExprCNat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: DataExpr as a data expression of sort Nat

ATermAppl gsMakeDataExprCPair(ATermAppl DataExprFst, ATermAppl DataExprLst);
//Pre: DataExprFst and DataExprLst are data expressions of sort Nat
//Ret: Data expression for the pair of DataExprFst and DataExprLst

ATermAppl gsMakeDataExprCNeg(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the negation of DataExpr

ATermAppl gsMakeDataExprCInt(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: DataExpr as a data expression of sort Int

ATermAppl gsMakeDataExprCReal(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int
//Ret: DataExpr as a data expression of sort Real

ATermAppl gsMakeDataExprPos2Nat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the conversion of DataExpr to Nat

ATermAppl gsMakeDataExprPos2Int(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the conversion of DataExpr to Int

ATermAppl gsMakeDataExprPos2Real(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Pos 
//Ret: Data expression for the conversion of DataExpr to Real

ATermAppl gsMakeDataExprNat2Pos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: Data expression for the conversion of DataExpr to Pos

ATermAppl gsMakeDataExprNat2Int(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: Data expression for the conversion of DataExpr to Int

ATermAppl gsMakeDataExprNat2Real(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat 
//Ret: Data expression for the conversion of DataExpr to Real

ATermAppl gsMakeDataExprInt2Pos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the conversion of DataExpr to Pos

ATermAppl gsMakeDataExprInt2Nat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the conversion of DataExpr to Nat

ATermAppl gsMakeDataExprInt2Real(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Int 
//Ret: Data expression for the conversion of DataExpr to Real

ATermAppl gsMakeDataExprReal2Pos(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Real 
//Ret: Data expression for the conversion of DataExpr to Pos

ATermAppl gsMakeDataExprReal2Nat(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Real
//Ret: Data expression for the conversion of DataExpr to Nat

ATermAppl gsMakeDataExprReal2Int(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Real
//Ret: Data expression for the conversion of DataExpr to Int

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
//Pre: DataExpr is a data expression of sort Pos, Nat or Int
//Ret: Data expression for the absolute value of DataExpr of sort
//     intersect(SortExpr, Nat)

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
//Pre: DataExprLHS and DataExprRHS are data expressions of sort Pos, Nat or
//     Int, denoted by t and u
//Ret: Data expression for t + u (see gsMakeOpIdAdd for information about the
//     result sort)

ATermAppl gsMakeDataExprAddC(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS);
//Pre: DataExprBit, DataExprLHS and DataExprRHS are data expressions of sort
//     Bool, Pos and Pos, respectively, which we denote by b, p and q.
//Ret: Data expression for 'p + q + |b|', i.e. addition with carry.

ATermAppl gsMakeDataExprSubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos, Nat
//     or Int
//Ret: Data expression for the subtraction of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprGTESubt(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are both data expressions of sort Pos or
//     Nat, which we denote by x and y.
//     x >= y
//Ret: Data expression for subtraction 'x - y', of sort Nat

ATermAppl gsMakeDataExprGTESubtB(ATermAppl DataExprBit, ATermAppl DataExprLHS,
  ATermAppl DataExprRHS);
//Pre: DataExprBit, DataExprLHS and DataExprRHS are data expressions of sort
//     Bool, Pos and Pos, respectively, which we denote by b, p and q.
//     p >= q + |b|
//Ret: Data expression for 'p - (q + |b|)', i.e. subtraction with borrow, of
//     sort Nat

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
//Pre: DataExprLHS is a data expression of sort Pos, Nat or Int, which we
//     denote by x
//     DataExprRHS is a data expression of sort Pos, which we denote by p
//Ret: Data expression for x div p of sort:
//     - Nat, if x is of sort Pos or Nat
//     - Int, if x is of sort Int

ATermAppl gsMakeDataExprMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS is a data expression of sort Pos, Nat or Int, which we
//     denote by x
//     DataExprRHS is a data expression of sort Pos, which we denote by p
//Ret: Data expression for x mod p of sort Nat

ATermAppl gsMakeDataExprDivMod(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of sort Pos
//Ret: Data expression for quotient and remainder after division,
//     of sort NatPair

ATermAppl gsMakeDataExprGDivMod(ATermAppl DataExprPair, ATermAppl DataExprBool,
  ATermAppl DataExprPos);
//Pre: DataExprPair, DataExprBool and DataExprPos are data
//     expressions of sort NatPair, Bool and Pos, respectively
//Ret: Data expression for the generalised quotient and remainder
//     after division, of sort NatPair

ATermAppl gsMakeDataExprGGDivMod(ATermAppl DataExprNat1, ATermAppl DataExprNat2,
  ATermAppl DataExprPos);
//Pre: DataExprNat1, DataExprNat2 and DataExprPos are data
//     expressions of sort Nat, Nat and Pos, respectively
//Ret: Data expression for the generalised generalised quotient and remainder
//     after division, of sort NatPair

ATermAppl gsMakeDataExprExp(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS is a data expression of sort Nat or Int
//     DataExprRHS is a data expression of sort Pos
//Ret: Data expression for the exponentiation of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprFirst(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort NatPair
//Ret: Data expression for 'first', of sort Nat

ATermAppl gsMakeDataExprLast(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort NatPair
//Ret: Data expression for 'last', of sort Nat

ATermAppl gsMakeDataExprEven(ATermAppl DataExpr);
//Pre: DataExpr is a data expression of sort Nat, which we denote by n
//Ret: Data expression for 'even(n)', of sort Bool

ATermAppl gsMakeDataExprEmptyList(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Data expression for the empty list of sort SortExpr

ATermAppl gsMakeDataExprCons(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for the list cons of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprListSize(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for the list size of DataExpr

ATermAppl gsMakeDataExprSnoc(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for the list snoc of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprConcat(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExpr is a sort expression
//Ret: Data expression for the list concatenation of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprEltAt(ATermAppl DataExprLHS, ATermAppl DataExprRHS,
  ATermAppl SortExpr);
//Pre: DataExprLHS and DataExprRHS are data expressions, of which the latter is
//     of sort Nat
//     SortExpr is a sort expression
//Ret: Data expression for the 'element at position' of DataExprLHS and
//     DataExprRHS with result sort SortExpr
     
ATermAppl gsMakeDataExprHead(ATermAppl DataExpr, ATermAppl SortExpr);
//Pre: DataExpr is a data expression and SortExpr is a sort expression
//Ret: Data expression for the head of DataExpr of result sort SortExpr

ATermAppl gsMakeDataExprTail(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for the tail of DataExpr

ATermAppl gsMakeDataExprRHead(ATermAppl DataExpr, ATermAppl SortExpr);
//Pre: DataExpr is a data expression and SortExpr is a sort expression
//Ret: Data expression for the right head of DataExpr of result sort SortExpr

ATermAppl gsMakeDataExprRTail(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for the right tail of DataExpr

ATermAppl gsMakeDataExprEltIn(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for element test "e in e'", where e = DataExprLHS and
//     e' = DataExprRHS

ATermAppl gsMakeDataExprSetComp(ATermAppl DataExpr, ATermAppl SortExpr);
//Pre: DataExpr is a data expression of sort S -> Bool
//     SortExprResult is a sort expression
//Ret: Set comprehension for sort S with result sort SortExprResult

ATermAppl gsMakeDataExprEmptySet(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Data expression for the empty set of sort SortExpr

ATermAppl gsMakeDataExprSubSetEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the subset or equality relation "e <= e'", where
//     e = DataExprLHS and e' = DataExprRHS

ATermAppl gsMakeDataExprSubSet(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the proper subset relation "e < e'", where
//     e = DataExprLHS and e' = DataExprRHS

ATermAppl gsMakeDataExprSetUnion(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the set union of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprSetDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the set difference of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprSetInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the set intersection of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprSetCompl(ATermAppl DataExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for the set complement of DataExpr

ATermAppl gsMakeDataExprBagComp(ATermAppl DataExpr, ATermAppl SortExprResult);
//Pre: DataExpr is a data expression of sort S -> Nat
//     SortExprResult is a sort expression
//Ret: Bag comprehension for sort S with result sort SortExprResult

ATermAppl gsMakeDataExprEmptyBag(ATermAppl SortExpr);
//Pre: SortExpr is a sort expression
//Ret: Data expression for the empty set of sort SortExpr

ATermAppl gsMakeDataExprCount(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions
//Ret: Data expression for the count of element DataExprLHS in bag DataExprRHS

ATermAppl gsMakeDataExprSubBagEq(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the subbag or equality relation "e <= e'", where
//     e = DataExprLHS and e' = DataExprRHS

ATermAppl gsMakeDataExprSubBag(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the proper subbag relation "e < e'", where
//     e = DataExprLHS and e' = DataExprRHS

ATermAppl gsMakeDataExprBagUnion(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the bag union of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprBagDiff(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the bag difference of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprBagInterSect(ATermAppl DataExprLHS, ATermAppl DataExprRHS);
//Pre: DataExprLHS and DataExprRHS are data expressions of the same sort
//Ret: Data expression for the bag intersection of DataExprLHS and DataExprRHS

ATermAppl gsMakeDataExprBag2Set(ATermAppl DataExpr, ATermAppl SortExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for Bag2Set(DataExpr) of sort SortExpr

ATermAppl gsMakeDataExprSet2Bag(ATermAppl DataExpr, ATermAppl SortExpr);
//Pre: DataExpr is a data expression
//Ret: Data expression for Set2Bag(DataExpr) of sort SortExpr

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


//Multiactions
//------------

ATermAppl gsSortMultAct(ATermAppl MultAct);
//Pre: MultAct is a multiaction
//Ret: the sorted variant of the argument


//Process expressions
//-------------------

bool gsIsProcExpr(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a process expression


//Mu-calculus formulas
//--------------------

bool gsIsStateFrm(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a state formula

bool gsIsRegFrm(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a regular formula

bool gsIsActFrm(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a action formula


//PBES's
//------

bool gsIsPBExpr(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a Parameterised Boolean Expression

bool gsIsFixpoint(ATermAppl Term);
//Pre: Term is not NULL
//Ret: Term is a fixpoint

#ifdef __cplusplus
}
#endif

#endif // MCRL_LIBSTRUCT_H
