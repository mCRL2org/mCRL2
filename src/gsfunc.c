#include "gsfunc.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

//strdup implementation
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

//ATmake extensions

ATermAppl gsMakeId(char *s)
{
  return ATmakeAppl1(ATmakeAFun("Id", 1, ATfalse),
    (ATerm) ATmakeAppl0(ATmakeAFun(s, 0, ATfalse)));
}

ATermAppl gsMakeNumber(char *s)
{
  return ATmakeAppl1(ATmakeAFun("Number", 1, ATfalse),
    (ATerm) ATmakeAppl0(ATmakeAFun(s, 0, ATfalse)));
}

ATermAppl gsMakeSpec(ATermList SpecElts)
{
  return ATmakeAppl1(ATmakeAFun("Spec", 1, ATfalse), (ATerm) SpecElts);
}

ATermAppl gsMakeSortSpec(ATermList SortDecls)
{
  return ATmakeAppl1(ATmakeAFun("SortSpec", 1, ATfalse), (ATerm) SortDecls);
}

ATermAppl gsMakeSortDeclStandard(ATermList Ids)
{
  return ATmakeAppl1(ATmakeAFun("SortDeclStandard", 1, ATfalse), (ATerm) Ids);
}

ATermAppl gsMakeSortDeclRef(ATermList Ids, ATermAppl SortExpr)
{
  return ATmakeAppl2(ATmakeAFun("SortDeclRef", 2, ATfalse), (ATerm) Ids,
    (ATerm) SortExpr);
}

ATermAppl gsMakeSortDeclStruct(ATermAppl Id, ATermList ConstrDecls)
{
  return ATmakeAppl2(ATmakeAFun("SortDeclStruct", 2, ATfalse), (ATerm) Id,
    (ATerm) ConstrDecls);
}

ATermAppl gsMakeStructDeclCons(ATermAppl ConsId, ATermList ProjDecls,
  ATermAppl NilOrId)
{
  return ATmakeAppl3(ATmakeAFun("StructDeclCons", 3, ATfalse), (ATerm) ConsId,
    (ATerm) ProjDecls, (ATerm) NilOrId);
}

ATermAppl gsMakeNil()
{
  return ATmakeAppl0(ATmakeAFun("Nil", 0, ATfalse));
}

ATermAppl gsMakeStructDeclProj(ATermAppl NilOrId, ATermAppl Domain)
{
  return ATmakeAppl2(ATmakeAFun("StructDeclProj", 2, ATfalse), (ATerm) NilOrId,
    (ATerm) Domain);
}

ATermAppl gsMakeDomain(ATermList SortExprs)
{
  return ATmakeAppl1(ATmakeAFun("Dom", 1, ATfalse), (ATerm) SortExprs);
}

ATermAppl gsMakeIds(ATermList Ids)
{
  return ATmakeAppl1(ATmakeAFun("Ids", 1, ATfalse), (ATerm) Ids);
}

ATermAppl gsMakeConsSpec(ATermList OpDecls)
{
  return ATmakeAppl1(ATmakeAFun("ConsSpec", 1, ATfalse), (ATerm) OpDecls);
}

ATermAppl gsMakeMapSpec(ATermList OpDecls)
{
  return ATmakeAppl1(ATmakeAFun("MapSpec", 1, ATfalse), (ATerm) OpDecls);
}

ATermAppl gsMakeIdsDecl(ATermList Ids, ATermAppl SortExpr)
{
  return ATmakeAppl2(ATmakeAFun("IdsDecl", 2, ATfalse), (ATerm) Ids,
    (ATerm) SortExpr);
}

ATermAppl gsMakeIdDecl(ATermAppl Id, ATermAppl SortExpr)
{
  return ATmakeAppl2(ATmakeAFun("IdDecl", 2, ATfalse), (ATerm) Id,
    (ATerm) SortExpr);
}

ATermAppl gsMakeEqnSpec(ATermList EqnVars, ATermList EqnDecls)
{
  return ATmakeAppl2(ATmakeAFun("EqnSpec", 2, ATfalse), (ATerm) EqnVars,
    (ATerm) EqnDecls);
}

