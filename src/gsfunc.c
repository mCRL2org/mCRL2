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

ATermAppl gsMakeSpec(ATermList SpecElts)
{
  return ATmakeAppl1(ATmakeAFun("Spec", 1, ATfalse), (ATerm) SpecElts);
}

ATermAppl gsMakeSortSpec(ATermList SortDecls)
{
  return ATmakeAppl1(ATmakeAFun("SortSpec", 1, ATfalse), (ATerm) SortDecls);
}

ATermAppl gsMakeConsSpec(ATermList OpDecls)
{
  return ATmakeAppl1(ATmakeAFun("ConsSpec", 1, ATfalse), (ATerm) OpDecls);
}

ATermAppl gsMakeMapSpec(ATermList OpDecls)
{
  return ATmakeAppl1(ATmakeAFun("MapSpec", 1, ATfalse), (ATerm) OpDecls);
}

ATermAppl gsMakeEqnSpec(ATermAppl EqnVars, ATermAppl EqnDecls)
{
  return ATmakeAppl2(ATmakeAFun("EqnSpec", 2, ATfalse), (ATerm) EqnVars, (ATerm) EqnDecls);
}

ATermAppl gsMakeActSpec(ATermList ActDecls)
{
  return ATmakeAppl1(ATmakeAFun("ActSpec", 1, ATfalse), (ATerm) ActDecls);
}

ATermAppl gsMakeProcSpec(ATermList ProcDecls)
{
  return ATmakeAppl1(ATmakeAFun("ProcSpec", 1, ATfalse), (ATerm) ProcDecls);
}

ATermAppl gsMakeInit(ATermAppl ProcExpr)
{
  return ATmakeAppl1(ATmakeAFun("Init", 1, ATfalse), (ATerm) ProcExpr);
}

ATermAppl gsMakeStandSortDecl(ATermList Ids)
{
  return ATmakeAppl1(ATmakeAFun("StandSortDecl", 1, ATfalse), (ATerm) Ids);
}

ATermAppl gsMakeSortRefDecl(ATermList Ids, ATermAppl SortExpr)
{
  return ATmakeAppl2(ATmakeAFun("SortRefDecl", 2, ATfalse), (ATerm) Ids, (ATerm) SortExpr);
}

ATermAppl gsMakeStructSortDecl(ATermAppl Id, ATermList ConstrDecls)
{
  return ATmakeAppl2(ATmakeAFun("StructSortDecl", 2, ATfalse), (ATerm) Ids, (ATerm) ConstrDecls);
}






//--------------------

ATermAppl gsMakeName(char *name)
{
  return ATmakeAppl0(ATmakeAFun(name, 0, ATtrue));
}

ATermAppl gsMakeVar(ATermAppl vName, ATermAppl vSort)
{
  return ATmakeAppl2(ATmakeAFun("v", 2, ATfalse), (ATerm) vName, (ATerm) vSort);
}

ATermAppl gsMakeAct(ATermAppl action)
{
  return ATmakeAppl1(ATmakeAFun("act", 1, ATfalse), (ATerm) action);
}

ATermAppl gsMakeT(void)
{
  return ATmakeAppl0(ATmakeAFun("T", 0, ATfalse));
}

ATermAppl gsMakeF(void)
{
  return ATmakeAppl0(ATmakeAFun("F", 0, ATfalse));
}

