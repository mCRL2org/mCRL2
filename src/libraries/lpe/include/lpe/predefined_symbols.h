#ifndef PREDEFINED_SYMBOLS_H
#define PREDEFINED_SYMBOLS_H

/** @file Contains predefined function symbols (for efficiency reasons).
  */

#include "atermpp/aterm.h"

namespace lpe {

using atermpp::function_symbol;

//--- begin symbols  
    inline
    function_symbol func_ActId()
    {
      static AFun f = ATmakeAFun("ActId", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeActId(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ActSpec()
    {
      static AFun f = ATmakeAFun("ActSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeActSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Action()
    {
      static AFun f = ATmakeAFun("Action", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeAction(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ParamId()
    {
      static AFun f = ATmakeAFun("ParamId", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeParamId(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Allow()
    {
      static AFun f = ATmakeAFun("Allow", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeAllow(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Assignment()
    {
      static AFun f = ATmakeAFun("Assignment", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeAssignment(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_AtTime()
    {
      static AFun f = ATmakeAFun("AtTime", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeAtTime(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_BInit()
    {
      static AFun f = ATmakeAFun("BInit", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeBInit(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_BagEnum()
    {
      static AFun f = ATmakeAFun("BagEnum", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeBagEnum(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_BagEnumElt()
    {
      static AFun f = ATmakeAFun("BagEnumElt", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeBagEnumElt(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Choice()
    {
      static AFun f = ATmakeAFun("Choice", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeChoice(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Comm()
    {
      static AFun f = ATmakeAFun("Comm", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeComm(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_CommExpr()
    {
      static AFun f = ATmakeAFun("CommExpr", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeCommExpr(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Cond()
    {
      static AFun f = ATmakeAFun("Cond", 3, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeCond(0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ConsSpec()
    {
      static AFun f = ATmakeAFun("ConsSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeConsSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_DataAppl()
    {
      static AFun f = ATmakeAFun("DataAppl", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDataAppl(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_DataApplProd()
    {
      static AFun f = ATmakeAFun("DataApplProd", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDataApplProd(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_DataEqn()
    {
      static AFun f = ATmakeAFun("DataEqn", 4, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDataEqn(0,0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_DataEqnSpec()
    {
      static AFun f = ATmakeAFun("DataEqnSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDataEqnSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_DataVarId()
    {
      static AFun f = ATmakeAFun("DataVarId", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDataVarId(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Id()
    {
      static AFun f = ATmakeAFun("Id", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeId(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Delta()
    {
      static AFun f = ATmakeAFun("Delta", 0, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeDelta();
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Exists()
    {
      static AFun f = ATmakeAFun("Exists", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeExists(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Forall()
    {
      static AFun f = ATmakeAFun("Forall", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeForall(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Hide()
    {
      static AFun f = ATmakeAFun("Hide", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeHide(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Init()
    {
      static AFun f = ATmakeAFun("Init", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeInit(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_LMerge()
    {
      static AFun f = ATmakeAFun("LMerge", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeLMerge(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_LPE()
    {
      static AFun f = ATmakeAFun("LPE", 3, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeLPE(0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_LPEInit()
    {
      static AFun f = ATmakeAFun("LPEInit", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeLPEInit(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_LPESummand()
    {
      static AFun f = ATmakeAFun("LPESummand", 5, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeLPESummand(0,0,0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Lambda()
    {
      static AFun f = ATmakeAFun("Lambda", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeLambda(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ListEnum()
    {
      static AFun f = ATmakeAFun("ListEnum", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeListEnum(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_MapSpec()
    {
      static AFun f = ATmakeAFun("MapSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeMapSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Merge()
    {
      static AFun f = ATmakeAFun("Merge", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeMerge(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_MultAct()
    {
      static AFun f = ATmakeAFun("MultAct", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeMultAct(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_MultActName()
    {
      static AFun f = ATmakeAFun("MultActName", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeMultActName(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Nil()
    {
      static AFun f = ATmakeAFun("Nil", 0, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeNil();
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Number()
    {
      static AFun f = ATmakeAFun("Number", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeNumber(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_OpId()
    {
      static AFun f = ATmakeAFun("OpId", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeOpId(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ProcEqn()
    {
      static AFun f = ATmakeAFun("ProcEqn", 4, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeProcEqn(0,0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ProcEqnSpec()
    {
      static AFun f = ATmakeAFun("ProcEqnSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeProcEqnSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_ProcVarId()
    {
      static AFun f = ATmakeAFun("ProcVarId", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeProcVarId(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Process()
    {
      static AFun f = ATmakeAFun("Process", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeProcess(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Rename()
    {
      static AFun f = ATmakeAFun("Rename", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeRename(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_RenameExpr()
    {
      static AFun f = ATmakeAFun("RenameExpr", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeRenameExpr(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Restrict()
    {
      static AFun f = ATmakeAFun("Restrict", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeRestrict(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Seq()
    {
      static AFun f = ATmakeAFun("Seq", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSeq(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SetBagComp()
    {
      static AFun f = ATmakeAFun("SetBagComp", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSetBagComp(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SetEnum()
    {
      static AFun f = ATmakeAFun("SetEnum", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSetEnum(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortArrow()
    {
      static AFun f = ATmakeAFun("SortArrow", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortArrow(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortArrowProd()
    {
      static AFun f = ATmakeAFun("SortArrowProd", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortArrowProd(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortBag()
    {
      static AFun f = ATmakeAFun("SortBag", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortBag(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortId()
    {
      static AFun f = ATmakeAFun("SortId", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortId(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortList()
    {
      static AFun f = ATmakeAFun("SortList", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortList(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortRef()
    {
      static AFun f = ATmakeAFun("SortRef", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortRef(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortSet()
    {
      static AFun f = ATmakeAFun("SortSet", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortSet(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortSpec()
    {
      static AFun f = ATmakeAFun("SortSpec", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortSpec(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SortStruct()
    {
      static AFun f = ATmakeAFun("SortStruct", 1, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSortStruct(0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_SpecV1()
    {
      static AFun f = ATmakeAFun("SpecV1", 7, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSpecV1(0,0,0,0,0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_StructCons()
    {
      static AFun f = ATmakeAFun("StructCons", 3, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeStructCons(0,0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_StructProj()
    {
      static AFun f = ATmakeAFun("StructProj", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeStructProj(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Sum()
    {
      static AFun f = ATmakeAFun("Sum", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSum(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Sync()
    {
      static AFun f = ATmakeAFun("Sync", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeSync(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Tau()
    {
      static AFun f = ATmakeAFun("Tau", 0, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeTau();
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Unknown()
    {
      static AFun f = ATmakeAFun("Unknown", 0, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeUnknown();
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_Whr()
    {
      static AFun f = ATmakeAFun("Whr", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeWhr(0,0);
      //return function_symbol(ATgetAFun(a));
    }

    inline
    function_symbol func_WhrDecl()
    {
      static AFun f = ATmakeAFun("WhrDecl", 2, ATfalse);
      return function_symbol(f);
      //ATprotectAFun(f);
      //static ATermAppl a = gsMakeWhrDecl(0,0);
      //return function_symbol(ATgetAFun(a));
    }


//--- end symbols  

} // namespace lpe

#endif // PREDEFINED_SYMBOLS_H