ATermAppl gsMakeEqnDecl(ATermAppl DataExprLHS, ATermAppl DataExprRHS)
{
  return ATmakeAppl2(ATmakeAFun("EqnDecl", 2, ATfalse), (ATerm) DataExprLHS,
    (ATerm) DataExprRHS);
}

ATermAppl gsMakeActSpec(ATermList ActDecls)
{
  return ATmakeAppl1(ATmakeAFun("ActSpec", 1, ATfalse), (ATerm) ActDecls);
}

ATermAppl gsMakeActDecl(ATermList Ids, ATermAppl ActDomain)
{
  return ATmakeAppl2(ATmakeAFun("ActDecl", 2, ATfalse), (ATerm) Ids,
    (ATerm) ActDomain);
}

ATermAppl gsMakeProcSpec(ATermList ProcDecls)
{
  return ATmakeAppl1(ATmakeAFun("ProcSpec", 1, ATfalse), (ATerm) ProcDecls);
}

ATermAppl gsMakeProcDecl(ATermAppl Id, ATermList ProcVars, ATermAppl ProcExpr)
{
  return ATmakeAppl3(ATmakeAFun("ProcDecl", 3, ATfalse), (ATerm) Id,
    (ATerm) ProcVars, (ATerm) ProcExpr);
}

ATermAppl gsMakeInit(ATermAppl ProcExpr)
{
  return ATmakeAppl1(ATmakeAFun("Init", 1, ATfalse), (ATerm) ProcExpr);
}

ATermAppl gsMakeSortBool()
{
  return ATmakeAppl0(ATmakeAFun("SortBool", 0, ATfalse));
}

ATermAppl gsMakeSortPos()
{
  return ATmakeAppl0(ATmakeAFun("SortPos", 0, ATfalse));
}

ATermAppl gsMakeSortNat()
{
  return ATmakeAppl0(ATmakeAFun("SortNat", 0, ATfalse));
}

ATermAppl gsMakeSortInt()
{
  return ATmakeAppl0(ATmakeAFun("SortInt", 0, ATfalse));
}

ATermAppl gsMakeSortList(ATermAppl SortExpr)
{
  return ATmakeAppl1(ATmakeAFun("SortList", 1, ATfalse), (ATerm) SortExpr);
}

ATermAppl gsMakeSortSet(ATermAppl SortExpr)
{
  return ATmakeAppl1(ATmakeAFun("SortSet", 1, ATfalse), (ATerm) SortExpr);
}

ATermAppl gsMakeSortBag(ATermAppl SortExpr)
{
  return ATmakeAppl1(ATmakeAFun("SortBag", 1, ATfalse), (ATerm) SortExpr);
}

ATermAppl gsMakeSortRef(ATermAppl Id)
{
  return ATmakeAppl1(ATmakeAFun("SortRef", 1, ATfalse), (ATerm) Id);
}

ATermAppl gsMakeSortArrow(ATermAppl Domain, ATermAppl SortExpr)
{
  return ATmakeAppl2(ATmakeAFun("SortArrow", 2, ATfalse), (ATerm) Domain,
    (ATerm) SortExpr);
}

ATermAppl gsMakeTrue()
{
  return ATmakeAppl0(ATmakeAFun("True", 0, ATfalse));
}

ATermAppl gsMakeFalse()
{
  return ATmakeAppl0(ATmakeAFun("False", 0, ATfalse));
}

ATermAppl gsMakeEmptyList()
{
  return ATmakeAppl0(ATmakeAFun("EmptyList", 0, ATfalse));
}

ATermAppl gsMakeEmptySetBag()
{
  return ATmakeAppl0(ATmakeAFun("EmptySetBag", 0, ATfalse));
}

ATermAppl gsMakeListEnum(ATermList DataExprs)
{
  return ATmakeAppl1(ATmakeAFun("ListEnum", 1, ATfalse), (ATerm) DataExprs);
}

ATermAppl gsMakeSetEnum(ATermList DataExprs)
{
  return ATmakeAppl1(ATmakeAFun("SetEnum", 1, ATfalse), (ATerm) DataExprs);
}

ATermAppl gsMakeBagEnum(ATermList BagEnumElts)
{
  return ATmakeAppl1(ATmakeAFun("BagEnum", 1, ATfalse), (ATerm) BagEnumElts);
}

