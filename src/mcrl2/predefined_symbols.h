#ifndef PREDEFINED_SYMBOLS_H
#define PREDEFINED_SYMBOLS_H

/** @file Contains predefined function symbols (for efficiency reasons).
  */

#include "atermpp/aterm.h"

namespace mcrl2 {

using atermpp::function_symbol;

//--- begin symbols  
    function_symbol func_ActId()
    {
      static AFun f = ATmakeAFun("ActId", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ActSpec()
    {
      static AFun f = ATmakeAFun("ActSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Action()
    {
      static AFun f = ATmakeAFun("Action", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ActionProcess()
    {
      static AFun f = ATmakeAFun("ActionProcess", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Allow()
    {
      static AFun f = ATmakeAFun("Allow", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Assignment()
    {
      static AFun f = ATmakeAFun("Assignment", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_AtTime()
    {
      static AFun f = ATmakeAFun("AtTime", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_BInit()
    {
      static AFun f = ATmakeAFun("BInit", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_BagEnum()
    {
      static AFun f = ATmakeAFun("BagEnum", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_BagEnumElt()
    {
      static AFun f = ATmakeAFun("BagEnumElt", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Choice()
    {
      static AFun f = ATmakeAFun("Choice", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Comm()
    {
      static AFun f = ATmakeAFun("Comm", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_CommExpr()
    {
      static AFun f = ATmakeAFun("CommExpr", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Cond()
    {
      static AFun f = ATmakeAFun("Cond", 3, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ConsSpec()
    {
      static AFun f = ATmakeAFun("ConsSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataAppl()
    {
      static AFun f = ATmakeAFun("DataAppl", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataApplProd()
    {
      static AFun f = ATmakeAFun("DataApplProd", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataEqn()
    {
      static AFun f = ATmakeAFun("DataEqn", 4, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataEqnSpec()
    {
      static AFun f = ATmakeAFun("DataEqnSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataVarId()
    {
      static AFun f = ATmakeAFun("DataVarId", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_DataVarIdOpId()
    {
      static AFun f = ATmakeAFun("DataVarIdOpId", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Delta()
    {
      static AFun f = ATmakeAFun("Delta", 0, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Exists()
    {
      static AFun f = ATmakeAFun("Exists", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Forall()
    {
      static AFun f = ATmakeAFun("Forall", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Hide()
    {
      static AFun f = ATmakeAFun("Hide", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Init()
    {
      static AFun f = ATmakeAFun("Init", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_LMerge()
    {
      static AFun f = ATmakeAFun("LMerge", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_LPE()
    {
      static AFun f = ATmakeAFun("LPE", 3, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_LPEInit()
    {
      static AFun f = ATmakeAFun("LPEInit", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_LPESummand()
    {
      static AFun f = ATmakeAFun("LPESummand", 5, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Lambda()
    {
      static AFun f = ATmakeAFun("Lambda", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ListEnum()
    {
      static AFun f = ATmakeAFun("ListEnum", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_MapSpec()
    {
      static AFun f = ATmakeAFun("MapSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Merge()
    {
      static AFun f = ATmakeAFun("Merge", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_MultAct()
    {
      static AFun f = ATmakeAFun("MultAct", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_MultActName()
    {
      static AFun f = ATmakeAFun("MultActName", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Nil()
    {
      static AFun f = ATmakeAFun("Nil", 0, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Number()
    {
      static AFun f = ATmakeAFun("Number", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_OpId()
    {
      static AFun f = ATmakeAFun("OpId", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ProcEqn()
    {
      static AFun f = ATmakeAFun("ProcEqn", 4, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ProcEqnSpec()
    {
      static AFun f = ATmakeAFun("ProcEqnSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_ProcVarId()
    {
      static AFun f = ATmakeAFun("ProcVarId", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Process()
    {
      static AFun f = ATmakeAFun("Process", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Rename()
    {
      static AFun f = ATmakeAFun("Rename", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_RenameExpr()
    {
      static AFun f = ATmakeAFun("RenameExpr", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Restrict()
    {
      static AFun f = ATmakeAFun("Restrict", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Seq()
    {
      static AFun f = ATmakeAFun("Seq", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SetBagComp()
    {
      static AFun f = ATmakeAFun("SetBagComp", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SetEnum()
    {
      static AFun f = ATmakeAFun("SetEnum", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortArrow()
    {
      static AFun f = ATmakeAFun("SortArrow", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortArrowProd()
    {
      static AFun f = ATmakeAFun("SortArrowProd", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortBag()
    {
      static AFun f = ATmakeAFun("SortBag", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortId()
    {
      static AFun f = ATmakeAFun("SortId", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortList()
    {
      static AFun f = ATmakeAFun("SortList", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortRef()
    {
      static AFun f = ATmakeAFun("SortRef", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortSet()
    {
      static AFun f = ATmakeAFun("SortSet", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortSpec()
    {
      static AFun f = ATmakeAFun("SortSpec", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SortStruct()
    {
      static AFun f = ATmakeAFun("SortStruct", 1, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_SpecV1()
    {
      static AFun f = ATmakeAFun("SpecV1", 7, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_StructCons()
    {
      static AFun f = ATmakeAFun("StructCons", 3, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_StructProj()
    {
      static AFun f = ATmakeAFun("StructProj", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Sum()
    {
      static AFun f = ATmakeAFun("Sum", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Sync()
    {
      static AFun f = ATmakeAFun("Sync", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Tau()
    {
      static AFun f = ATmakeAFun("Tau", 0, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Unknown()
    {
      static AFun f = ATmakeAFun("Unknown", 0, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_Whr()
    {
      static AFun f = ATmakeAFun("Whr", 2, ATtrue);
      return function_symbol(f);
    }

    function_symbol func_WhrDecl()
    {
      static AFun f = ATmakeAFun("WhrDecl", 2, ATtrue);
      return function_symbol(f);
    }


//--- end symbols  

} // namespace mcrl2

#endif // PREDEFINED_SYMBOLS_H
