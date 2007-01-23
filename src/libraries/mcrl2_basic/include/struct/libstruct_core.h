#ifndef MCRL_LIBSTRUCT_CORE_H
#define MCRL_LIBSTRUCT_CORE_H

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

bool gsCoreConstructorFunctionsEnabled(void);
//Ret:  the constructor creator and recogniser functions are enabled

//gsEnableConstrucorFunctions() should be called before calling any of the
//remaining functions in this file.
void gsEnableCoreConstructorFunctions(void);
//Post: the constructor creator and recogniser functions are enabled

//Creation of all constructor elements of the internal structure
//--------------------------------------------------------------

//sort expressions
ATermAppl gsMakeSortCons(ATermAppl SortConsType, ATermAppl SortExpr);
ATermAppl gsMakeSortStruct(ATermList StructConss);
ATermAppl gsMakeSortArrowProd(ATermList SortExprsDomain,
  ATermAppl SortExprResult);
ATermAppl gsMakeSortArrow(ATermAppl SortExprDomain, ATermAppl SortExprResult);
ATermAppl gsMakeSortId(ATermAppl Name);
ATermAppl gsMakeSortList();
ATermAppl gsMakeSortSet();
ATermAppl gsMakeSortBag();
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
//data specifications
ATermAppl gsMakeDataSpec(ATermAppl SortSpec, ATermAppl ConsSpec,
            ATermAppl MapSpec, ATermAppl DataEqnSpec);
ATermAppl gsMakeSortSpec(ATermList SortDecls);
ATermAppl gsMakeConsSpec(ATermList OpIds);
ATermAppl gsMakeMapSpec(ATermList OpIds);
ATermAppl gsMakeDataEqnSpec(ATermList DataEqns);
ATermAppl gsMakeSortRef(ATermAppl Name, ATermAppl SortExpr);
ATermAppl gsMakeDataEqn(ATermList DataVarIds, ATermAppl BoolExprOrNil,
  ATermAppl DataExprLHS, ATermAppl DataExprRHS);
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
ATermAppl gsMakeIfThen(ATermAppl BoolExprIf, ATermAppl ProcExprThen);
ATermAppl gsMakeIfThenElse(ATermAppl BoolExprIf, ATermAppl ProcExprThen,
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
ATermAppl gsMakeSpecV1(ATermAppl DataSpec, ATermAppl ActSpec,
            ATermAppl ProcEqnSpec, ATermAppl Init);
ATermAppl gsMakeActSpec(ATermList ActIds);
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
ATermAppl gsMakePropVarDecl(ATermAppl Name, ATermList DataVarIds);
ATermAppl gsMakeMu();
ATermAppl gsMakeNu();
ATermAppl gsMakePBESTrue();
ATermAppl gsMakePBESFalse();
ATermAppl gsMakePBESAnd(ATermAppl PBExprLHS, ATermAppl PBExprRHS);
ATermAppl gsMakePBESOr(ATermAppl PBExprLHS, ATermAppl PBExprRHS);
ATermAppl gsMakePBESForall(ATermList DataVarId, ATermAppl PBExpr);
ATermAppl gsMakePBESExists(ATermList DataVarId, ATermAppl PBExpr);

//Recognisers of all constructor elements of the internal structure
//-----------------------------------------------------------------

//sort expressions
bool gsIsSortCons(ATermAppl Term);
bool gsIsSortStruct(ATermAppl Term);
bool gsIsSortArrowProd(ATermAppl Term);
bool gsIsSortArrow(ATermAppl Term);
bool gsIsSortId(ATermAppl Term);
bool gsIsSortList(ATermAppl Term);
bool gsIsSortSet(ATermAppl Term);
bool gsIsSortBag(ATermAppl Term);
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
//data specifications
bool gsIsDataSpec(ATermAppl Term);
bool gsIsSortSpec(ATermAppl Term);
bool gsIsConsSpec(ATermAppl Term);
bool gsIsMapSpec(ATermAppl Term);
bool gsIsDataEqnSpec(ATermAppl Term);
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
bool gsIsIfThen(ATermAppl Term);
bool gsIsIfThenElse(ATermAppl Term);
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
bool gsIsPropVarDecl(ATermAppl Term);
bool gsIsMu(ATermAppl Term);
bool gsIsNu(ATermAppl Term);
bool gsIsPBESTrue(ATermAppl Term);
bool gsIsPBESFalse(ATermAppl Term);
bool gsIsPBESAnd(ATermAppl Term);
bool gsIsPBESOr(ATermAppl Term);
bool gsIsPBESForall(ATermAppl Term);
bool gsIsPBESExists(ATermAppl Term);

#ifdef __cplusplus
}
#endif

#endif // MCRL_LIBSTRUCT_CORE_H