ATermAppl gsMakeSetBagComp(ATermAppl IdDecl, ATermAppl DataExpr)
{
  return ATmakeAppl2(ATmakeAFun("SetBagComp", 2, ATfalse), (ATerm) IdDecl,
    (ATerm) DataExpr);
}

ATermAppl gsMakeFuncApp(ATermAppl DataExpr, ATermList FuncArgs)
{
  return ATmakeAppl2(ATmakeAFun("FuncApp", 2, ATfalse), (ATerm) DataExpr,
    (ATerm) FuncArgs);
}

ATermAppl gsMakeNotOrCompl(ATermAppl DataExpr)
{
  return ATmakeAppl1(ATmakeAFun("NotOrCompl", 1, ATfalse), (ATerm) DataExpr);
}

ATermAppl gsMakeNeg(ATermAppl DataExpr)
{
  return ATmakeAppl1(ATmakeAFun("Neg", 1, ATfalse), (ATerm) DataExpr);
}

ATermAppl gsMakeSize(ATermAppl DataExpr)
{
  return ATmakeAppl1(ATmakeAFun("Size", 1, ATfalse), (ATerm) DataExpr);
}

ATermAppl gsMakeForall(ATermAppl IdDecl, ATermAppl DataExpr)
{
  return ATmakeAppl2(ATmakeAFun("Forall", 2, ATfalse), (ATerm) IdDecl,
    (ATerm) DataExpr);
}

ATermAppl gsMakeExists(ATermAppl IdDecl, ATermAppl DataExpr)
{
  return ATmakeAppl2(ATmakeAFun("Exists", 2, ATfalse), (ATerm) IdDecl,
    (ATerm) DataExpr);
}

