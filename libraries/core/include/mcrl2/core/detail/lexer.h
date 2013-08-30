// Author(s): Aad Mathijsen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/lexer.h
/// \brief These functions were moved from the lex and yacc files.

#ifndef MCRL2_CORE_DETAIL_LEXER_H
#define MCRL2_CORE_DETAIL_LEXER_H

namespace mcrl2
{

namespace core
{

namespace detail
{

// aterm_appl gsDataSpecEltsToSpec(aterm_list SpecElts);
// //Pre: SpecElts contains zero or more occurrences of sort, constructor,
// //     operation and data equation specifications.
// //Ret: data specification containing one sort, constructor, operation,
// //     and data equation specification, in that order.
//
// aterm_appl gsProcSpecEltsToSpec(aterm_list SpecElts);
// //Pre: SpecElts contains one process initialisation and zero or more
// //     occurrences of sort, constructor, operation, data equation, action and
// //     process equation specifications.
// //Ret: process specification containing one sort, constructor, operation,
// //     data equation, action and process equation specification, and one
// //     process initialisation, in that order.
//
// aterm_appl gsActionRenameEltsToActionRename(aterm_list SpecElts);
// //Pre: ActionRenameElts contains zero or more occurrences of
// //     sort, constructor, operation, equation, action and action rename
// //     rules.
// //Ret: specification containing one sort, constructor, operation, equation,
// //     action and action rename rules in that order.
//
// aterm_appl gsPBESSpecEltsToSpec(aterm_list SpecElts);
// //Pre: SpecElts contains one parameterised boolean initialisation and zero or
// //     more occurrences of sort, constructor, operation, data equation, action
// //     and parameterised boolean equation specifications.
// //Ret: BPES specification containing one sort, constructor, operation,
// //     data equation, action and parameterised boolean equation specification,
// //     and one parameterised boolean initialisation, in that order.

inline
aterm_appl gsDataSpecEltsToSpec(aterm_list SpecElts)
{
  aterm_appl Result = NULL;
  aterm_list SortDecls = ATmakeList0();
  aterm_list ConsDecls = ATmakeList0();
  aterm_list MapDecls = ATmakeList0();
  aterm_list DataEqnDecls = ATmakeList0();
  size_t n = ATgetLength(SpecElts);
  for (size_t i = 0; i < n; i++)
  {
    aterm_appl SpecElt = ATAelementAt(SpecElts, i);
    aterm_list SpecEltArg0 = ATLgetArgument(SpecElt, 0);
    if (gsIsSortSpec(SpecElt))
    {
      SortDecls = ATconcat(SortDecls, SpecEltArg0);
    }
    else if (gsIsConsSpec(SpecElt))
    {
      ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
    }
    else if (gsIsMapSpec(SpecElt))
    {
      MapDecls = ATconcat(MapDecls, SpecEltArg0);
    }
    else if (gsIsDataEqnSpec(SpecElt))
    {
      DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
    }
  }
  Result = gsMakeDataSpec(
             gsMakeSortSpec(SortDecls),
             gsMakeConsSpec(ConsDecls),
             gsMakeMapSpec(MapDecls),
             gsMakeDataEqnSpec(DataEqnDecls)
           );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //mCRL2log(debug) << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //mCRL2log(debug) << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //mCRL2log(debug) << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //mCRL2log(debug) << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

inline
aterm_appl gsProcSpecEltsToSpec(aterm_list SpecElts)
{
  aterm_appl Result = NULL;
  aterm_list SortDecls = ATmakeList0();
  aterm_list ConsDecls = ATmakeList0();
  aterm_list MapDecls = ATmakeList0();
  aterm_list DataEqnDecls = ATmakeList0();
  aterm_list GlobVars = ATmakeList0();
  aterm_list ActDecls = ATmakeList0();
  aterm_list ProcEqnDecls = ATmakeList0();
  aterm_appl ProcInit = NULL;
  size_t n = ATgetLength(SpecElts);
  for (size_t i = 0; i < n; i++)
  {
    aterm_appl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsProcessInit(SpecElt))
    {
      if (ProcInit == NULL)
      {
        ProcInit = SpecElt;
      }
      else
      {
        //ProcInit != NULL
        mCRL2log(log::error) << "parse error: multiple initialisations" << std::endl;
        return NULL;
      }
    }
    else
    {
      aterm_list SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt))
      {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      }
      else if (gsIsSortSpec(SpecElt))
      {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      }
      else if (gsIsConsSpec(SpecElt))
      {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      }
      else if (gsIsMapSpec(SpecElt))
      {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      }
      else if (gsIsDataEqnSpec(SpecElt))
      {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      }
      else if (gsIsActSpec(SpecElt))
      {
        ActDecls = ATconcat(ActDecls, SpecEltArg0);
      }
      else if (gsIsProcEqnSpec(SpecElt))
      {
        ProcEqnDecls = ATconcat(ProcEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether an initialisation is present
  if (ProcInit == NULL)
  {
    mCRL2log(log::error) << "parse error: missing initialisation" << std::endl;
    return NULL;
  }
  Result = gsMakeProcSpec(
             gsMakeDataSpec(
               gsMakeSortSpec(SortDecls),
               gsMakeConsSpec(ConsDecls),
               gsMakeMapSpec(MapDecls),
               gsMakeDataEqnSpec(DataEqnDecls)
             ),
             gsMakeActSpec(ActDecls),
             gsMakeGlobVarSpec(GlobVars),
             gsMakeProcEqnSpec(ProcEqnDecls),
             ProcInit
           );
  //Uncomment the lines below to check if the parser stack size isn't too big
  //mCRL2log(debug) << "SIZE_MAX:              " << SIZE_MAX << std::endl;
  //mCRL2log(debug) << "YYMAXDEPTH:            " << YYMAXDEPTH << std::endl;
  //mCRL2log(debug) << "sizeof (yyGLRStackItem): " << sizeof (yyGLRStackItem) << std::endl;
  //mCRL2log(debug) << "SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem): " << (SIZE_MAX < YYMAXDEPTH * sizeof (yyGLRStackItem)) << std::endl;
  return Result;
}

inline
aterm_appl gsPBESSpecEltsToSpec(aterm_list SpecElts)
{
  aterm_appl Result = NULL;
  aterm_list SortDecls = ATmakeList0();
  aterm_list ConsDecls = ATmakeList0();
  aterm_list MapDecls = ATmakeList0();
  aterm_list DataEqnDecls = ATmakeList0();
  aterm_list GlobVars = ATmakeList0();
  aterm_appl PBEqnSpec = NULL;
  aterm_appl PBInit = NULL;
  size_t n = ATgetLength(SpecElts);
  for (size_t i = 0; i < n; i++)
  {
    aterm_appl SpecElt = ATAelementAt(SpecElts, i);
    if (gsIsPBEqnSpec(SpecElt))
    {
      if (PBEqnSpec == NULL)
      {
        PBEqnSpec = SpecElt;
      }
      else
      {
        //PBEqnSpec != NULL
        mCRL2log(log::error) << "parse error: multiple parameterised boolean equation specifications" << std::endl;
        return NULL;
      }
    }
    else if (gsIsPBInit(SpecElt))
    {
      if (PBInit == NULL)
      {
        PBInit = SpecElt;
      }
      else
      {
        //PBInit != NULL
        mCRL2log(log::error) << "parse error: multiple initialisations" << std::endl;
        return NULL;
      }
    }
    else
    {
      aterm_list SpecEltArg0 = ATLgetArgument(SpecElt, 0);
      if (gsIsGlobVarSpec(SpecElt))
      {
        GlobVars = ATconcat(GlobVars, SpecEltArg0);
      }
      else if (gsIsSortSpec(SpecElt))
      {
        SortDecls = ATconcat(SortDecls, SpecEltArg0);
      }
      else if (gsIsConsSpec(SpecElt))
      {
        ConsDecls = ATconcat(ConsDecls, SpecEltArg0);
      }
      else if (gsIsMapSpec(SpecElt))
      {
        MapDecls = ATconcat(MapDecls, SpecEltArg0);
      }
      else if (gsIsDataEqnSpec(SpecElt))
      {
        DataEqnDecls = ATconcat(DataEqnDecls, SpecEltArg0);
      }
    }
  }
  //check whether a parameterised boolean equation specification is present
  if (PBEqnSpec == NULL)
  {
    mCRL2log(log::error) << "parse error: missing parameterised boolean equation specification" << std::endl;
    return NULL;
  }
  //check whether an initialisation is present
  if (PBInit == NULL)
  {
    mCRL2log(log::error) << "parse error: missing initialisation" << std::endl;
    return NULL;
  }
  Result = gsMakePBES(
             gsMakeDataSpec(
               gsMakeSortSpec(SortDecls),
               gsMakeConsSpec(ConsDecls),
               gsMakeMapSpec(MapDecls),
               gsMakeDataEqnSpec(DataEqnDecls)
             ),
             gsMakeGlobVarSpec(GlobVars),
             PBEqnSpec,
             PBInit
           );
  return Result;
}

inline
aterm_appl gsActionRenameEltsToActionRename(aterm_list ActionRenameElts)
{
  aterm_appl Result = NULL;
  aterm_list SortDecls = ATmakeList0();
  aterm_list ConsDecls = ATmakeList0();
  aterm_list MapDecls = ATmakeList0();
  aterm_list DataEqnDecls = ATmakeList0();
  aterm_list ActDecls = ATmakeList0();
  aterm_list ActionRenameRules = ATmakeList0();
  size_t n = ATgetLength(ActionRenameElts);
  for (size_t i = 0; i < n; i++)
  {
    aterm_appl ActionRenameElt = ATAelementAt(ActionRenameElts, i);
    aterm_list ActionRenameEltArg0 = ATLgetArgument(ActionRenameElt, 0);
    if (gsIsSortSpec(ActionRenameElt))
    {
      SortDecls = ATconcat(SortDecls, ActionRenameEltArg0);
    }
    else if (gsIsConsSpec(ActionRenameElt))
    {
      ConsDecls = ATconcat(ConsDecls, ActionRenameEltArg0);
    }
    else if (gsIsMapSpec(ActionRenameElt))
    {
      MapDecls = ATconcat(MapDecls, ActionRenameEltArg0);
    }
    else if (gsIsDataEqnSpec(ActionRenameElt))
    {
      DataEqnDecls = ATconcat(DataEqnDecls, ActionRenameEltArg0);
    }
    else if (gsIsActSpec(ActionRenameElt))
    {
      ActDecls = ATconcat(ActDecls, ActionRenameEltArg0);
    }
    else if (gsIsActionRenameRules(ActionRenameElt))
    {
      ActionRenameRules = ATconcat(ActionRenameRules, ActionRenameEltArg0);
    }
    else
    {
      assert(false);
    }
  }

  Result = gsMakeActionRenameSpec(
             gsMakeDataSpec(
               gsMakeSortSpec(SortDecls),
               gsMakeConsSpec(ConsDecls),
               gsMakeMapSpec(MapDecls),
               gsMakeDataEqnSpec(DataEqnDecls)
             ),
             gsMakeActSpec(ActDecls),
             gsMakeActionRenameRules(ActionRenameRules)
           );
  return Result;
}

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_LEXER_H