ATermAppl gsMakeNot(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("not", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeAnd(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("and", 2, ATfalse),
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeOr(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("or", 2, ATfalse), 
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeImp(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("imp", 2, ATfalse), 
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeEq(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("eq", 2, ATfalse), 
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeForall(ATermAppl v_name, ATermAppl v_sort, ATermAppl form)
{
  return ATmakeAppl3(ATmakeAFun("forall", 3, ATfalse), 
    (ATerm) v_name, (ATerm) v_sort, (ATerm) form);
}

ATermAppl gsMakeExists(ATermAppl v_name, ATermAppl v_sort, ATermAppl form)
{
  return ATmakeAppl3(ATmakeAFun("exists", 3, ATfalse), 
    (ATerm) v_name, (ATerm) v_sort, (ATerm) form);
}

ATermAppl gsMakeNil(void)
{
  return ATmakeAppl0(ATmakeAFun("nil", 0, ATfalse));
}

ATermAppl gsMakeConcat(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("concat", 2, ATfalse),
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeChoice(ATermAppl form0, ATermAppl form1)
{
  return ATmakeAppl2(ATmakeAFun("choice", 2, ATfalse),
    (ATerm) form0, (ATerm) form1);
}

ATermAppl gsMakeTrClose(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("tr_close", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeTClose(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("t_close", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeFormRec(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("form_rec", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeForm(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("form", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeRec(ATermAppl fpVO)
{
  return ATmakeAppl1(ATmakeAFun("rec", 1, ATfalse), (ATerm) fpVO);
}

ATermAppl gsMakeMay(ATermAppl regFrm, ATermAppl modFrm) 
{
  return ATmakeAppl2(ATmakeAFun("may", 2, ATfalse),
    (ATerm) regFrm, (ATerm) modFrm);
}

ATermAppl gsMakeMust(ATermAppl regFrm, ATermAppl modFrm)
{
  return ATmakeAppl2(ATmakeAFun("must", 2, ATfalse), 
    (ATerm) regFrm, (ATerm) modFrm);
}

ATermAppl gsMakeLoop(ATermAppl form)
{
  return ATmakeAppl1(ATmakeAFun("loop", 1, ATfalse), (ATerm) form);
}

ATermAppl gsMakeMu(ATermAppl fpVar, ATermList vdList, ATermAppl modFrm, ATermList iList)
{
  return ATmakeAppl4(ATmakeAFun("mu", 4, ATfalse), 
    (ATerm) fpVar, (ATerm) vdList, (ATerm) modFrm, (ATerm) iList);
}

ATermAppl gsMakeNu(ATermAppl fpVar, ATermList vdList, ATermAppl modFrm, ATermList iList)
{
  return ATmakeAppl4(ATmakeAFun("nu", 4, ATfalse), 
    (ATerm) fpVar, (ATerm) vdList, (ATerm) modFrm, (ATerm) iList);
}

//strcmp extensions

bool gsIsVar(char *s)
{
  return strcmp(s, "v") == 0;
}

bool gsIsAct(char *s)
{
  return strcmp(s, "act") == 0;
}

bool gsIsT(char *s)
{
  return strcmp(s, "T") == 0;
}

bool gsIsF(char *s)
{
  return strcmp(s, "F") == 0;
}

bool gsIsNot(char *s)
{
  return strcmp(s, "not") == 0;
}

bool gsIsAnd(char *s)
{
  return strcmp(s, "and") == 0;
}

bool gsIsOr(char *s)
{
  return strcmp(s, "or") == 0;
}

bool gsIsImp(char *s)
{
  return strcmp(s, "imp") == 0;
}

bool gsIsEq(char *s)
{
  return strcmp(s, "eq") == 0;
}

bool gsIsForall(char *s)
{
  return strcmp(s, "forall") == 0;
}

bool gsIsExists(char *s)
{
  return strcmp(s, "exists") == 0;
}

bool gsIsNil(char *s)
{
  return strcmp(s, "nil") == 0;
}

bool gsIsConcat(char *s)
{
  return strcmp(s, "concat") == 0;
}

bool gsIsChoice(char *s)
{
  return strcmp(s, "choice") == 0;
}

bool gsIsTrClose(char *s)
{
  return strcmp(s, "tr_close") == 0;
}

bool gsIsTClose(char *s)
{
  return strcmp(s, "t_close") == 0;
}

bool gsIsFormRec(char *s)
{
  return strcmp(s, "form_rec") == 0;
}

bool gsIsForm(char *s)
{
  return strcmp(s, "form") == 0;
}

bool gsIsRec(char *s)
{
  return strcmp(s, "rec") == 0;
}

bool gsIsMay(char *s)
{
  return strcmp(s, "may") == 0;
}

bool gsIsMust(char *s)
{
  return strcmp(s, "must") == 0;
}

bool gsIsLoop(char *s)
{
  return strcmp(s, "loop") == 0;
}

bool gsIsMu(char *s)
{
  return strcmp(s, "mu") == 0;
}

bool gsIsNu(char *s)
{
  return strcmp(s, "nu") == 0;
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