ATermAppl gsMakeListAt(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("ListAt", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeMultOrIntersect(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("MultOrIntersect", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeDiv(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Div", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeMod(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Mod", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeAddOrUnion(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("AddOrUnion", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeSubtOrDiff(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("SubtOrDiff", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeLTOrPropSubset(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("LTOrPropSubset", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeGT(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("GT", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeLTEOrSubset(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("LTEOrSubset", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeGTE(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("GTE", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeIn(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("In", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeCons(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Cons", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeSnoc(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Snoc", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeConcat(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Concat", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeEq(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Eq", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeNeq(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Neq", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeAnd(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("And", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeOr(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Or", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeImp(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Imp", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeLambda(ATermAppl IdDecl, ATermAppl DataExpr)
{
  return ATmakeAppl2(ATmakeAFun("Lambda", 2, ATfalse), (ATerm) IdDecl,
    (ATerm) DataExpr);
}

ATermAppl gsMakeWhr(ATermAppl DataExpr, ATermList DataExprs)
{
  return ATmakeAppl2(ATmakeAFun("Whr", 2, ATfalse), (ATerm) DataExpr,
    (ATerm) DataExprs);
}

ATermAppl gsMakeBagEnumElt(ATermAppl DataExpr, ATermAppl Multiplicity)
{
  return ATmakeAppl2(ATmakeAFun("BagEnumElt", 2, ATfalse), (ATerm) DataExpr,
    (ATerm) Multiplicity);
}

ATermAppl gsMakeActProcRef(ATermAppl Id, ATermList ActProcArgs)
{
  return ATmakeAppl2(ATmakeAFun("ActProcRef", 2, ATfalse), (ATerm) Id,
    (ATerm) ActProcArgs);
}

ATermAppl gsMakeDelta()
{
  return ATmakeAppl0(ATmakeAFun("Delta", 0, ATfalse));
}

ATermAppl gsMakeTau()
{
  return ATmakeAppl0(ATmakeAFun("Tau", 0, ATfalse));
}

ATermAppl gsMakeSum(ATermAppl IdDecl, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Sum", 2, ATfalse), (ATerm) IdDecl,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeRestrict(ATermList MAIdSet, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Restrict", 2, ATfalse), (ATerm) MAIdSet,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeAllow(ATermList MAIdSet, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Allow", 2, ATfalse), (ATerm) MAIdSet,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeHide(ATermList MAIdSet, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Hide", 2, ATfalse), (ATerm) MAIdSet,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeRename(ATermList RenExprSet, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Rename", 2, ATfalse), (ATerm) RenExprSet,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeComm(ATermList CommExprSet, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Comm", 2, ATfalse), (ATerm) CommExprSet,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeSync(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Sync", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeAtTime(ATermAppl ProcExpr, ATermAppl DataExpr)
{
  return ATmakeAppl2(ATmakeAFun("AtTime", 2, ATfalse), (ATerm) ProcExpr,
    (ATerm) DataExpr);
}

ATermAppl gsMakeSeq(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Seq", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeCond(ATermAppl DataExpr, ATermAppl ProcExpr)
{
  return ATmakeAppl2(ATmakeAFun("Cond", 2, ATfalse), (ATerm) DataExpr,
    (ATerm) ProcExpr);
}

ATermAppl gsMakeBInit(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("BInit", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeMerge(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Merge", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeLMerge(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("LMerge", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeChoice(ATermAppl OpLeft, ATermAppl OpRight)
{
  return ATmakeAppl2(ATmakeAFun("Choice", 2, ATfalse), (ATerm) OpLeft,
    (ATerm) OpRight);
}

ATermAppl gsMakeMAId(ATermList Ids)
{
  return ATmakeAppl1(ATmakeAFun("MAId", 1, ATfalse), (ATerm) Ids);
}

ATermAppl gsMakeMAIdSet(ATermList MAIds)
{
  return ATmakeAppl1(ATmakeAFun("MAIdSet", 1, ATfalse), (ATerm) MAIds);
}

ATermAppl gsMakeCommExpr(ATermAppl MAId, ATermAppl TauOrId)
{
  return ATmakeAppl2(ATmakeAFun("CommExpr", 2, ATfalse), (ATerm) MAId,
    (ATerm) TauOrId);
}

ATermAppl gsMakeCommExprSet(ATermList CommExprs)
{
  return ATmakeAppl1(ATmakeAFun("CommExprSet", 1, ATfalse), (ATerm) CommExprs);
}

ATermAppl gsMakeRenExpr(ATermAppl FromId, ATermAppl ToId)
{
  return ATmakeAppl2(ATmakeAFun("RenExpr", 2, ATfalse), (ATerm) FromId,
    (ATerm) ToId);
}

ATermAppl gsMakeRenExprSet(ATermList RenExprs)
{
  return ATmakeAppl1(ATmakeAFun("RenExprSet", 1, ATfalse), (ATerm) RenExprs);
}

//strcmp extensions

bool gsIsId(char *s)
{
  return strcmp(s, "Id") == 0;
}

bool gsIsNumber(char *s)
{
  return strcmp(s, "Number") == 0;
}

bool gsIsSpec(char *s)
{
  return strcmp(s, "Spec") == 0;
}

bool gsIsSortSpec(char *s)
{
  return strcmp(s, "SortSpec") == 0;
}

bool gsIsSortDeclStandard(char *s)
{
  return strcmp(s, "SortDeclStandard") == 0;
}

bool gsIsSortDeclRef(char *s)
{
  return strcmp(s, "SortDeclRef") == 0;
}

bool gsIsSortDeclStruct(char *s)
{
  return strcmp(s, "SortDeclStruct") == 0;
}

bool gsIsStructDeclCons(char *s)
{
  return strcmp(s, "StructDeclCons") == 0;
}

bool gsIsNil(char *s)
{
  return strcmp(s, "Nil") == 0;
}

bool gsIsStructDeclProj(char *s)
{
  return strcmp(s, "StructDeclProj") == 0;
}

bool gsIsDomain(char *s)
{
  return strcmp(s, "Dom") == 0;
}

bool gsIsIds(char *s)
{
  return strcmp(s, "Ids") == 0;
}

bool gsIsConsSpec(char *s)
{
  return strcmp(s, "ConsSpec") == 0;
}

bool gsIsMapSpec(char *s)
{
  return strcmp(s, "MapSpec") == 0;
}

bool gsIsIdsDecl(char *s)
{
  return strcmp(s, "IdsDecl") == 0;
}

bool gsIsIdDecl(char *s)
{
  return strcmp(s, "IdDecl") == 0;
}

bool gsIsEqnSpec(char *s)
{
  return strcmp(s, "EqnSpec") == 0;
}

bool gsIsEqnDecl(char *s)
{
  return strcmp(s, "EqnDecl") == 0;
}

bool gsIsActSpec(char *s)
{
  return strcmp(s, "ActSpec") == 0;
}

bool gsIsActDecl(char *s)
{
  return strcmp(s, "ActDecl") == 0;
}

bool gsIsProcSpec(char *s)
{
  return strcmp(s, "ProcSpec") == 0;
}

bool gsIsProcDecl(char *s)
{
  return strcmp(s, "ProcDecl") == 0;
}

bool gsIsInit(char *s)
{
  return strcmp(s, "Init") == 0;
}

bool gsIsSortBool(char *s)
{
  return strcmp(s, "SortBool") == 0;
}

bool gsIsSortPos(char *s)
{
  return strcmp(s, "SortPos") == 0;
}

bool gsIsSortNat(char *s)
{
  return strcmp(s, "SortNat") == 0;
}

bool gsIsSortInt(char *s)
{
  return strcmp(s, "SortInt") == 0;
}

bool gsIsSortList(char *s)
{
  return strcmp(s, "SortList") == 0;
}

bool gsIsSortSet(char *s)
{
  return strcmp(s, "SortSet") == 0;
}

bool gsIsSortBag(char *s)
{
  return strcmp(s, "SortBag") == 0;
}

bool gsIsSortRef(char *s)
{
  return strcmp(s, "SortRef") == 0;
}

bool gsIsSortArrow(char *s)
{
  return strcmp(s, "SortArrow") == 0;
}

bool gsIsTrue(char *s)
{
  return strcmp(s, "True") == 0;
}

bool gsIsFalse(char *s)
{
  return strcmp(s, "False") == 0;
}

bool gsIsEmptyList(char *s)
{
  return strcmp(s, "EmptyList") == 0;
}

bool gsIsEmptySetBag(char *s)
{
  return strcmp(s, "EmptySetBag") == 0;
}

bool gsIsListEnum(char *s)
{
  return strcmp(s, "ListEnum") == 0;
}

bool gsIsSetEnum(char *s)
{
  return strcmp(s, "SetEnum") == 0;
}

bool gsIsBagEnum(char *s)
{
  return strcmp(s, "BagEnum") == 0;
}

bool gsIsSetBagComp(char *s)
{
  return strcmp(s, "SetBagComp") == 0;
}

bool gsIsFuncApp(char *s)
{
  return strcmp(s, "FuncApp") == 0;
}

bool gsIsNotOrCompl(char *s)
{
  return strcmp(s, "NotOrCompl") == 0;
}

bool gsIsNeg(char *s)
{
  return strcmp(s, "Neg") == 0;
}

bool gsIsSize(char *s)
{
  return strcmp(s, "Size") == 0;
}

bool gsIsForall(char *s)
{
  return strcmp(s, "Forall") == 0;
}

bool gsIsExists(char *s)
{
  return strcmp(s, "Exists") == 0;
}

bool gsIsListAt(char *s)
{
  return strcmp(s, "ListAt") == 0;
}

bool gsIsMultOrIntersect(char *s)
{
  return strcmp(s, "MultOrIntersect") == 0;
}

bool gsIsDiv(char *s)
{
  return strcmp(s, "Div") == 0;
}

bool gsIsMod(char *s)
{
  return strcmp(s, "Mod") == 0;
}

bool gsIsAddOrUnion(char *s)
{
  return strcmp(s, "AddOrUnion") == 0;
}

bool gsIsSubtOrDiff(char *s)
{
  return strcmp(s, "SubtOrDiff") == 0;
}

bool gsIsLTOrPropSubset(char *s)
{
  return strcmp(s, "LTOrPropSubset") == 0;
}

bool gsIsGT(char *s)
{
  return strcmp(s, "GT") == 0;
}

bool gsIsLTEOrSubset(char *s)
{
  return strcmp(s, "LTEOrSubset") == 0;
}

bool gsIsGTE(char *s)
{
  return strcmp(s, "GTE") == 0;
}

bool gsIsIn(char *s)
{
  return strcmp(s, "In") == 0;
}

bool gsIsCons(char *s)
{
  return strcmp(s, "Cons") == 0;
}

bool gsIsSnoc(char *s)
{
  return strcmp(s, "Snoc") == 0;
}

bool gsIsConcat(char *s)
{
  return strcmp(s, "Concat") == 0;
}

bool gsIsEq(char *s)
{
  return strcmp(s, "Eq") == 0;
}

bool gsIsNeq(char *s)
{
  return strcmp(s, "Neq") == 0;
}

bool gsIsAnd(char *s)
{
  return strcmp(s, "And") == 0;
}

bool gsIsOr(char *s)
{
  return strcmp(s, "Or") == 0;
}

bool gsIsImp(char *s)
{
  return strcmp(s, "Imp") == 0;
}

bool gsIsLambda(char *s)
{
  return strcmp(s, "Lambda") == 0;
}

bool gsIsWhr(char *s)
{
  return strcmp(s, "Whr") == 0;
}

bool gsIsBagEnumElt(char *s)
{
  return strcmp(s, "BagEnumElt") == 0;
}

bool gsIsActProcRef(char *s)
{
  return strcmp(s, "ActProcRef") == 0;
}

bool gsIsDelta(char *s)
{
  return strcmp(s, "Delta") == 0;
}

bool gsIsTau(char *s)
{
  return strcmp(s, "Tau") == 0;
}

bool gsIsSum(char *s)
{
  return strcmp(s, "Sum") == 0;
}

bool gsIsRestrict(char *s)
{
  return strcmp(s, "Restrict") == 0;
}

bool gsIsAllow(char *s)
{
  return strcmp(s, "Allow") == 0;
}

bool gsIsHide(char *s)
{
  return strcmp(s, "Hide") == 0;
}

bool gsIsRename(char *s)
{
  return strcmp(s, "Rename") == 0;
}

bool gsIsComm(char *s)
{
  return strcmp(s, "Comm") == 0;
}

bool gsIsSync(char *s)
{
  return strcmp(s, "Sync") == 0;
}

bool gsIsAtTime(char *s)
{
  return strcmp(s, "AtTime") == 0;
}

bool gsIsSeq(char *s)
{
  return strcmp(s, "Seq") == 0;
}

bool gsIsCond(char *s)
{
  return strcmp(s, "Cond") == 0;
}

bool gsIsBInit(char *s)
{
  return strcmp(s, "BInit") == 0;
}

bool gsIsMerge(char *s)
{
  return strcmp(s, "Merge") == 0;
}

bool gsIsLMerge(char *s)
{
  return strcmp(s, "LMerge") == 0;
}

bool gsIsChoice(char *s)
{
  return strcmp(s, "Choice") == 0;
}

bool gsIsMAId(char *s)
{
  return strcmp(s, "MAId") == 0;
}

bool gsIsMAIdSet(char *s)
{
  return strcmp(s, "MAIdSet") == 0;
}

bool gsIsCommExpr(char *s)
{
  return strcmp(s, "CommExpr") == 0;
}

bool gsIsCommExprSet(char *s)
{
  return strcmp(s, "CommExprSet") == 0;
}

bool gsIsRenExpr(char *s)
{
  return strcmp(s, "RenExpr") == 0;
}

bool gsIsRenExprSet(char *s)
{
  return strcmp(s, "RenExprSet") == 0;
}

//ATerm libary work arounds

ATermAppl ATAelementAt(ATermList list, int index)
{
  return (ATermAppl) ATelementAt(list, index);
}

ATermList ATLelementAt(ATermList list, int index)
{
  return (ATermList) ATelementAt(list, index);
}

ATermInt ATIelementAt(ATermList list, int index)
{
  return (ATermInt) ATelementAt(list, index);
}

ATermAppl ATAgetArgument(ATermAppl appl, int nr)
{
  return (ATermAppl) ATgetArgument(appl, nr);
}

ATermList ATLgetArgument(ATermAppl appl, int nr)
{
  return (ATermList) ATgetArgument(appl, nr);
}

ATermInt ATIgetArgument(ATermAppl appl, int nr)
{
  return (ATermInt) ATgetArgument(appl, nr);
}
