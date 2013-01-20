// Author(s): Aad Mathijsen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/pp_deprecated.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_PP_DEPRECATED_H
#define MCRL2_CORE_DETAIL_PP_DEPRECATED_H

#include <string>
#include <ostream>
#include <iostream>

#include <assert.h>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/data/lambda.h"

namespace mcrl2 {

namespace core {

using namespace atermpp;

/// \brief t_pp_format represents the available pretty print formats
typedef enum { ppDefault, ppDebug, ppInternal, ppInternalDebug} t_pp_format;

namespace detail
{

/**
 *  * \brief Gets an aterm_appl at a specified position in a list
 *   **/
inline const aterm_appl &ATAelementAt(const aterm_list &List, const size_t Index)
{
  return aterm_cast<const aterm_appl>(element_at(List, Index));
}

/**
 *  * \brief Gets the argument as aterm_appl at the specified position
 *   **/
inline const aterm_appl &ATAgetArgument(const aterm_appl &Appl, const size_t Nr)
{
  const aterm &Result = Appl[Nr];
  // assert(ATisApplOrNull(Result));
  return aterm_cast<const aterm_appl>(Result);
}

/*
   \brief Gets the argument as aterm_list at the specified position
*/
inline const aterm_list &ATLgetArgument(const aterm_appl &Appl, const size_t Nr)
{
  const aterm &Result = Appl[Nr];
  // assert(ATisListOrNull(Result));
  return aterm_cast<const aterm_list>(Result);
}

/**
 *  * \brief Gets the first argument as aterm_appl
 *   **/
inline const aterm_appl &ATAgetFirst(const aterm_list &List)
{
  const aterm &Result = List.front();
  return aterm_cast<const aterm_appl>(Result);
}

/**
 * \brief Gets the first argument as aterm_list
 **/
inline const aterm_list &ATLgetFirst(const aterm_list &List)
{
  const aterm &Result = List.front();
  return aterm_cast<const aterm_list>(Result);
}

inline
aterm_list ATgetSlice(const aterm_list &list_in, const size_t start, const size_t end)
{
  size_t i, size;
  aterm_list result;
  std::vector<aterm> buffer(end<=start?0:end-start);

  if (end<=start)
  {
    return result;
  }

  size = end-start;

  aterm_list list=list_in;
  for (i=0; i<start; i++)
  {
    list = list.tail();
  }

  for (i=0; i<size; i++)
  {
    buffer[i] = list.front();
    list = list.tail();
  }

  for (i=size; i>0; i--)
  {
    result.push_front(buffer[i-1]);
  }

  return result;
}


/// \pre BoolExpr is a boolean expression, SortExpr is of type Pos, Nat, Int or
//     Real.
/// \return if(BoolExpr, 1, 0) of sort SortExpr
inline data::data_expression bool_to_numeric(data::data_expression const& e, data::sort_expression const& s)
{
  // TODO Maybe enforce that SortExpr is a PNIR sort
  return data::if_(e, data::function_symbol("1", s), data::function_symbol("0", s));
}

static inline
aterm_appl gsMakeEmptyDataSpec()
{
  return gsMakeDataSpec(
           gsMakeSortSpec(aterm_list()),
           gsMakeConsSpec(aterm_list()),
           gsMakeMapSpec(aterm_list()),
           gsMakeDataEqnSpec(aterm_list())
         );
}

//declarations
//------------

//The internal format mentioned below can be found in specs/mcrl2.internal.txt

static void PrintPart__CXX(std::ostream& OutStream, const aterm Part,
                                   t_pp_format pp_format);
/*Pre: OutStream points to a stream to which can be written
       Part is an aterm containing a part of the internal format
  Post:A textual representation of Part is written to OutStream, using method
       pp_format
*/

static void PrintPart_Appl(std::ostream& OutStream,
                                       const aterm_appl Part, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Part is an aterm_appl containing a part of the internal format
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintPart_List(std::ostream& OutStream,
                                       const aterm_list Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
                                       const char* Terminator, const char* Separator);
/*Pre: OutStream points to a stream to which can be written
       Parts is an aterm_list containing parts of the internal format
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the parts
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream using method
       pp_format. In this textual representation:
       - PrecLevel and ShowSort are distributed over the parts
       - each part is terminated by Terminator, if it is not NULL
       - two successive parts are separated by Separator, if it is not NULL
*/

static void PrintPart_BagEnum(std::ostream& OutStream,
    const aterm_list Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
    const char* Terminator, const char* Separator);
/*Pre: OutStream points to a stream to which can be written
       Parts is a Bag Enumeration containing parts of the internal format
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the parts
       0 <= PrecLevel
  Post:A textual representation of Parts is written to OutStream using method
       pp_format. In this textual representation:
       - PrecLevel and ShowSort are distributed over the parts
       - each part is terminated by Terminator, if it is not NULL
       - two successive parts are separated by Separator, if it is not NULL
*/


static void PrintEqns(std::ostream& OutStream,
                                  const aterm_list Eqns, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Eqns is an aterm_list containing data or rename equations in the
       internal format
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for each equation
       PrecLevel indicates the precedence level of the context of the equations
       0 <= PrecLevel
  Post:A textual representation of the equations is written to OutStream using
       method pp_format, in which:
       - equations are grouped in equation sections, i.e. variable declarations
         apply to groups of equations
       - PrecLevel and ShowSort are distributed over the equations
*/

static void PrintDecls(std::ostream& OutStream,
                                   const aterm_list Decls, t_pp_format pp_format,
                                   const char* Terminator, const char* Separator);
/*Pre: Decls is an aterm_list containing action, operation, or variable
       declarations from the internal format
       pp_format != ppInternal
  Ret: A textual representation of the declarations is written to OutStream,
       in which, if pp_format == ppDefault:
       - of two consecutive declarations Decl(x, S) and Decl(y, T), the first
         is printed as:
         + "x,", if S = T
         + "x: S", followed by Terminator and Separator, if S != T
       - the last declaration Decl(x, S) is printed as "x: S", followed by
         Terminator
*/

static void PrintDecl(std::ostream& OutStream,
                                  const aterm_appl Decl, t_pp_format pp_format, const bool ShowSorts);
/*Pre: Decl is an aterm_appl that represents an action, operation, or variable
       declaration from the internal format
       pp_format != ppInternal
       ShowSorts indicates if the sort of the declaration should be shown
  Ret: A textual representation of the declaration, say Decl(x, S), is written
       to OutStream, i.e.:
       - "x: S", if ShowSorts
       - "x", otherwise
*/

static void PrintSortExpr(std::ostream& OutStream,
                                      const aterm_appl SortExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       SortExprOrUnknown is a sort expression or unknown
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintDataExpr(std::ostream& OutStream,
                                      const aterm_appl DataExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       DataExpr is a data expression
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintProcExpr(std::ostream& OutStream,
                                      const aterm_appl ProcExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       ProcExpr is a process expression
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintStateFrm(std::ostream& OutStream,
                                      const aterm_appl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       StateFrm is a state formula
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintRegFrm(std::ostream& OutStream,
                                    const aterm_appl RegFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       RegFrm is a regular formula
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintActFrm(std::ostream& OutStream,
                                    const aterm_appl ActFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       ActFrm is an action formula
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintPBExpr(std::ostream& OutStream,
                                    const aterm_appl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       PBExpr is a parameterised boolean expression
       pp_format != ppInternal
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of Part is written to OutStream, using method
       pp_format. In this textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

static void PrintLinearProcessSummand(std::ostream& OutStream,
    const aterm_appl LinearProcessSummand, t_pp_format pp_format, bool ShowSorts);
/*Pre: OutStream points to a stream to which can be written
       LinearProcessSummand is a linear process summand
       pp_format != ppInternal
       ShowSorts indicates if the sorts of DataExpr should be shown
  Post:A textual representation of the expression is written to OutStream, in
       which ShowSorts is taken into account
*/

static void IndentedATerm(std::ostream& OutStream, const aterm Term, size_t Nesting = 0);
//Pre:  Term is an aterm containing applications and lists only
//      Nesting indicates the nesting depth of Term in its context
//Post: A string representation of the aterm is written to OutStream,
//      such that each element part of the aterm is indented by Nesting + its nesting depth

static void PrintAFun(std::ostream& OutStream, const function_symbol Fun);
//Post: The string representation Fun is written to OutStream

static aterm_list GetAssignmentsRHS(aterm_list Assignments);
/*Pre: Assignments is a list of assignments of data expressions to data
       variables
  Ret: The right-hand sides the assignments
*/

static aterm_list gsGroupDeclsBySort(aterm_list Decls);
/*Pre: Decls is an aterm_list containing declarations of the form
       Decl(Name, Sort) from the internal format
  Ret: a list containing the declarations from Decls, where declarations of the
       same sort are placed in sequence
*/

static bool gsHasConsistentContext(const table &DataVarDecls,
                                   const aterm_appl Part);
/*Pre: DataVarDecls represents the variables from an equation section, where
       the keys are the variable names and the values are the corresponding
       variables
       Part is an aterm_appl containing an equation of the internal format,
       or the elements it consists of
  Ret: all operations occurring in Part are consistent with the variables from
       the context
 */

static bool gsHasConsistentContextList(const table &DataVarDecls,
                                       const aterm_list Parts);
/*Pre: DataVarDecls represents the variables from an equation section, where
       the keys are the variable names and the values are the
       corresponding variables
       Parts is an aterm_list containing elements of an equation of the internal
       format
  Ret: all operations occurring in Parts are consistent with the variables from
       the context
 */

static bool gsIsOpIdNumericUpCast(aterm_appl DataExpr);
//Ret: DataExpr is an operation identifier for a numeric upcast

static bool gsIsIdListEnum(aterm_appl DataExpr);
//Ret: DataExpr is a list enumeration identifier

static bool gsIsIdSetEnum(aterm_appl DataExpr);
//Ret: DataExpr is a set enumeration identifier

static bool gsIsIdBagEnum(aterm_appl DataExpr);
//Ret: DataExpr is a bag enumeration identifier

static bool gsIsIdFuncUpdate(aterm_appl DataExpr);
//Ret: DataExpr is a function update identifier

static bool gsIsIdPrefix(aterm_appl DataExpr, size_t ArgsLength);
//Ret: DataExpr is a prefix identifier and ArgsLength == 1

static bool gsIsIdInfix(aterm_appl DataExpr, size_t ArgsLength);
//Ret: DataExpr is an infix identifier and ArgsLength == 2

static int gsPrecIdPrefix();
//Ret: Precedence of prefix operators
//     (higher than all infix operators and arguments thereof)

static int gsPrecIdInfix(aterm_appl IdName);
//Pre: IdName is the name of an infix identifier
//Ret: Precedence of the operation itself

static int gsPrecIdInfixLeft(aterm_appl IdName);
//Pre: IdInfix is the name of an infix identifier
//Ret: Precedence of the left argument of the operation

static int gsPrecIdInfixRight(aterm_appl IdName);
//Pre: IdInfix is the name of an infix identifier
//Ret: Precedence of the right argument of the operation


//implementation
//--------------


inline static void dbg_prints(const char* Value)
{
  static_cast<void>(Value); // Harmless
//Pre:  Value is not NULL
//Post: Value is written to stderr in C/C++ style if gsDebug is true
  mCRL2log(log::debug2, "pretty printer") << Value << std::endl;
}

static void PrintAFun(std::ostream& OutStream, const function_symbol fun)
{
  OutStream <<  "\"" << fun.name() << "\"";
}

static void IndentedATerm(std::ostream& OutStream, const aterm term, size_t nesting)
{
  std::string prefix(2*nesting, ' ');
  if (term.type_is_appl())
  {
    OutStream <<  prefix.c_str();
    aterm_appl appl = (aterm_appl) term;
    function_symbol fun = appl.function();
    PrintAFun(OutStream, fun);
    size_t arity = fun.arity();
    if (arity > 0)
    {
      OutStream <<  "(\n";
      for (size_t i = 0; i < arity; i++)
      {
        IndentedATerm(OutStream, appl[i], nesting+1);
        if (i+1 < arity)
        {
          OutStream <<  ",\n";
        }
      }
      OutStream <<  "\n";
      OutStream <<  prefix;
      OutStream <<  ")";
    }
  }
  else if (term.type_is_list())
  {
    OutStream <<  prefix;
    if (((aterm_list) term).empty())
    {
      OutStream <<  "[]";
    }
    else
    {
      OutStream <<  "[\n";
      for (aterm_list l = (aterm_list) term; !l.empty(); l = l.tail())
      {
        IndentedATerm(OutStream, l.front(), nesting+1);
        if (!l.tail().empty())
        {
          OutStream <<  ",\n";
        }
      }
      OutStream <<  "\n";
      OutStream <<  prefix;
      OutStream <<  "]";
    }
  }
  else
  {
    OutStream <<  "ERROR: term is not an aterm_appl or aterm_list";
  }
}

void PrintPart__CXX(std::ostream& OutStream, const aterm Part,
                            t_pp_format pp_format)
{
  if (pp_format == ppInternal)
  {
    OutStream << Part.to_string() << std::endl;
  }
  else if (pp_format == ppInternalDebug)
  {
    IndentedATerm(OutStream, Part);
  }
  else
  {
    if (Part.type_is_appl())
    {
      PrintPart_Appl(OutStream, (aterm_appl) Part, pp_format,
                                 false, 0);
    }
    else if (Part.type_is_list())
    {
      OutStream <<  "[";
      PrintPart_List(OutStream, (aterm_list) Part,
                                 pp_format, false, 0, "", ", ");
      OutStream <<  "]";
    }
    else
    {
      mCRL2log(log::error)  << "aterm Part is not an aterm_appl or an aterm_list" << std::endl;
      assert(false);
    }
  }
}

void PrintPart_Appl(std::ostream& OutStream,
                                const aterm_appl Part, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  if (gsIsSortExpr(Part))
  {
    //print sort expression or unknown
    mCRL2log(log::debug2, "pretty printer") << "printing sort expression or unknown\n" << std::endl;
    PrintSortExpr(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsStructCons(Part))
  {
    //print structured sort constructor
    mCRL2log(log::debug2, "pretty printer") << "printing structured sort constructor" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list StructProjs = ATLgetArgument(Part, 1);
    if (StructProjs.size() > 0)
    {
      OutStream <<  "(";
      PrintPart_List(OutStream, StructProjs,
                                 pp_format, ShowSorts, PrecLevel, NULL, ", ");
      OutStream <<  ")";
    }
    aterm_appl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser))
    {
      OutStream <<  "?";
      PrintPart_Appl(OutStream, Recogniser,
                                 pp_format, ShowSorts, PrecLevel);
    }
  }
  else if (gsIsStructProj(Part))
  {
    //print structured sort projection
    mCRL2log(log::debug2, "pretty printer") << "printing structured sort projection" << std::endl;
    aterm_appl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection))
    {
      PrintPart_Appl(OutStream, Projection,
                                 pp_format, ShowSorts, PrecLevel);
      OutStream <<  ": ";
    }
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsNil(Part))
  {
    //print nil
    mCRL2log(log::debug2, "pretty printer") << "printing nil" << std::endl;
    OutStream <<  "nil";
  }
  else if (gsIsDataExpr(Part))
  {
    //print data expression
    mCRL2log(log::debug2, "pretty printer") << "printing data expression" << std::endl;
    PrintDataExpr(OutStream, Part, pp_format, ShowSorts, PrecLevel);

  }
  else if (gsIsIdInit(Part))
  {
    //print identifier initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing identifier initialisation" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " = ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsMultAct(Part))
  {
    //print multiaction
    mCRL2log(log::debug2, "pretty printer") << "printing multiaction" << std::endl;
    aterm_list Actions = ATLgetArgument(Part, 0);
    size_t ActionsLength = Actions.size();
    if (ActionsLength == 0)
    {
      OutStream <<  "tau";
    }
    else
    {
      //ActionsLength > 0
      if (PrecLevel > 7)
      {
        OutStream <<  "(";
      }
      PrintPart_List(OutStream, Actions,
                                 pp_format, ShowSorts, PrecLevel, NULL, "|");
      if (PrecLevel > 7)
      {
        OutStream <<  ")";
      }
    }
  }
  else if (gsIsActId(Part))
  {
    //print action identifier
    mCRL2log(log::debug2, "pretty printer") << "printing action identifier" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    if (ShowSorts)
    {
      aterm_list SortExprs = ATLgetArgument(Part, 1);
      if (SortExprs.size() > 0)
      {
        OutStream <<  ": ";
        PrintPart_List(OutStream, SortExprs,
                                   pp_format, ShowSorts, 2, NULL, " # ");
      }
    }
  }
  else if (gsIsProcExpr(Part))
  {
    //print process expression
    mCRL2log(log::debug2, "pretty printer") << "printing process expression" << std::endl;
    PrintProcExpr(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsProcVarId(Part))
  {
    //print process variable
    mCRL2log(log::debug2, "pretty printer") << "printing process variable" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    if (ShowSorts)
    {
      aterm_list SortExprs = ATLgetArgument(Part, 1);
      if (SortExprs.size() > 0)
      {
        OutStream <<  ": ";
        PrintPart_List(OutStream, SortExprs,
                                   pp_format, ShowSorts, 2, NULL, " # ");
      }
    }
  }
  else if (gsIsMultActName(Part))
  {
    //print multiaction name
    mCRL2log(log::debug2, "pretty printer") << "printing multiaction name" << std::endl;
    PrintPart_List(OutStream, ATLgetArgument(Part, 0),
                               pp_format, ShowSorts, 0, NULL, " | ");
  }
  else if (gsIsRenameExpr(Part))
  {
    //print renaming expression
    mCRL2log(log::debug2, "pretty printer") << "printing renaming expression" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " -> ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsCommExpr(Part))
  {
    //print communication expression
    mCRL2log(log::debug2, "pretty printer") << "printing communication expression" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_appl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult))
    {
      OutStream <<  " -> ";
      PrintPart_Appl(OutStream, CommResult,
                                 pp_format, ShowSorts, PrecLevel);
    }
  }
  else if (gsIsProcSpec(Part) || gsIsLinProcSpec(Part))
  {
    //print process specification or LPS
    mCRL2log(log::debug2, "pretty printer") << "printing process specification or LPS" << std::endl;
    aterm_appl DataSpec = ATAgetArgument(Part, 0);
    bool DataSpecEmpty = DataSpec==gsMakeEmptyDataSpec();
    aterm_appl ActSpec = ATAgetArgument(Part, 1);
    bool ActSpecEmpty = (ATLgetArgument(ActSpec, 0)).empty();
    aterm_appl GlobVarSpec = ATAgetArgument(Part, 2);
    bool GlobVarSpecEmpty = (ATLgetArgument(GlobVarSpec, 0)).empty();
    aterm_appl ProcEqnSpec = ATAgetArgument(Part, 3);
    bool ProcEqnSpecEmpty = gsIsProcSpec(Part)?(bool)(ATLgetArgument(ProcEqnSpec, 0)).empty():false;
    aterm_appl ProcInit = ATAgetArgument(Part, 4);
    PrintPart_Appl(OutStream, DataSpec, pp_format, ShowSorts, PrecLevel);
    if (!ActSpecEmpty && !DataSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ActSpec, pp_format, ShowSorts, PrecLevel);
    if (!GlobVarSpecEmpty && (!DataSpecEmpty || !ActSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, GlobVarSpec, pp_format, ShowSorts, PrecLevel);
    if (!ProcEqnSpecEmpty && (!DataSpecEmpty || !ActSpecEmpty || !GlobVarSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ProcEqnSpec, pp_format, ShowSorts, PrecLevel);
    if (!DataSpecEmpty || !ActSpecEmpty || !GlobVarSpecEmpty || !ProcEqnSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ProcInit, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsDataSpec(Part))
  {
    //print data specification
    mCRL2log(log::debug2, "pretty printer") << "printing data specification" << std::endl;
    aterm_appl SortSpec = ATAgetArgument(Part, 0);
    bool SortSpecEmpty = (ATLgetArgument(SortSpec, 0)).empty();
    aterm_appl ConsSpec = ATAgetArgument(Part, 1);
    bool ConsSpecEmpty = (ATLgetArgument(ConsSpec, 0)).empty();
    aterm_appl MapSpec = ATAgetArgument(Part, 2);
    bool MapSpecEmpty = (ATLgetArgument(MapSpec, 0)).empty();
    aterm_appl DataEqnSpec = ATAgetArgument(Part, 3);
    bool DataEqnSpecEmpty = (ATLgetArgument(DataEqnSpec, 0)).empty();
    PrintPart_Appl(OutStream, SortSpec, pp_format, ShowSorts, PrecLevel);
    if (!ConsSpecEmpty && !SortSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ConsSpec, pp_format, ShowSorts, PrecLevel);
    if (!MapSpecEmpty && (!SortSpecEmpty || !ConsSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, MapSpec, pp_format, ShowSorts, PrecLevel);
    if (!DataEqnSpecEmpty && (!SortSpecEmpty || !ConsSpecEmpty || !MapSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, DataEqnSpec, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsSortSpec(Part))
  {
    //print sort specification
    mCRL2log(log::debug2, "pretty printer") << "printing sort specification" << std::endl;
    aterm_list SortDecls = ATLgetArgument(Part, 0);
    if (SortDecls.size() > 0)
    {
      OutStream <<  "sort ";
      PrintPart_List(OutStream, SortDecls,
                                 pp_format, ShowSorts, PrecLevel, ";\n", "     ");
    }
  }
  else if (gsIsConsSpec(Part) || gsIsMapSpec(Part))
  {
    //print operation specification
    mCRL2log(log::debug2, "pretty printer") << "printing operation specification" << std::endl;
    aterm_list OpIds = ATLgetArgument(Part, 0);
    if (OpIds.size() > 0)
    {
      OutStream <<  (gsIsConsSpec(Part)?"cons ":"map  ");
      PrintDecls(OutStream, OpIds, pp_format, ";\n", "     ");
    }
  }
  else if (gsIsDataEqnSpec(Part))
  {
    //print equation specification
    mCRL2log(log::debug2, "pretty printer") << "printing equation specification" << std::endl;
    PrintEqns(OutStream, ATLgetArgument(Part, 0),
                          pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsActSpec(Part))
  {
    //print action specification
    mCRL2log(log::debug2, "pretty printer") << "printing action specification" << std::endl;
    aterm_list ActIds = ATLgetArgument(Part, 0);
    if (ActIds.size() > 0)
    {
      OutStream <<  "act  ";
      PrintDecls(OutStream, ActIds, pp_format, ";\n", "     ");
    }
  }
  else if (gsIsSortRef(Part))
  {
    //print sort reference
    mCRL2log(log::debug2, "pretty printer") << "printing sort reference declaration" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " = ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsDataEqn(Part))
  {
    //print data equation (without variables)
    mCRL2log(log::debug2, "pretty printer") << "printing data equation" << std::endl;
    aterm_appl Condition = ATAgetArgument(Part, 1);
    if (/*!gsIsNil(Condition) && */!data::sort_bool::is_true_function_symbol(data::data_expression(Condition)))
    {
      PrintPart_Appl(OutStream, Condition,
                                 pp_format, ShowSorts, 0);
      OutStream <<  "  ->  ";
    }
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 2),
                               pp_format, ShowSorts, 0);
    OutStream <<  "  =  ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 3),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsGlobVarSpec(Part))
  {
    //print global variable specification
    mCRL2log(log::debug2, "pretty printer") << "printing global variable specification" << std::endl;
    aterm_list Vars = ATLgetArgument(Part, 0);
    if (Vars.size() > 0)
    {
      OutStream <<  "glob ";
      PrintDecls(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
                             pp_format, ";\n", "     ");
    }
  }
  else if (gsIsProcEqnSpec(Part))
  {
    //print process equation specification
    mCRL2log(log::debug2, "pretty printer") << "printing process equation specification" << std::endl;
    aterm_list ProcEqns = ATLgetArgument(Part, 0);
    if (ProcEqns.size() > 0)
    {
      OutStream <<  "proc ";
      PrintPart_List(OutStream, ProcEqns,
                                 pp_format, ShowSorts, PrecLevel, ";\n", "     ");
    }
  }
  else if (gsIsLinearProcess(Part))
  {
    //print linear process
    mCRL2log(log::debug2, "pretty printer") << "printing linear process" << std::endl;
    //print process name and variable declarations
    aterm_list VarDecls = ATLgetArgument(Part, 0);
    size_t VarDeclsLength = VarDecls.size();
    OutStream <<  "proc P";
    if (VarDeclsLength > 0)
    {
      OutStream <<  "(";
      PrintDecls(OutStream, VarDecls, pp_format, NULL, ", ");
      OutStream <<  ")";
    }
    OutStream <<  " =";
    //print summations
    aterm_list Summands = ATLgetArgument(Part, 1);
    size_t SummandsLength = Summands.size();
    if (SummandsLength == 0)
    {
      OutStream <<  " delta@0;\n";
    }
    else
    {
      //SummandsLength > 0
      OutStream <<  "\n       ";
      aterm_list l = Summands;
      while (!l.empty())
      {
        if (l!=Summands)
        {
          OutStream <<  "\n     + ";
        }
        PrintLinearProcessSummand(OutStream, ATAgetFirst(l),
                                              pp_format, ShowSorts);
        l = l.tail();
      }
      OutStream <<  ";\n";
    }
  }
  else if (gsIsProcEqn(Part))
  {
    //print process equation
    mCRL2log(log::debug2, "pretty printer") << "printing process equation" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list DataVarIds = ATLgetArgument(Part, 1);
    if (DataVarIds.size() > 0)
    {
      OutStream <<  "(";
      PrintDecls(OutStream, DataVarIds, pp_format, NULL, ", ");
      OutStream <<  ")";
    }
    OutStream <<  " = ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 2),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsLinearProcessSummand(Part))
  {
    //print summand
    PrintLinearProcessSummand(OutStream, Part, pp_format, ShowSorts);
  }
  else if (gsIsProcessInit(Part))
  {
    //print initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing initialisation" << std::endl;
    OutStream <<  "init ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  ";\n";
  }
  else if (gsIsLinearProcessInit(Part))
  {
    //print linear process initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing LPS initialisation" << std::endl;
    OutStream <<  "init P";
    aterm_list Args = ATLgetArgument(Part, 0);
    if (Args.size() > 0)
    {
      OutStream <<  "(";
      if (pp_format == ppDefault)
      {
        Args = GetAssignmentsRHS(Args);
      }
      PrintPart_List(OutStream, Args,
                                 pp_format, ShowSorts, 0, NULL, ", ");
      OutStream <<  ")";
    }
    OutStream <<  ";\n";
  }
  else if (gsIsStateFrm(Part))
  {
    //print state formula
    mCRL2log(log::debug2, "pretty printer") << "printing state formula" << std::endl;
    PrintStateFrm(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsDataVarIdInit(Part))
  {
    //print data variable identifier and initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing data variable identifier and initialisation" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " = ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsRegFrm(Part))
  {
    //print regular formula
    mCRL2log(log::debug2, "pretty printer") << "printing regular formula" << std::endl;
    PrintRegFrm(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsPBES(Part))
  {
    //print PBES specification
    mCRL2log(log::debug2, "pretty printer") << "printing PBES specification" << std::endl;
    aterm_appl DataSpec = ATAgetArgument(Part, 0);
    bool DataSpecEmpty = DataSpec==gsMakeEmptyDataSpec();
    aterm_appl GlobVarSpec = ATAgetArgument(Part, 1);
    bool GlobVarSpecEmpty = (ATLgetArgument(GlobVarSpec, 0)).empty();
    aterm_appl PBEqnSpec = ATAgetArgument(Part, 2);
    bool PBEqnSpecEmpty = (ATLgetArgument(PBEqnSpec, 0)).empty();
    aterm_appl PBInit = ATAgetArgument(Part, 3);
    PrintPart_Appl(OutStream, DataSpec, pp_format, ShowSorts, PrecLevel);
    if (!GlobVarSpecEmpty && !DataSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, GlobVarSpec, pp_format, ShowSorts, PrecLevel);
    if (!PBEqnSpecEmpty && (!DataSpecEmpty || !GlobVarSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, PBEqnSpec, pp_format, ShowSorts, PrecLevel);
    if (!DataSpecEmpty || !GlobVarSpecEmpty || !PBEqnSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, PBInit, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsPBEqnSpec(Part))
  {
    //print parameterised boolean equation specification
    mCRL2log(log::debug2, "pretty printer") << "printing parameterised boolean equation specification" << std::endl;
    aterm_list PBEqns = ATLgetArgument(Part, 0);
    if (PBEqns.size() > 0)
    {
      OutStream <<  "pbes ";
      PrintPart_List(OutStream, PBEqns,
                                 pp_format, ShowSorts, PrecLevel, ";\n", "     ");
    }
  }
  else if (gsIsPBInit(Part))
  {
    //print parameterised boolean initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing parameterised boolean initialisation" << std::endl;
    OutStream <<  "init ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  ";\n";
  }
  else if (gsIsPBEqn(Part))
  {
    //print parameterised boolean equation
    mCRL2log(log::debug2, "pretty printer") << "printing parameterised boolean equation" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 1),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  " =\n       ";
    PrintPBExpr(OutStream, ATAgetArgument(Part, 2),
                            pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsFixpoint(Part))
  {
    //print fixpoint
    mCRL2log(log::debug2, "pretty printer") << "printing fixpoint" << std::endl;
    if (gsIsNu(Part))
    {
      OutStream <<  "nu";
    }
    else
    {
      OutStream <<  "mu";
    }
  }
  else if (gsIsPropVarDecl(Part))
  {
    //print propositional variable declaration
    mCRL2log(log::debug2, "pretty printer") << "printing propositional variable declaration" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list DataVarIds = ATLgetArgument(Part, 1);
    if (DataVarIds.size() > 0)
    {
      OutStream <<  "(";
      PrintDecls(OutStream, DataVarIds, pp_format, NULL, ", ");
      OutStream <<  ")";
    }
  }
  else if (gsIsPBExpr(Part))
  {
    //print parameterised boolean expression
    mCRL2log(log::debug2, "pretty printer") << "printing parameterised boolean expression" << std::endl;
    PrintPBExpr(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsActionRenameSpec(Part))
  {
    //print action rename specification
    mCRL2log(log::debug2, "pretty printer") << "printing action rename specification" << std::endl;
    aterm_appl DataSpec = ATAgetArgument(Part, 0);
    bool DataSpecEmpty = DataSpec==gsMakeEmptyDataSpec();
    aterm_appl ActSpec = ATAgetArgument(Part, 1);
    bool ActSpecEmpty = (ATLgetArgument(ActSpec, 0)).empty();
    aterm_appl ActionRenameRules = ATAgetArgument(Part, 2);
    bool ActionRenameRulesEmpty = (ATLgetArgument(ActionRenameRules, 0)).empty();
    PrintPart_Appl(OutStream, DataSpec, pp_format, ShowSorts, PrecLevel);
    if (!ActSpecEmpty && !DataSpecEmpty)
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ActSpec, pp_format, ShowSorts, PrecLevel);
    if (!ActionRenameRulesEmpty && (!DataSpecEmpty || !ActSpecEmpty))
    {
      OutStream <<  "\n";
    }
    PrintPart_Appl(OutStream, ActionRenameRules, pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsActionRenameRule(Part))
  {
    //print action rename rule (without variables)
    mCRL2log(log::debug2, "pretty printer") << "printing action rename rule" << std::endl;
    aterm_appl Condition = ATAgetArgument(Part, 1);
    if (/*!gsIsNil(Condition) && */!data::sort_bool::is_true_function_symbol(data::data_expression(Condition)))
    {
      PrintPart_Appl(OutStream, Condition,
                                 pp_format, ShowSorts, 0);
      OutStream <<  "  ->  ";
    }
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 2),
                               pp_format, ShowSorts, 0);
    OutStream <<  "  =>  ";
    PrintPart_Appl(OutStream, ATAgetArgument(Part, 3),
                               pp_format, ShowSorts, 0);
  }
  else if (gsIsActionRenameRules(Part))
  {
    //print action rename rules
    mCRL2log(log::debug2, "pretty printer") << "printing action rename rules" << std::endl;
    PrintEqns(OutStream, ATLgetArgument(Part, 0),
                          pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsString(Part))
  {
    //print string
    OutStream <<  Part.function().name();
  }
  else
  {
    mCRL2log(log::error)
        << "the term "
        << Part.to_string()
        << " is not part of the internal format" << std::endl;
    assert(false);
  }
}

void PrintPart_List(std::ostream& OutStream,
                                const aterm_list Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
                                const char* Terminator, const char* Separator)
{
  aterm_list l = Parts;
  while (!l.empty())
  {
    if (l!=Parts && Separator != NULL)
    {
      OutStream <<  Separator;
    }
    PrintPart_Appl(OutStream, ATAgetFirst(l),
                               pp_format, ShowSorts, PrecLevel);
    if (Terminator != NULL)
    {
      OutStream <<  Terminator;
    }
    l = l.tail();
  }
}

void PrintPart_BagEnum(std::ostream& OutStream,
                                   const aterm_list Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
                                   const char* Terminator, const char* Separator)
{
  aterm_list l = Parts;
  while (!l.empty())
  {
    if (l!=Parts && Separator != NULL)
    {
      OutStream <<  Separator;
    }
    PrintPart_Appl(OutStream, ATAgetFirst(l),
                               pp_format, ShowSorts, PrecLevel);
    l = l.tail();
    OutStream <<  ": ";
    PrintPart_Appl(OutStream, ATAgetFirst(l),
                               pp_format, ShowSorts, PrecLevel);
    if (Terminator != NULL)
    {
      OutStream <<  Terminator;
    }
    l = l.tail();
  }
}

void PrintEqns(std::ostream& OutStream, const aterm_list Eqns,
                           t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  if (pp_format == ppDebug)
  {
    aterm_list l = Eqns;
    while (!l.empty())
    {
      aterm_appl Eqn = ATAgetFirst(l);
      aterm_list DataDecls = ATLgetArgument(Eqn, 0);
      if (!DataDecls.empty())
      {
        OutStream <<  "var  ";
        PrintDecls(OutStream, (pp_format == ppDebug)?DataDecls:gsGroupDeclsBySort(DataDecls),
                               pp_format, ";\n", "     ");
      }
      if (gsIsDataEqn(Eqn))
      {
        OutStream <<  "eqn  ";
      }
      else if (gsIsActionRenameRule(Eqn))
      {
        OutStream <<  "rename\n     ";
      }
      else
      {
        assert(false);
      }
      PrintPart_Appl(OutStream, Eqn,
                                 pp_format, ShowSorts, PrecLevel);
      OutStream <<  ";\n";
      l = l.tail();
      if (!l.empty())
      {
        OutStream <<  "\n";
      }
    }
  }
  else     //pp_format == ppDefault
  {
    size_t EqnsLength = Eqns.size();
    if (EqnsLength > 0)
    {
      size_t StartPrefix = 0;
      table VarDeclTable(63, 50);
      //VarDeclTable is a hash table with variable declarations as values, where
      //the name of each variable declaration is used a key.
      //Note that the hash table will be increased if at least 32 values are added,
      //This can be avoided by increasing the initial size.
      size_t i = 0;
      while (i < EqnsLength)
      {
        //StartPrefix represents the start index of the maximum consistent prefix
        //of variable declarations in Eqns to which Eqns(i) belongs
        //VarDeclTable represents the variable declarations of Eqns from
        //StartPrefix up to i.
        //Check consistency of Eqns(i) with VarDeclTable and add newly declared
        //variables to VarDeclTable.
        aterm_appl Eqn = ATAelementAt(Eqns, i);
        bool Consistent = gsHasConsistentContext(VarDeclTable, Eqn);
        if (Consistent)
        {
          //add new variables from Eqns(i) to VarDeclTable
          aterm_list VarDecls = ATLgetArgument(Eqn, 0);
          size_t VarDeclsLength = VarDecls.size();
          for (size_t j = 0; j < VarDeclsLength; j++)
          {
            aterm_appl VarDecl = ATAelementAt(VarDecls, j);
            aterm_appl VarDeclName = ATAgetArgument(VarDecl, 0);
            if (VarDeclTable.get(VarDeclName) == aterm())
            {
              VarDeclTable.put(VarDeclName, VarDecl);
            }
          }
          i++;
        }
        if (!Consistent || (i == EqnsLength))
        {
          //VarDeclTable represents the maximum consistent prefix of variable
          //declarations of Eqns starting at StartPrefix. Print this prefixa and
          //the corresponding equations,and if necessary, update StartPrefix and
          //reset VarDeclTable.
          aterm_list VarDecls = VarDeclTable.values();
          if (VarDecls.size() > 0)
          {
            OutStream <<  "var  ";
            PrintDecls(OutStream, (pp_format == ppDebug)?VarDecls:gsGroupDeclsBySort(VarDecls),
                                   pp_format, ";\n", "     ");
          }
          if (gsIsDataEqn(Eqn))
          {
            OutStream <<  "eqn  ";
          }
          else if (gsIsActionRenameRule(Eqn))
          {
            OutStream <<  "rename\n     ";
          }
          else
          {
            assert(false);
          }
          PrintPart_List(OutStream, ATgetSlice(Eqns, StartPrefix, i),
                                     pp_format, ShowSorts, PrecLevel, ";\n", "     ");
          if (i < EqnsLength)
          {
            OutStream <<  "\n";
            StartPrefix = i;
            VarDeclTable.clear();
          }
        }
      }
      //finalisation after printing all (>0) equations
    }
  }
}

void PrintDecls(std::ostream& OutStream, const aterm_list Decls,
                            t_pp_format pp_format, const char* Terminator, const char* Separator)
{
  if (!Decls.empty())
  {
    aterm_appl Decl = ATAgetFirst(Decls);
    aterm_list NextDecls = Decls.tail();
    while (!NextDecls.empty())
    {
      if (Decl[1]==ATAgetFirst(NextDecls)[1])
      {
        PrintDecl(OutStream, Decl, pp_format, false);
        OutStream <<  ",";
      }
      else
      {
        PrintDecl(OutStream, Decl, pp_format, true);
        if (Terminator  != NULL)
        {
          OutStream <<  Terminator;
        }
        if (Separator  != NULL)
        {
          OutStream <<  Separator;
        }
      }
      Decl = ATAgetFirst(NextDecls);
      NextDecls = NextDecls.tail();
    }
    PrintDecl(OutStream, Decl, pp_format, true);
    if (Terminator  != NULL)
    {
      OutStream <<  Terminator;
    }
  }
}

void PrintDecl(std::ostream& OutStream, const aterm_appl Decl,
                           t_pp_format pp_format, const bool ShowSorts)
{
  PrintPart_Appl(OutStream, ATAgetArgument(Decl, 0),
                             pp_format, ShowSorts, 0);
  if (ShowSorts)
  {
    if (gsIsActId(Decl))
    {
      aterm_list SortExprs = ATLgetArgument(Decl, 1);
      if (SortExprs.size() > 0)
      {
        OutStream <<  ": ";
        PrintPart_List(OutStream, SortExprs,
                                   pp_format, ShowSorts, 2, NULL, " # ");
      }
    }
    else
    {
      OutStream <<  ": ";
      PrintPart_Appl(OutStream, ATAgetArgument(Decl, 1),
                                 pp_format, ShowSorts, 0);
    }
  }
}

static void PrintSortExpr(std::ostream& OutStream,
                                      const aterm_appl SortExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsSortExpr(SortExpr));
  if (gsIsSortId(SortExpr))
  {
    //print sort identifier
    mCRL2log(log::debug2, "pretty printer") << "printing standard sort identifier" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(SortExpr, 0),
                               pp_format, ShowSorts, PrecLevel);
  }
  else if (gsIsSortArrow(SortExpr))
  {
    //print product arrow sort
    mCRL2log(log::debug2, "pretty printer") << "printing product arrow sort" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    PrintPart_List(OutStream, ATLgetArgument(SortExpr, 0),
                               pp_format, ShowSorts, 1, NULL, " # ");
    OutStream <<  " -> ";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (data::sort_list::is_list(data::sort_expression(SortExpr)))
  {
    //print list sort
    mCRL2log(log::debug2, "pretty printer") << "printing list sort" << std::endl;
    OutStream <<  "List(";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (data::sort_set::is_set(data::sort_expression(SortExpr)))
  {
    //print set sort
    mCRL2log(log::debug2, "pretty printer") << "printing set sort" << std::endl;
    OutStream <<  "Set(";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (data::sort_bag::is_bag(data::sort_expression(SortExpr)))
  {
    //print bag sort
    mCRL2log(log::debug2, "pretty printer") << "printing bag sort" << std::endl;
    OutStream <<  "Bag(";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (data::sort_fset::is_fset(data::sort_expression(SortExpr)))
  {
    //print set sort
    mCRL2log(log::debug2, "pretty printer") << "printing set sort" << std::endl;
    OutStream <<  "@FSet(";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (data::sort_fbag::is_fbag(data::sort_expression(SortExpr)))
  {
    //print bag sort
    mCRL2log(log::debug2, "pretty printer") << "printing bag sort" << std::endl;
    OutStream <<  "@FBag(";
    PrintSortExpr(OutStream, ATAgetArgument(SortExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (gsIsSortStruct(SortExpr))
  {
    //print structured sort
    mCRL2log(log::debug2, "pretty printer") << "printing structured sort" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    OutStream <<  "struct ";
    PrintPart_List(OutStream, ATLgetArgument(SortExpr, 0),
                               pp_format, ShowSorts, PrecLevel, NULL, " | ");
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsSortUnknown(SortExpr))
  {
    //print unknown
    mCRL2log(log::debug2, "pretty printer") << "printing unknown" << std::endl;
    OutStream <<  "unknown";
  }
  else if (gsIsSortsPossible(SortExpr))
  {
    //print possible sorts
    mCRL2log(log::debug2, "pretty printer") << "printing possible sorts" << std::endl;
    OutStream <<  "{";
    PrintPart_List(OutStream, ATLgetArgument(SortExpr, 0),
                               pp_format, ShowSorts, 0, NULL, ", ");
    OutStream <<  "}";
  }
}

static
aterm_appl gsGetDataExprHead(aterm_appl DataExpr)
{
  while (gsIsDataAppl(DataExpr))
  {
    DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return DataExpr;
}

static
aterm_list gsGetDataExprArgs(aterm_appl DataExpr)
{
  aterm_list l;
  while (gsIsDataAppl(DataExpr))
  {
    l = ATLgetArgument(DataExpr, 1)+l;
    DataExpr = ATAgetArgument(DataExpr, 0);
  }
  return l;
}

// Copied from data reconstruction
static aterm_appl reconstruct_pos_mult(const aterm_appl PosExpr, std::vector<char>& Mult)
{
  aterm_appl Head = gsGetDataExprHead(PosExpr);
  aterm_list Args = gsGetDataExprArgs(PosExpr);
  if (data::sort_pos::is_c1_function_symbol(data::data_expression(PosExpr)))
  {
    //PosExpr is 1; return Mult
    return data::function_symbol(data::detail::vector_number_to_string(Mult), data::sort_pos::pos());
  }
  else if (data::sort_pos::is_cdub_function_symbol(data::data_expression(Head)))
  {
    //PosExpr is of the form cDub(b,p); return (Mult*2)*v(p) + Mult*v(b)
    aterm_appl BoolArg = ATAelementAt(Args, 0);
    aterm_appl PosArg = ATAelementAt(Args, 1);
    std::vector<char> DoubleMult=Mult;
    data::detail::decimal_number_multiply_by_two(DoubleMult);
    PosArg = reconstruct_pos_mult(data::data_expression(PosArg), DoubleMult);
    if (data::sort_bool::is_false_function_symbol(data::data_expression(BoolArg)))
    {
      //Mult*v(b) = 0
      return PosArg;
    }
    else if (data::sort_bool::is_true_function_symbol(data::data_expression(BoolArg)))
    {
      //Mult*v(b) = Mult
      return data::sort_real::plus(data::data_expression(data::data_expression(PosArg)),
                                   data::function_symbol(data::detail::vector_number_to_string(Mult), data::sort_pos::pos()));
    }
    else if (data::detail::vector_number_to_string(Mult) == "1")
    {
      //Mult*v(b) = v(b)
      return data::sort_real::plus(data::data_expression(PosArg), bool_to_numeric(data::data_expression(BoolArg), data::sort_nat::nat()));
    }
    else
    {
      //Mult*v(b)
      return data::sort_real::plus(data::data_expression(PosArg),
                                   data::sort_real::times(data::function_symbol(data::detail::vector_number_to_string(Mult), data::sort_nat::nat()),
                                       bool_to_numeric(data::data_expression(BoolArg), data::sort_nat::nat())));
    }
  }
  else
  {
    //PosExpr is not a Pos constructor
    if (data::detail::vector_number_to_string(Mult) == "1")
    {
      return PosExpr;
    }
    else
    {
      return data::sort_real::times(data::function_symbol(data::detail::vector_number_to_string(Mult), data::sort_pos::pos()), data::data_expression(PosExpr));
    }
  }
}

static aterm_appl reconstruct_numeric_expression(aterm_appl Part)
{
  if (data::sort_pos::is_c1_function_symbol(data::data_expression(Part)) || data::sort_pos::is_cdub_application(data::data_expression(Part)))
  {
    //  mCRL2log(log::debug) << "Reconstructing implementation of a positive number (" << atermpp::aterm( Part) << ")" << std::endl;
    if (data::sort_pos::is_positive_constant(data::data_expression(Part)))
    {
      std::string positive_value(data::sort_pos::positive_constant_as_string(data::data_expression(Part)));
      Part = data::function_symbol(positive_value, data::sort_pos::pos());
    }
    else
    {
      std::vector< char > number = data::detail::string_to_vector_number("1");
      Part = reconstruct_pos_mult(Part, number);
    }
  }
  else if (data::sort_nat::is_c0_function_symbol(data::data_expression(Part)))
  {
    //    mCRL2log(log::debug) << "Reconstructing implementation of " << atermpp::aterm( Part) << std::endl;
    Part = data::function_symbol("0", data::sort_nat::nat());
  }
  else if ((data::sort_nat::is_cnat_application(data::data_expression(Part)) || data::sort_nat::is_pos2nat_application(data::data_expression(Part)))
           && (data::sort_pos::is_pos(data::data_expression(ATAgetFirst(ATLgetArgument(Part, 1))).sort())))
  {
    //    mCRL2log(log::debug) << "Reconstructing implementation of CNat or Pos2Nat (" << atermpp::aterm( Part) << ")" << std::endl;
    aterm_appl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_numeric_expression(value);
    Part = data::sort_nat::pos2nat(data::data_expression(value));
    if (gsIsOpId(value))
    {
      aterm_appl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name)))
      {
        Part = data::function_symbol(atermpp::aterm_string(name), data::sort_nat::nat());
      }
    }
  }
  else if (data::sort_nat::is_cpair_application(data::data_expression(Part)))
  {
    //    mCRL2log(log::debug) << "Currently not reconstructing implementation of CPair (" << atermpp::aterm( Part) << ")" << std::endl;
  }
  else if (data::sort_int::is_cneg_application(data::data_expression(Part)))
  {
    //    mCRL2log(log::debug) << "Reconstructing implementation of CNeg (" << atermpp::aterm( Part) << ")" << std::endl;
    Part = data::sort_int::negate(data::data_expression(ATAgetFirst(ATLgetArgument(Part, 1))));
  }
  else if ((data::sort_int::is_cint_application(data::data_expression(Part)) || data::sort_int::is_nat2int_application(data::data_expression(Part)))
           && (data::sort_nat::is_nat(data::data_expression(ATAgetFirst(ATLgetArgument(Part, 1))).sort())))
  {
    //    mCRL2log(log::debug) << "Reconstructing implementation of CInt or Nat2Int (" << atermpp::aterm( Part) << ")" << std::endl;
    aterm_appl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_numeric_expression(value);
    Part = data::sort_int::nat2int(data::data_expression(value));
    if (gsIsOpId(value))
    {
      aterm_appl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name)))
      {
        Part = data::function_symbol(atermpp::aterm_string(name), data::sort_int::int_());
      }
    }
  }
  else if (data::sort_real::is_int2real_application(data::data_expression(Part))
           && (data::sort_int::is_int(data::data_expression(ATAgetFirst(ATLgetArgument(Part, 1))).sort())))
  {
    //    mCRL2log(log::debug) << "Reconstructing implementation of Int2Real (" << atermpp::aterm( Part) << ")" << std::endl;
    aterm_appl value = ATAgetFirst(ATLgetArgument(Part, 1));
    value = reconstruct_numeric_expression(value);
    Part = data::sort_real::int2real(data::data_expression(value));
    if (gsIsOpId(value))
    {
      aterm_appl name = ATAgetArgument(value, 0);
      if (gsIsNumericString(gsATermAppl2String(name)))
      {
        Part = data::function_symbol(atermpp::aterm_string(name), data::sort_real::real_());
      }
    }
  }
  else if (data::sort_real::is_creal_application(data::data_expression(Part)))
  {
//    mCRL2log(log::debug) << "Reconstructing implementation of CReal (" << atermpp::aterm( Part) << ")" << std::endl;
    aterm_list Args = ATLgetArgument(Part, 1);
    aterm_appl ArgNumerator = reconstruct_numeric_expression(ATAelementAt(Args, 0));
    aterm_appl ArgDenominator = reconstruct_numeric_expression(ATAelementAt(Args, 1));
    if (ArgDenominator==static_cast<aterm_appl>(data::function_symbol("1", data::sort_pos::pos())))
    {
      Part = data::sort_real::int2real(data::data_expression(ArgNumerator));
      if (gsIsOpId(ArgNumerator))
      {
        aterm_appl name = ATAgetArgument(ArgNumerator, 0);
        if (gsIsNumericString(gsATermAppl2String(name)))
        {
          Part = data::function_symbol(atermpp::aterm_string(name), data::sort_real::real_());
        }
      }
    }
    else
    {
      Part = data::sort_real::divides(data::data_expression(ArgNumerator),
                                      data::sort_int::pos2int(data::data_expression(ArgDenominator)));
      if (gsIsOpId(ArgDenominator))
      {
        aterm_appl name = ATAgetArgument(ArgDenominator, 0);
        if (gsIsNumericString(gsATermAppl2String(name)))
        {
          Part = data::sort_real::divides(data::data_expression(ArgNumerator),
                                          data::function_symbol(atermpp::aterm_string(name), data::sort_int::int_()));
        }
      }
    }
  }
  else if (data::sort_real::is_reduce_fraction_where_application(data::data_expression(Part)))
  {
    data::data_expression e(Part);
    Part = data::sort_real::plus(data::sort_real::int2real(data::sort_real::arg2(e)),
                                 data::sort_real::divides(data::sort_real::arg3(e),
                                                          data::sort_real::arg1(e)));
  }
  return Part;
}

static aterm_appl
reconstruct_container_expression(aterm_appl Part)
{
  using namespace mcrl2::data;

  if (!gsIsDataAppl(Part) && !gsIsOpId(Part))
  {
    return Part;
  }

  data_expression expr(Part);
  if (sort_list::is_cons_application(expr))
  {
    data_expression_vector elements;
    while (sort_list::is_cons_application(expr))
    {
      elements.push_back(sort_list::left(expr));
      expr = sort_list::right(expr);
    }

    if (sort_list::is_empty_function_symbol(expr))
    {
      Part = sort_list::list_enumeration(expr.sort(), elements);
    }
  }
  else if (sort_list::is_snoc_application(expr))
  {
    data_expression_vector elements;
    while (sort_list::is_snoc_application(expr))
    {
      elements.insert(elements.begin(), sort_list::right(expr));
      expr = sort_list::left(expr);
    }

    if (sort_list::is_empty_function_symbol(expr))
    {
      Part = sort_list::list_enumeration(expr.sort(), elements);
    }
  }
  else if (sort_set::is_constructor_application(expr))
  {
    //mCRL2log(log::debug) << "Reconstructing implementation of set comprehension" << std::endl;
    //part is an internal set representation;
    //replace by a finite set to set conversion or a set comprehension.
    sort_expression element_sort(*function_sort(sort_set::left(expr).sort()).domain().begin());
    if (sort_set::is_false_function_function_symbol(sort_set::left(expr)))
    {
      Part = reconstruct_container_expression(static_cast<aterm_appl>(sort_set::set_fset(element_sort, sort_set::right(expr))));
    }
    else if (sort_set::is_true_function_function_symbol(sort_set::left(expr)))
    {
      Part = static_cast<aterm_appl>(sort_set::complement(element_sort, sort_set::set_fset(element_sort, sort_set::right(expr))));
    }
    else
    {
      aterm_appl se_func = sort_set::left(expr).sort();
      aterm_appl se_func_dom = ATAgetFirst(ATLgetArgument(se_func, 0));
      aterm_appl var = gsMakeDataVarId(gsFreshString2ATermAppl("x",
                                      static_cast<aterm_appl>(expr), true), se_func_dom);
      aterm_appl body;
      if (data::sort_fset::is_empty_function_symbol(sort_set::right(expr)))
      {
        body = sort_set::left(expr)(data::variable(var));

        if (is_lambda(sort_set::left(expr)))
        {
          data::lambda left(sort_set::left(expr));
          data::variable_list vars = left.variables();
          if (vars.size() == 1)
          {
            var = *(vars.begin());
            body = left.body();
          }
        }
      }
      else
      {
        data_expression lhs(sort_set::left(expr)(data::variable(var)));
        data_expression rhs(sort_set::in(element_sort, data_expression(var), sort_set::set_fset(element_sort, sort_set::right(expr))));
        body = static_cast<aterm_appl>(data::not_equal_to(lhs,rhs));
      }
      Part = gsMakeBinder(gsMakeSetComp(), make_list<aterm>(var), body);
    }
  }
  else if (sort_set::is_set_fset_application(expr))
  {
    //mCRL2log(log::debug) << "Reconstructing SetFSet" << std::endl;
    //try to reconstruct Part as the empty set or as a set enumeration
    data_expression de_fset(sort_set::arg(expr));
    data_expression result(reconstruct_container_expression(de_fset));
    if (sort_set::is_set_enumeration_application(result) || sort_set::is_set_enumeration_function_symbol(result))
    {
      Part = static_cast<aterm_appl>(result);
    }
  }
  else if (sort_fset::is_empty_function_symbol(expr))
  {
    Part = static_cast<aterm_appl>(sort_set::set_enumeration(container_sort(expr.sort()).element_sort(), data_expression_vector()));
  }
  else if (sort_fset::is_cons_application(expr) || sort_fset::is_insert_application(expr))
  {
    data_expression de_fset(expr);
    bool elts_is_consistent = true;
    data_expression_vector elements;
    while (!sort_fset::is_empty_function_symbol(de_fset) && elts_is_consistent)
    {
      if (sort_fset::is_cons_application(de_fset))
      {
        elements.push_back(sort_fset::left(de_fset));
        de_fset = sort_fset::right(de_fset);
      }
      else if (sort_fset::is_insert_application(de_fset))
      {
        elements.push_back(sort_fset::left(de_fset));
        de_fset = sort_fset::right(de_fset);
      }
      else
      {
        elts_is_consistent = false;
      }
    }
    if (elts_is_consistent)
    {
      Part = static_cast<aterm_appl>(sort_set::set_enumeration(expr.sort(), elements));
    }
  }
  else if (sort_set::is_set_comprehension_application(expr))
  {
    data_expression body(sort_set::arg(expr));
    data_expression_vector variables;
    sort_expression_list domain_of_body_sort(function_sort(body.sort()).domain());
    data_expression_list context = atermpp::make_list(body);

    for (sort_expression_list::const_iterator i = domain_of_body_sort.begin();
         i != domain_of_body_sort.end(); ++i)
    {
      variable var = data::variable(gsMakeDataVarId(gsFreshString2ATermAppl("x",
                                    static_cast<aterm_list>(context), true), static_cast<aterm_appl>(*i)));
      context.push_front(data_expression(var));
      variables.push_back(var);
    }

    body = data::application(body, variables);
    Part = gsMakeBinder(gsMakeSetComp(), data_expression_list(variables.begin(),variables.end()), body);
  }

  else if (sort_bag::is_constructor_application(expr))
  {
    //part is an internal set representation;
    //replace by a finite set to set conversion or a set comprehension.
    sort_expression element_sort = *function_sort(sort_bag::left(expr).sort()).domain().begin();
    if (sort_bag::is_zero_function_function_symbol(sort_bag::left(expr)))
    {
      Part = reconstruct_container_expression(static_cast<aterm_appl>(sort_bag::bag_fbag(element_sort, sort_bag::right(expr))));
    }
    else
    {
      aterm_appl se_func = sort_bag::left(expr).sort();
      aterm_appl se_func_dom = ATAgetFirst(ATLgetArgument(se_func, 0));
      data_expression var(gsMakeDataVarId(gsFreshString2ATermAppl("x",
                                          static_cast<aterm_appl>(expr), true), se_func_dom));
      data_expression body;

      if (sort_bag::is_one_function_function_symbol(sort_bag::left(expr)))
      {
        body = number(sort_nat::nat(), "1");
      }
      else
      {
        body = sort_bag::left(expr)(var);

        if (is_lambda(sort_bag::left(expr)))
        {
          data::lambda left(sort_bag::left(expr));
          data::variable_list vars = left.variables();
          if (vars.size() == 1)
          {
            var = *(vars.begin());
            body = left.body();
          }
        }
      }
      if (!sort_fbag::is_empty_function_symbol(sort_bag::right(expr)))
      {
        body = sort_nat::swap_zero(body, sort_bag::count(element_sort, var, sort_bag::bag_fbag(element_sort, sort_bag::right(expr))));
      }
      Part = gsMakeBinder(gsMakeBagComp(), make_list(var), body);
    }
  }
  else if (sort_bag::is_bag_fbag_application(expr))
  {
    //mCRL2log(log::debug) << "Reconstructing BagFBag" << std::endl;
    //try to reconstruct Part as the empty bag or as a bag enumeration
    data_expression de_fbag(sort_bag::arg(expr));
    data_expression result(reconstruct_container_expression(de_fbag));
    if (sort_bag::is_bag_enumeration_application(result) || sort_bag::is_bag_enumeration_function_symbol(result))
    {
      Part = static_cast<aterm_appl>(result);
    }
  }
  else if (sort_fbag::is_empty_function_symbol(expr))
  {
    Part = static_cast<aterm_appl>(sort_bag::bag_enumeration(container_sort(expr.sort()).element_sort(), data_expression_vector()));
  }
  else if (sort_fbag::is_cons_application(expr) || sort_fbag::is_insert_application(expr) || sort_fbag::is_cinsert_application(expr))
  {
    data_expression de_fbag(expr);
    bool elts_is_consistent = true;
    data_expression_vector elements;
    while (!sort_fbag::is_empty_function_symbol(de_fbag) && elts_is_consistent)
    {
      if (sort_fbag::is_cons_application(de_fbag))
      {
        elements.push_back(sort_fbag::arg1(de_fbag));
        elements.push_back(sort_fbag::arg2(de_fbag));
        de_fbag = sort_fbag::arg3(de_fbag);
      }
      else if (sort_fbag::is_insert_application(de_fbag))
      {
        elements.push_back(sort_fbag::arg1(de_fbag));
        elements.push_back(sort_nat::cnat(sort_fbag::arg2(de_fbag)));
        de_fbag = sort_fbag::arg3(de_fbag);
      }
      else if (sort_fbag::is_cinsert_application(de_fbag))
      {
        elements.push_back(sort_fbag::arg1(de_fbag));
        elements.push_back(sort_fbag::arg2(de_fbag));
        de_fbag = sort_fbag::arg3(de_fbag);
      }
      else
      {
        elts_is_consistent = false;
      }
    }
    if (elts_is_consistent)
    {
      Part = static_cast<aterm_appl>(sort_bag::bag_enumeration(container_sort(expr.sort()).element_sort(), elements));
    }
  }
  else if (sort_bag::is_bag_comprehension_application(expr))
  {
    data_expression body(sort_bag::arg(expr));
    data_expression_vector variables;
    sort_expression_list domain_of_body_sort(function_sort(body.sort()).domain());
    data_expression_list context = atermpp::make_list(body);

    for (sort_expression_list::const_iterator i = domain_of_body_sort.begin();
         i != domain_of_body_sort.end(); ++i)
    {
      variable var = data::variable(gsMakeDataVarId(gsFreshString2ATermAppl("x",
                                    static_cast<aterm_list>(context), true), static_cast<aterm_appl>(*i)));
      context.push_front(data_expression(var));
      variables.push_back(var);
    }

    body = data::application(body, variables);
    Part = gsMakeBinder(gsMakeBagComp(), data_expression_list(variables.begin(),variables.end()), body);
  }
  return Part;
}

void PrintDataExpr(std::ostream& OutStream,
                               aterm_appl DataExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsDataExpr(DataExpr));
  if (gsIsId(DataExpr) || gsIsOpId(DataExpr) || gsIsDataVarId(DataExpr) ||
      gsIsDataAppl(DataExpr))
  {
    if (pp_format == ppDebug)
    {
      PrintPart_Appl(OutStream, ATAgetArgument(DataExpr, 0),
                                 pp_format, ShowSorts, 0);
      if (gsIsDataAppl(DataExpr))
      {
        OutStream <<  "(";
        //gsIsDataAppl(DataExpr)
        PrintPart_List(OutStream, ATLgetArgument(DataExpr, 1),
                                   pp_format, ShowSorts, 0, NULL, ", ");
        OutStream <<  ")";
      }
    }
    else     //pp_format == ppDefault
    {
      //print data expression in the external format, if possible
      DataExpr = reconstruct_container_expression(DataExpr);
      aterm_appl Head;
      aterm_list Args;
      if (!gsIsDataAppl(DataExpr))
      {
        Head = DataExpr;
        Args = aterm_list();
      }
      else
      {
        Head = ATAgetArgument(DataExpr, 0);
        Args = ATLgetArgument(DataExpr, 1);
      }
      size_t ArgsLength = Args.size();
      if (gsIsBinder(Head) && Args == aterm_list())
      {
        // A binder could be introduced by reconstructing a container expression
        // just print recursively.
        PrintDataExpr(OutStream, Head,
                                  pp_format, ShowSorts, PrecLevel);
      }
      else if (gsIsOpIdNumericUpCast(Head) && ArgsLength == 1)
      {
        //print upcast expression
        mCRL2log(log::debug2, "pretty printer") << "printing upcast expression" << std::endl;
        PrintDataExpr(OutStream, ATAelementAt(Args, 0),
                                  pp_format, ShowSorts, PrecLevel);
      }
      else if (gsIsIdListEnum(Head))
      {
        //print list enumeration
        mCRL2log(log::debug2, "pretty printer") << "printing list enumeration" << std::endl;
        OutStream <<  "[";
        PrintPart_List(OutStream, Args,
                                   pp_format, ShowSorts, 0, NULL, ", ");
        OutStream <<  "]";
      }
      else if (gsIsIdSetEnum(Head))
      {
        //print set enumeration
        mCRL2log(log::debug2, "pretty printer") << "printing set enumeration" << std::endl;
        OutStream <<  "{";
        PrintPart_List(OutStream, Args,
                                   pp_format, ShowSorts, 0, NULL, ", ");
        OutStream <<  "}";
      }
      else if (gsIsIdBagEnum(Head))
      {
        //print bag enumeration
        OutStream <<  "{";
        PrintPart_BagEnum(OutStream, Args,
                                      pp_format, ShowSorts, 0, NULL, ", ");
        OutStream <<  "}";
      }
      else if (gsIsIdFuncUpdate(Head) && Args != aterm_list())
      {
        //print function update
        mCRL2log(log::debug2, "pretty printer") << "printing function update" << std::endl;
        assert(Args.size()==3);
        PrintDataExpr(OutStream, ATAelementAt(Args, 0),
                                  pp_format, ShowSorts, gsPrecIdPrefix());
        OutStream <<  "[";
        PrintDataExpr(OutStream, ATAelementAt(Args, 1),
                                  pp_format, ShowSorts, 0);
        OutStream <<  " -> ";
        PrintDataExpr(OutStream, ATAelementAt(Args, 2),
                                  pp_format, ShowSorts, 0);
        OutStream <<  "]";
      }
      else if (gsIsIdPrefix(Head, ArgsLength))
      {
        //print prefix expression
        mCRL2log(log::debug2, "pretty printer") << "printing prefix expression" << std::endl;
        PrintPart_Appl(OutStream, Head,
                                   pp_format, ShowSorts, PrecLevel);
        PrintDataExpr(OutStream, ATAelementAt(Args, 0),
                                  pp_format, ShowSorts, gsPrecIdPrefix());
      }
      else if (gsIsIdInfix(Head, ArgsLength))
      {
        //print infix expression
        mCRL2log(log::debug2, "pretty printer") << "printing infix expression" << std::endl;
        aterm_appl HeadName = ATAgetArgument(Head, 0);
        if (PrecLevel > gsPrecIdInfix(HeadName))
        {
          OutStream <<  "(";
        }
        PrintDataExpr(OutStream, ATAelementAt(Args, 0),
                                  pp_format, ShowSorts, gsPrecIdInfixLeft(HeadName));
        OutStream <<  " ";
        PrintPart_Appl(OutStream, Head, pp_format, ShowSorts, PrecLevel);
        OutStream <<  " ";
        PrintDataExpr(OutStream, ATAelementAt(Args, 1),
                                  pp_format, ShowSorts, gsPrecIdInfixRight(HeadName));
        if (PrecLevel > gsPrecIdInfix(HeadName))
        {
          OutStream <<  ")";
        }
      }
      else if (gsIsId(DataExpr))
      {
        //print untyped data variable or operation identifier
        mCRL2log(log::debug2, "pretty printer") << "printing untyped data variable or operation identifier" << std::endl;
        PrintPart_Appl(OutStream, ATAgetArgument(DataExpr, 0),
                                   pp_format, ShowSorts, PrecLevel);
      }
      else if (gsIsOpId(DataExpr) || gsIsDataVarId(DataExpr))
      {
        aterm_appl Reconstructed(reconstruct_numeric_expression(DataExpr));
        //print data variable or operation identifier
        if (Reconstructed == DataExpr)
        {
          mCRL2log(log::debug2, "pretty printer") << "printing data variable or operation identifier" << std::endl;
          PrintPart_Appl(OutStream, ATAgetArgument(DataExpr, 0),
                                     pp_format, ShowSorts, PrecLevel);
          if (ShowSorts)
          {
            OutStream <<  ": ";
            PrintPart_Appl(OutStream, ATAgetArgument(DataExpr, 1),
                                       pp_format, ShowSorts, 0);
          }
        }
        else
        {
          PrintDataExpr(OutStream, Reconstructed, pp_format, ShowSorts, PrecLevel);
        }
      }
      else
      {
        aterm_appl Reconstructed(reconstruct_numeric_expression(DataExpr));
        if (Reconstructed == DataExpr)
        {
          //print data application
          mCRL2log(log::debug2, "pretty printer") << "printing data application" << std::endl;
          PrintDataExpr(OutStream, Head, pp_format, ShowSorts, gsPrecIdPrefix());
          OutStream <<  "(";
          PrintPart_List(OutStream, Args, pp_format, ShowSorts, 0, NULL, ", ");
          OutStream <<  ")";
        }
        else
        {
          mCRL2log(log::debug2, "pretty printer") << "printing numeric representation" << std::endl;
          PrintDataExpr(OutStream, Reconstructed, pp_format, ShowSorts, PrecLevel);
        }
      }
    }
  }
  else if (gsIsBinder(DataExpr))
  {
    mCRL2log(log::debug2, "pretty printer") << "printing binder" << std::endl;
    aterm_appl BindingOperator = ATAgetArgument(DataExpr, 0);
    if (gsIsSetBagComp(BindingOperator) || gsIsSetComp(BindingOperator)
        || gsIsBagComp(BindingOperator))
    {
      //print set/bag comprehension
      mCRL2log(log::debug2, "pretty printer") << "printing set/bag comprehension" << std::endl;
      OutStream <<  "{ ";
      assert(!(ATLgetArgument(DataExpr,1)).empty());
      PrintDecls(OutStream, ATLgetArgument(DataExpr, 1),
                             pp_format, NULL, ", ");
      OutStream <<  " | ";
      PrintDataExpr(OutStream, ATAgetArgument(DataExpr, 2),
                                pp_format, ShowSorts, 0);
      OutStream <<  " }";
    }
    else if (gsIsLambda(BindingOperator) || gsIsForall(BindingOperator)
             || gsIsExists(BindingOperator))
    {
      //print lambda abstraction or universal/existential quantification
      mCRL2log(log::debug2, "pretty printer") << "printing lambda abstraction or universal/existential quantification" << std::endl;
      if (PrecLevel > 1)
      {
        OutStream <<  "(";
      }
      OutStream << (gsIsLambda(BindingOperator)?"lambda ":
                                      (gsIsForall(BindingOperator))?"forall ":
                                      "exists ");
      PrintDecls(OutStream, ATLgetArgument(DataExpr, 1),
                             pp_format, NULL, ", ");
      OutStream <<  ". ";
      PrintPart_Appl(OutStream, ATAgetArgument(DataExpr, 2),
                                 pp_format, ShowSorts, 1);
      if (PrecLevel > 1)
      {
        OutStream <<  ")";
      }
    }
  }
  else if (gsIsWhr(DataExpr))
  {
    //print where clause
    mCRL2log(log::debug2, "pretty printer") << "printing where clause" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    PrintDataExpr(OutStream, ATAgetArgument(DataExpr, 0),
                              pp_format, ShowSorts, 0);
    OutStream <<  " whr ";
    PrintPart_List(OutStream, ATLgetArgument(DataExpr, 1),
                               pp_format, ShowSorts, PrecLevel, NULL, ", ");
    OutStream <<  " end";
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
}

static void PrintProcExpr(std::ostream& OutStream,
                                      const aterm_appl ProcExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsProcExpr(ProcExpr));
  if (gsIsParamId(ProcExpr) || gsIsAction(ProcExpr) || gsIsProcess(ProcExpr))
  {
    //print action or process reference
    mCRL2log(log::debug2, "pretty printer") << "printing action or process reference" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(ProcExpr, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list Args = ATLgetArgument(ProcExpr, 1);
    if (Args.size() > 0)
    {
      OutStream <<  "(";
      PrintPart_List(OutStream, Args,
                                 pp_format, ShowSorts, 0, NULL, ", ");
      OutStream <<  ")";
    }
  }
  else if (gsIsIdAssignment(ProcExpr) || gsIsProcessAssignment(ProcExpr))
  {
    //print process assignment
    mCRL2log(log::debug2, "pretty printer") << "printing process assignment" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(ProcExpr, 0),
                               pp_format, ShowSorts, PrecLevel);
    OutStream <<  "(";
    PrintPart_List(OutStream, ATLgetArgument(ProcExpr, 1),
                               pp_format, ShowSorts, 0, NULL, ", ");
    OutStream <<  ")";
  }
  else if (gsIsDelta(ProcExpr))
  {
    //print delta
    mCRL2log(log::debug2, "pretty printer") << "printing delta" << std::endl;
    OutStream <<  "delta";
  }
  else if (gsIsTau(ProcExpr))
  {
    //print tau
    mCRL2log(log::debug2, "pretty printer") << "printing tau" << std::endl;
    OutStream <<  "tau";
  }
  else if (gsIsChoice(ProcExpr))
  {
    //print choice
    mCRL2log(log::debug2, "pretty printer") << "printing choice" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 1);
    OutStream <<  " + ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsSum(ProcExpr))
  {
    //print summation
    mCRL2log(log::debug2, "pretty printer") << "printing summation" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    OutStream <<  "sum ";
    PrintDecls(OutStream, ATLgetArgument(ProcExpr, 0),
                           pp_format, NULL, ", ");
    OutStream <<  ". ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 1);
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsMerge(ProcExpr) || gsIsLMerge(ProcExpr))
  {
    //print merge or left merge
    mCRL2log(log::debug2, "pretty printer") << "printing merge or left merge" << std::endl;
    if (PrecLevel > 2)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 3);
    if (gsIsMerge(ProcExpr))
    {
      OutStream <<  " || ";
    }
    else
    {
      OutStream <<  " ||_ ";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 2);
    if (PrecLevel > 2)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsBInit(ProcExpr))
  {
    //print bounded initialisation
    mCRL2log(log::debug2, "pretty printer") << "printing bounded initialisation" << std::endl;
    if (PrecLevel > 3)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 4);
    OutStream <<  " << ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 3);
    if (PrecLevel > 3)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsIfThen(ProcExpr))
  {
    //print if then
    mCRL2log(log::debug2, "pretty printer") << "printing if then" << std::endl;
    if (PrecLevel > 4)
    {
      OutStream <<  "(";
    }
    PrintDataExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, gsPrecIdPrefix());
    OutStream <<  " -> ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 5);
    if (PrecLevel > 4)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsIfThenElse(ProcExpr))
  {
    //print if then else
    mCRL2log(log::debug2, "pretty printer") << "printing if then else" << std::endl;
    if (PrecLevel > 4)
    {
      OutStream <<  "(";
    }
    PrintDataExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, gsPrecIdPrefix());
    OutStream <<  " -> ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 5);
    aterm_appl ProcExprElse = ATAgetArgument(ProcExpr, 2);
    OutStream <<  " <> ";
    PrintProcExpr(OutStream, ProcExprElse,
                              pp_format, ShowSorts, 5);
    if (PrecLevel > 4)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsSeq(ProcExpr))
  {
    //print sequential composition
    mCRL2log(log::debug2, "pretty printer") << "printing sequential composition" << std::endl;
    if (PrecLevel > 5)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 6);
    OutStream <<  " . ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 5);
    if (PrecLevel > 5)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsAtTime(ProcExpr))
  {
    //print at expression
    mCRL2log(log::debug2, "pretty printer") << "printing at expression" << std::endl;
    if (PrecLevel > 6)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 6);
    OutStream <<  " @ ";
    PrintDataExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, gsPrecIdPrefix());
    if (PrecLevel > 6)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsSync(ProcExpr))
  {
    //print sync
    mCRL2log(log::debug2, "pretty printer") << "printing sync" << std::endl;
    if (PrecLevel > 7)
    {
      OutStream <<  "(";
    }
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 0),
                              pp_format, ShowSorts, 8);
    OutStream <<  " | ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 7);
    if (PrecLevel > 7)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsBlock(ProcExpr) || gsIsHide(ProcExpr) || gsIsRename(ProcExpr) ||
           gsIsComm(ProcExpr) || gsIsAllow(ProcExpr))
  {
    //print process quantification
    mCRL2log(log::debug2, "pretty printer") << "printing process quantification" << std::endl;
    if (gsIsBlock(ProcExpr))
    {
      OutStream <<  "block";
    }
    else if (gsIsHide(ProcExpr))
    {
      OutStream <<  "hide";
    }
    else if (gsIsRename(ProcExpr))
    {
      OutStream <<  "rename";
    }
    else if (gsIsComm(ProcExpr))
    {
      OutStream <<  "comm";
    }
    else
    {
      OutStream <<  "allow";
    }
    OutStream <<  "({";
    PrintPart_List(OutStream, ATLgetArgument(ProcExpr, 0),
                               pp_format, ShowSorts, 0, NULL, ", ");
    OutStream <<  "}, ";
    PrintProcExpr(OutStream, ATAgetArgument(ProcExpr, 1),
                              pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
}

static void PrintStateFrm(std::ostream& OutStream,
                                      const aterm_appl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsStateFrm(StateFrm));
  if (gsIsDataExpr(StateFrm))
  {
    //print data expression
    mCRL2log(log::debug2, "pretty printer") << "printing data expression" << std::endl;
    OutStream <<  "val(";
    PrintDataExpr(OutStream, StateFrm, pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (gsIsStateTrue(StateFrm))
  {
    //print true
    mCRL2log(log::debug2, "pretty printer") << "printing true" << std::endl;
    OutStream <<  "true";
  }
  else if (gsIsStateFalse(StateFrm))
  {
    //print false
    mCRL2log(log::debug2, "pretty printer") << "printing false" << std::endl;
    OutStream <<  "false";
  }
  else if (gsIsStateYaled(StateFrm))
  {
    //print yaled
    mCRL2log(log::debug2, "pretty printer") << "printing yaled" << std::endl;
    OutStream <<  "yaled";
  }
  else if (gsIsStateDelay(StateFrm))
  {
    //print delay
    mCRL2log(log::debug2, "pretty printer") << "printing delay" << std::endl;
    OutStream <<  "delay";
  }
  else if (gsIsStateYaledTimed(StateFrm))
  {
    //print timed yaled
    mCRL2log(log::debug2, "pretty printer") << "printing timed yaled" << std::endl;
    OutStream <<  "yaled @ ";
    PrintDataExpr(OutStream, ATAgetArgument(StateFrm, 0),
                              pp_format, ShowSorts, gsPrecIdPrefix());
  }
  else if (gsIsStateDelayTimed(StateFrm))
  {
    //print timed delay
    mCRL2log(log::debug2, "pretty printer") << "printing timed delay" << std::endl;
    OutStream <<  "delay @ ";
    PrintDataExpr(OutStream, ATAgetArgument(StateFrm, 0),
                              pp_format, ShowSorts, gsPrecIdPrefix());
  }
  else if (gsIsStateVar(StateFrm))
  {
    //print fixpoint variable
    mCRL2log(log::debug2, "pretty printer") << "printing fixpoint variable" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(StateFrm, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list Args = ATLgetArgument(StateFrm, 1);
    if (Args.size() > 0)
    {
      OutStream <<  "(";
      PrintPart_List(OutStream, Args,
                                 pp_format, ShowSorts, 0, NULL, ", ");
      OutStream <<  ")";
    }
  }
  else if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    //print quantification
    mCRL2log(log::debug2, "pretty printer") << "printing quantification" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    if (gsIsStateForall(StateFrm))
    {
      OutStream <<  "forall ";
    }
    else
    {
      OutStream <<  "exists ";
    }
    PrintDecls(OutStream, ATLgetArgument(StateFrm, 0),
                           pp_format, NULL, ", ");
    OutStream <<  ". ";
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 1),
                              pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    //print fixpoint quantification
    mCRL2log(log::debug2, "pretty printer") << "printing fixpoint quantification" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    if (gsIsStateNu(StateFrm))
    {
      OutStream <<  "nu ";
    }
    else
    {
      OutStream <<  "mu ";
    }
    PrintPart_Appl(OutStream, ATAgetArgument(StateFrm, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list DataVarInits = ATLgetArgument(StateFrm, 1);
    if (DataVarInits.size() > 0)
    {
      OutStream <<  "(";
      PrintPart_List(OutStream, DataVarInits,
                                 pp_format, ShowSorts, 0, NULL, ", ");
      OutStream <<  ")";
    }
    OutStream <<  ". ";
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 2),
                              pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsStateImp(StateFrm))
  {
    //print implication
    mCRL2log(log::debug2, "pretty printer") << "printing implication" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 0),
                              pp_format, ShowSorts, 2);
    OutStream <<  " => ";
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 1),
                              pp_format, ShowSorts, 1);
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm))
  {
    //print conjunction or disjunction
    mCRL2log(log::debug2, "pretty printer") << "printing conjunction or disjunction" << std::endl;
    if (PrecLevel > 2)
    {
      OutStream <<  "(";
    }
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 0),
                              pp_format, ShowSorts, 3);
    if (gsIsStateAnd(StateFrm))
    {
      OutStream <<  " && ";
    }
    else
    {
      OutStream <<  " || ";
    }
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 1),
                              pp_format, ShowSorts, 2);
    if (PrecLevel > 2)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsStateNot(StateFrm))
  {
    //print negation
    mCRL2log(log::debug2, "pretty printer") << "printing negation" << std::endl;
    if (PrecLevel > 3)
    {
      OutStream <<  "(";
    }
    OutStream <<  "!";
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 0),
                              pp_format, ShowSorts, 3);
    if (PrecLevel > 3)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm))
  {
    //print must or may
    mCRL2log(log::debug2, "pretty printer") << "printing must or may" << std::endl;
    if (PrecLevel > 3)
    {
      OutStream <<  "(";
    }
    if (gsIsStateMust(StateFrm))
    {
      OutStream <<  "[";
    }
    else
    {
      OutStream <<  "<";
    }
    PrintRegFrm(OutStream, ATAgetArgument(StateFrm, 0),
                            pp_format, ShowSorts, 0);
    if (gsIsStateMust(StateFrm))
    {
      OutStream <<  "]";
    }
    else
    {
      OutStream <<  ">";
    }
    PrintStateFrm(OutStream, ATAgetArgument(StateFrm, 1),
                              pp_format, ShowSorts, 3);
    if (PrecLevel > 3)
    {
      OutStream <<  ")";
    }
  }
}

static void PrintRegFrm(std::ostream& OutStream,
                                    const aterm_appl RegFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsRegFrm(RegFrm));
  if (gsIsActFrm(RegFrm))
  {
    //print regular formula
    mCRL2log(log::debug2, "pretty printer") << "printing action formula" << std::endl;
    PrintActFrm(OutStream, RegFrm, pp_format, ShowSorts, 0);
  }
  else if (gsIsRegNil(RegFrm))
  {
    //print nil
    mCRL2log(log::debug2, "pretty printer") << "printing nil" << std::endl;
    OutStream <<  "nil";
  }
  else if (gsIsRegAlt(RegFrm))
  {
    //print alternative composition
    mCRL2log(log::debug2, "pretty printer") << "printing alternative composition" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    PrintRegFrm(OutStream, ATAgetArgument(RegFrm, 0),
                            pp_format, ShowSorts, 1);
    OutStream <<  " + ";
    PrintRegFrm(OutStream, ATAgetArgument(RegFrm, 1),
                            pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsRegSeq(RegFrm))
  {
    //print sequential composition
    mCRL2log(log::debug2, "pretty printer") << "printing sequential composition" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    PrintRegFrm(OutStream, ATAgetArgument(RegFrm, 0),
                            pp_format, ShowSorts, 2);
    OutStream <<  " . ";
    PrintRegFrm(OutStream, ATAgetArgument(RegFrm, 1),
                            pp_format, ShowSorts, 1);
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsRegTransOrNil(RegFrm) || gsIsRegTrans(RegFrm))
  {
    //print (reflexive) transitive closure
    mCRL2log(log::debug2, "pretty printer") << "printing (reflexive) transitive closure" << std::endl;
    if (PrecLevel > 2)
    {
      OutStream <<  "(";
    }
    PrintRegFrm(OutStream, ATAgetArgument(RegFrm, 0),
                            pp_format, ShowSorts, 2);
    if (gsIsRegTransOrNil(RegFrm))
    {
      OutStream <<  "*";
    }
    else
    {
      OutStream <<  "+";
    }
    if (PrecLevel > 2)
    {
      OutStream <<  ")";
    }
  }
}

static void PrintActFrm(std::ostream& OutStream,
                                    const aterm_appl ActFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsActFrm(ActFrm));
  if (gsIsDataExpr(ActFrm))
  {
    //print data expression
    mCRL2log(log::debug2, "pretty printer") << "printing data expression" << std::endl;
    OutStream <<  "val(";
    PrintDataExpr(OutStream, ActFrm, pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (gsIsMultAct(ActFrm))
  {
    //print multiaction
    PrintPart_Appl(OutStream, ActFrm, pp_format, ShowSorts, 0);
  }
  else if (gsIsActTrue(ActFrm))
  {
    //print true
    mCRL2log(log::debug2, "pretty printer") << "printing true" << std::endl;
    OutStream <<  "true";
  }
  else if (gsIsActFalse(ActFrm))
  {
    //print false
    mCRL2log(log::debug2, "pretty printer") << "printing false" << std::endl;
    OutStream <<  "false";
  }
  else if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    //print quantification
    mCRL2log(log::debug2, "pretty printer") << "printing quantification" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    if (gsIsActForall(ActFrm))
    {
      OutStream <<  "forall ";
    }
    else
    {
      OutStream <<  "exists ";
    }
    PrintDecls(OutStream, ATLgetArgument(ActFrm, 0),
                           pp_format, NULL, ", ");
    OutStream <<  ". ";
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 1),
                            pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsActImp(ActFrm))
  {
    //print implication
    mCRL2log(log::debug2, "pretty printer") << "printing implication" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 0),
                            pp_format, ShowSorts, 2);
    OutStream <<  " => ";
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 1),
                            pp_format, ShowSorts, 1);
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm))
  {
    //print conjunction or disjunction
    mCRL2log(log::debug2, "pretty printer") << "printing conjunction or disjunction" << std::endl;
    if (PrecLevel > 2)
    {
      OutStream <<  "(";
    }
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 0),
                            pp_format, ShowSorts, 3);
    if (gsIsActAnd(ActFrm))
    {
      OutStream <<  " && ";
    }
    else
    {
      OutStream <<  " || ";
    }
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 1),
                            pp_format, ShowSorts, 2);
    if (PrecLevel > 2)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsActAt(ActFrm))
  {
    //print at expression
    mCRL2log(log::debug2, "pretty printer") << "printing at expression" << std::endl;
    if (PrecLevel > 3)
    {
      OutStream <<  "(";
    }
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 0),
                            pp_format, ShowSorts, 3);
    OutStream <<  " @ ";
    PrintDataExpr(OutStream, ATAgetArgument(ActFrm, 1),
                              pp_format, ShowSorts, gsPrecIdPrefix());
    if (PrecLevel > 3)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsActNot(ActFrm))
  {
    //print negation
    mCRL2log(log::debug2, "pretty printer") << "printing negation" << std::endl;
    if (PrecLevel > 4)
    {
      OutStream <<  "(";
    }
    OutStream <<  "!";
    PrintActFrm(OutStream, ATAgetArgument(ActFrm, 0),
                            pp_format, ShowSorts, 4);
    if (PrecLevel > 4)
    {
      OutStream <<  ")";
    }
  }
}

static void PrintPBExpr(std::ostream& OutStream,
                                    const aterm_appl PBExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsPBExpr(PBExpr));
  if (gsIsDataExpr(PBExpr))
  {
    //print data expression
    mCRL2log(log::debug2, "pretty printer") << "printing data expression" << std::endl;
    OutStream <<  "val(";
    PrintDataExpr(OutStream, PBExpr, pp_format, ShowSorts, 0);
    OutStream <<  ")";
  }
  else if (gsIsPBESTrue(PBExpr))
  {
    //print true
    mCRL2log(log::debug2, "pretty printer") << "printing true" << std::endl;
    OutStream <<  "true";
  }
  else if (gsIsPBESFalse(PBExpr))
  {
    //print false
    mCRL2log(log::debug2, "pretty printer") << "printing false" << std::endl;
    OutStream <<  "false";
  }
  else if (gsIsPropVarInst(PBExpr))
  {
    //print propositional variable instance
    mCRL2log(log::debug2, "pretty printer") << "printing propositional variable instance" << std::endl;
    PrintPart_Appl(OutStream, ATAgetArgument(PBExpr, 0),
                               pp_format, ShowSorts, PrecLevel);
    aterm_list Args = ATLgetArgument(PBExpr, 1);
    if (Args.size() > 0)
    {
      OutStream <<  "(";
      PrintPart_List(OutStream, Args,
                                 pp_format, ShowSorts, 0, NULL, ", ");
      OutStream <<  ")";
    }
  }
  else if (gsIsPBESForall(PBExpr) || gsIsPBESExists(PBExpr))
  {
    //print quantification
    mCRL2log(log::debug2, "pretty printer") << "printing quantification" << std::endl;
    if (PrecLevel > 0)
    {
      OutStream <<  "(";
    }
    if (gsIsPBESForall(PBExpr))
    {
      OutStream <<  "forall ";
    }
    else
    {
      OutStream <<  "exists ";
    }
    PrintDecls(OutStream, ATLgetArgument(PBExpr, 0),
                           pp_format, NULL, ", ");
    OutStream <<  ". ";
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 1),
                            pp_format, ShowSorts, 0);
    if (PrecLevel > 0)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsPBESImp(PBExpr))
  {
    //print implication
    mCRL2log(log::debug2, "pretty printer") << "printing implication" << std::endl;
    if (PrecLevel > 1)
    {
      OutStream <<  "(";
    }
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 0),
                            pp_format, ShowSorts, 2);
    OutStream <<  " => ";
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 1),
                            pp_format, ShowSorts, 1);
    if (PrecLevel > 1)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsPBESAnd(PBExpr) || gsIsPBESOr(PBExpr))
  {
    //print conjunction or disjunction
    mCRL2log(log::debug2, "pretty printer") << "printing conjunction or disjunction" << std::endl;
    if (PrecLevel > 2)
    {
      OutStream <<  "(";
    }
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 0),
                            pp_format, ShowSorts, 3);
    if (gsIsPBESAnd(PBExpr))
    {
      OutStream <<  " && ";
    }
    else
    {
      OutStream <<  " || ";
    }
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 1),
                            pp_format, ShowSorts, 2);
    if (PrecLevel > 2)
    {
      OutStream <<  ")";
    }
  }
  else if (gsIsPBESNot(PBExpr))
  {
    //print negation
    mCRL2log(log::debug2, "pretty printer") << "printing negation" << std::endl;
    if (PrecLevel > 3)
    {
      OutStream <<  "(";
    }
    OutStream <<  "!";
    PrintPBExpr(OutStream, ATAgetArgument(PBExpr, 0),
                            pp_format, ShowSorts, 3);
    if (PrecLevel > 3)
    {
      OutStream <<  ")";
    }
  }
}

void PrintLinearProcessSummand(std::ostream& OutStream,
    const aterm_appl Summand, t_pp_format pp_format, bool ShowSorts)
{
  assert(gsIsLinearProcessSummand(Summand));
  mCRL2log(log::debug2, "pretty printer") << "printing LPS summand" << std::endl;
  //print data summations
  aterm_list SumVarDecls = ATLgetArgument(Summand, 0);
  if (SumVarDecls.size() > 0)
  {
    OutStream <<  "sum ";
    PrintDecls(OutStream, SumVarDecls, pp_format, NULL, ",");
    OutStream <<  ".\n         ";
  }
  //print condition
  aterm_appl Cond = ATAgetArgument(Summand, 1);
  if (/*!gsIsNil(Cond) && */!data::sort_bool::is_true_function_symbol(data::data_expression(Cond)))   // JK 15/10/2009 condition is always a data expression
  {
    PrintDataExpr(OutStream, Cond, pp_format, ShowSorts, gsPrecIdPrefix());
    OutStream <<  " ->\n         ";
  }
  //print multiaction
  aterm_appl MultAct = ATAgetArgument(Summand, 2);
  aterm_appl Time = ATAgetArgument(Summand, 3);
  bool IsTimed = !gsIsNil(Time);
  PrintPart_Appl(OutStream, MultAct, pp_format, ShowSorts,
                             (IsTimed)?6:5);
  //print time
  if (IsTimed)
  {
    OutStream <<  " @ ";
    PrintDataExpr(OutStream, Time, pp_format, ShowSorts, gsPrecIdPrefix());
  }
  //print process reference
  if (!gsIsDelta(MultAct))
  {
    OutStream <<  " .\n         ";
    OutStream <<  "P";
    aterm_list Assignments = ATLgetArgument(Summand, 4);
    OutStream <<  "(";
    PrintPart_List(OutStream, Assignments,
                               pp_format, ShowSorts, 0, NULL, ", ");
    OutStream <<  ")";
  }
}

aterm_list GetAssignmentsRHS(aterm_list Assignments)
{
  aterm_list l;
  while (!Assignments.empty())
  {
    l.push_front(ATAgetFirst(Assignments)[1]);
    Assignments = Assignments.tail();
  }
  return reverse(l);
}

aterm_list gsGroupDeclsBySort(aterm_list Decls)
{
  using namespace atermpp;
  if (!Decls.empty())
  {
    table SortDeclsTable(2*Decls.size(), 50);
    //Add all variable declarations from Decls to hash table
    //SortDeclsTable
    while (!Decls.empty())
    {
      aterm_appl Decl = ATAgetFirst(Decls);
      aterm_appl DeclSort = ATAgetArgument(Decl, 1);
      aterm_list CorDecls = aterm_cast<aterm_list>(SortDeclsTable.get(DeclSort));
      if (CorDecls.defined())
      {
        aterm_list temp=CorDecls;
        temp.push_front(Decl);
        SortDeclsTable.put(DeclSort,temp);
      }
      else
      {
        SortDeclsTable.put(DeclSort,make_list<aterm>(Decl));
      }
      Decls = Decls.tail();
    }
    //Return the hash table as a list of variable declarations
    aterm_list DeclSorts = SortDeclsTable.keys();
    aterm_list Result;
    while (!DeclSorts.empty())
    {
      Result = aterm_cast<aterm_list>(SortDeclsTable.get(DeclSorts.front()))+ Result;
      DeclSorts = DeclSorts.tail();
    }
    return reverse(Result);
  }
  else
  {
    //Decls is empty
    return Decls;
  }
}

bool gsHasConsistentContext(const table &DataVarDecls,
                            const aterm_appl Part)
{
  bool Result = true;
  if (gsIsDataEqn(Part) || gsIsProcEqn(Part))
  {
    //check consistency of DataVarDecls with the variable declarations
    aterm_list VarDecls = ATLgetArgument(Part, 0);
    size_t n = VarDecls.size();
    for (size_t i = 0; i < n && Result; i++)
    {
      //check consistency of variable VarDecls(j) with VarDeclTable
      aterm_appl VarDecl = ATAelementAt(VarDecls, i);
      aterm_appl CorVarDecl =
        aterm_cast<aterm_appl>(DataVarDecls.get(VarDecl[0]));
      if (CorVarDecl != aterm())
      {
        //check consistency of VarDecl with CorVarDecl
        Result = (VarDecl==CorVarDecl);
      }
    }
  }
  else if (gsIsOpId(Part) || gsIsId(Part))
  {
    //Part may be an operation; check that its name does not occur in
    //DataVarDecls
    Result = (DataVarDecls.get(Part[0]) == aterm());
  }
  //check consistency in the arguments of Part
  if (Result)
  {
    function_symbol Head = Part.function();
    size_t NrArgs = Head.arity();
    for (size_t i = 0; i < NrArgs && Result; i++)
    {
      aterm Arg = Part[i];
      if (Arg.type_is_appl())
      {
        Result = gsHasConsistentContext(DataVarDecls, (aterm_appl) Arg);
      }
      else //Arg.type_is_list()
      {
        Result = gsHasConsistentContextList(DataVarDecls, (aterm_list) Arg);
      }
    }
  }
  return Result;
}

bool gsHasConsistentContextList(const table &DataVarDecls,
                                const aterm_list Parts)
{
  bool Result = true;
  aterm_list l = Parts;
  while (!l.empty() && Result)
  {
    Result = gsHasConsistentContext(DataVarDecls, ATAgetFirst(l));
    l = l.tail();
  }
  return Result;
}

bool gsIsOpIdNumericUpCast(aterm_appl DataExpr)
{
  if (!gsIsOpId(DataExpr))
  {
    return false;
  }
  return
    data::sort_nat::is_pos2nat_function_symbol(data::data_expression(DataExpr)) ||
    data::sort_int::is_pos2int_function_symbol(data::data_expression(DataExpr)) ||
    data::sort_real::is_pos2real_function_symbol(data::data_expression(DataExpr)) ||
    data::sort_int::is_nat2int_function_symbol(data::data_expression(DataExpr)) ||
    data::sort_real::is_nat2real_function_symbol(data::data_expression(DataExpr)) ||
    data::sort_real::is_int2real_function_symbol(data::data_expression(DataExpr))
    ;
}

bool gsIsIdListEnum(aterm_appl DataExpr)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  return ATAgetArgument(DataExpr, 0) == data::sort_list::list_enumeration_name();
}

bool gsIsIdSetEnum(aterm_appl DataExpr)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  return ATAgetArgument(DataExpr, 0) == data::sort_set::set_enumeration_name();
}

bool gsIsIdBagEnum(aterm_appl DataExpr)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  return ATAgetArgument(DataExpr, 0) == data::sort_bag::bag_enumeration_name();
}

bool gsIsIdFuncUpdate(aterm_appl DataExpr)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  return ATAgetArgument(DataExpr, 0) == data::function_update_name();
}

bool gsIsIdPrefix(aterm_appl DataExpr, size_t ArgsLength)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  if (ArgsLength != 1)
  {
    return false;
  }
  aterm_appl IdName = ATAgetArgument(DataExpr, 0);
  return
    (IdName == data::sort_bool::not_name())      ||
    (IdName == data::sort_int::negate_name())      ||
    (IdName == data::sort_list::count_name()) ||
    (IdName == data::sort_set::complement_name());
}

bool gsIsIdInfix(aterm_appl DataExpr, size_t ArgsLength)
{
  if (!(gsIsId(DataExpr) || gsIsOpId(DataExpr)))
  {
    return false;
  }
  if (ArgsLength != 2)
  {
    return false;
  }
  aterm_appl IdName = ATAgetArgument(DataExpr, 0);
  return
    (IdName == data::sort_bool::implies_name())          ||
    (IdName == data::sort_bool::and_name())          ||
    (IdName == data::sort_bool::or_name())           ||
    (IdName == data::detail::equal_symbol())           ||
    (IdName == data::detail::not_equal_symbol())          ||
    (IdName == data::detail::less_symbol())           ||
    (IdName == data::detail::less_equal_symbol())          ||
    (IdName == data::detail::greater_symbol())           ||
    (IdName == data::detail::greater_equal_symbol())          ||
    (IdName == data::sort_list::in_name())        ||
    (IdName == data::sort_list::cons_name())         ||
    (IdName == data::sort_list::snoc_name())         ||
    (IdName == data::sort_list::concat_name())       ||
    (IdName == data::sort_real::plus_name())          ||
    (IdName == data::sort_real::minus_name())         ||
    (IdName == data::sort_set::union_name())     ||
    (IdName == data::sort_set::difference_name())      ||
    (IdName == data::sort_bag::join_name())      ||
    (IdName == data::sort_bag::difference_name())      ||
    (IdName == data::sort_int::div_name())          ||
    (IdName == data::sort_int::mod_name())          ||
    (IdName == data::sort_real::divides_name())       ||
    (IdName == data::sort_int::times_name())         ||
    (IdName == data::sort_list::element_at_name())        ||
    (IdName == data::sort_set::intersection_name()) ||
    (IdName == data::sort_bag::intersection_name());
}

int gsPrecIdPrefix()
{
  return 13;
}

int gsPrecIdInfix(aterm_appl IdName)
{
  if (IdName == data::sort_bool::implies_name())
  {
    return 2;
  }
  else if ((IdName == data::sort_bool::and_name()) || (IdName == data::sort_bool::or_name()))
  {
    return 3;
  }
  else if ((IdName == data::detail::equal_symbol()) || (IdName == data::detail::not_equal_symbol()))
  {
    return 4;
  }
  else if (
    (IdName == data::detail::less_symbol()) || (IdName == data::detail::less_equal_symbol()) ||
    (IdName == data::detail::greater_symbol()) || (IdName == data::detail::greater_equal_symbol()) ||
    (IdName == data::sort_list::in_name())
  )
  {
    return 5;
  }
  else if ((IdName == data::sort_list::cons_name()))
  {
    return 6;
  }
  else if ((IdName == data::sort_list::snoc_name()))
  {
    return 7;
  }
  else if ((IdName == data::sort_list::concat_name()))
  {
    return 8;
  }
  else if (
    (IdName == data::sort_real::plus_name()) || (IdName == data::sort_real::minus_name()) ||
    (IdName == data::sort_set::union_name()) || (IdName == data::sort_set::difference_name()) ||
    (IdName == data::sort_bag::join_name()) || (IdName == data::sort_bag::difference_name())
  )
  {
    return 9;
  }
  else if ((IdName == data::sort_int::div_name()) || (IdName == data::sort_int::mod_name()) ||
           (IdName == data::sort_real::divides_name()))
  {
    return 10;
  }
  else if (
    (IdName == data::sort_int::times_name()) || (IdName == data::sort_list::element_at_name()) ||
    (IdName == data::sort_set::intersection_name()) ||
    (IdName == data::sort_bag::intersection_name())
  )
  {
    return 11;
  }
  else
  {
    //something went wrong
    return -1;
  }
}

int gsPrecIdInfixLeft(aterm_appl IdName)
{
  if (IdName == data::sort_bool::implies_name())
  {
    return 3;
  }
  else if ((IdName == data::sort_bool::and_name()) || (IdName == data::sort_bool::or_name()))
  {
    return 4;
  }
  else if ((IdName == data::detail::equal_symbol()) || (IdName == data::detail::not_equal_symbol()))
  {
    return 5;
  }
  else if (
    (IdName == data::detail::less_symbol()) || (IdName == data::detail::less_equal_symbol()) ||
    (IdName == data::detail::greater_symbol()) || (IdName == data::detail::greater_equal_symbol()) ||
    (IdName == data::sort_list::in_name())
  )
  {
    return 6;
  }
  else if ((IdName == data::sort_list::cons_name()))
  {
    return 9;
  }
  else if ((IdName == data::sort_list::snoc_name()))
  {
    return 7;
  }
  else if ((IdName == data::sort_list::concat_name()))
  {
    return 8;
  }
  else if (
    (IdName == data::sort_real::plus_name()) || (IdName == data::sort_real::minus_name()) ||
    (IdName == data::sort_set::union_name()) || (IdName == data::sort_set::difference_name()) ||
    (IdName == data::sort_bag::join_name()) || (IdName == data::sort_bag::difference_name())
  )
  {
    return 9;
  }
  else if ((IdName == data::sort_int::div_name()) || (IdName == data::sort_int::mod_name()) ||
           (IdName == data::sort_real::divides_name()))
  {
    return 10;
  }
  else if (
    (IdName == data::sort_int::times_name()) || (IdName == data::sort_list::element_at_name()) ||
    (IdName == data::sort_set::intersection_name()) ||
    (IdName == data::sort_bag::intersection_name())
  )
  {
    return 11;
  }
  else
  {
    //something went wrong
    return -1;
  }
}

int gsPrecIdInfixRight(aterm_appl IdName)
{
  if (IdName == data::sort_bool::implies_name())
  {
    return 2;
  }
  else if ((IdName == data::sort_bool::and_name()) || (IdName == data::sort_bool::or_name()))
  {
    return 3;
  }
  else if ((IdName == data::detail::equal_symbol()) || (IdName == data::detail::not_equal_symbol()))
  {
    return 4;
  }
  else if (
    (IdName == data::detail::less_symbol()) || (IdName == data::detail::less_equal_symbol()) ||
    (IdName == data::detail::greater_symbol()) || (IdName == data::detail::greater_equal_symbol()) ||
    (IdName == data::sort_list::in_name())
  )
  {
    return 6;
  }
  else if ((IdName == data::sort_list::cons_name()))
  {
    return 6;
  }
  else if ((IdName == data::sort_list::snoc_name()))
  {
    return 9;
  }
  else if ((IdName == data::sort_list::concat_name()))
  {
    return 9;
  }
  else if (
    (IdName == data::sort_real::plus_name()) || (IdName == data::sort_real::minus_name()) ||
    (IdName == data::sort_set::union_name()) || (IdName == data::sort_set::difference_name()) ||
    (IdName == data::sort_bag::join_name()) || (IdName == data::sort_bag::difference_name())
  )
  {
    return 10;
  }
  else if ((IdName == data::sort_int::div_name()) || (IdName == data::sort_int::mod_name()) ||
           (IdName == data::sort_real::divides_name()))
  {
    return 11;
  }
  else if (
    (IdName == data::sort_int::times_name()) || (IdName == data::sort_list::element_at_name()) ||
    (IdName == data::sort_set::intersection_name()) ||
    (IdName == data::sort_bag::intersection_name())
  )
  {
    return 12;
  }
  else
  {
    //something went wrong
    return -1;
  }
}

} // namespace detail

inline
void PrintPart_CXX(std::ostream& out_stream, const aterm part,
                   t_pp_format pp_format)
{
  detail::PrintPart__CXX(out_stream, part, pp_format);
}

inline
std::string PrintPart_CXX(const aterm part, t_pp_format pp_format)
{
  std::stringstream ss;
  PrintPart_CXX(ss, part, pp_format);
  return ss.str();
}

/** \brief Return a textual description of an aterm representation of an
 *         mCRL2 specification or expression.
 *  \param[in] part An aterm representation of a part of an mCRL2
 *             specification or expression.
 *  \param[in] pp_format A pretty print format.
 *  \return A textual representation of part according to method pp_format.
**/
template <typename Term>
std::string pp_deprecated(Term part, t_pp_format pp_format = ppDefault)
{
  return PrintPart_CXX(part, pp_format);
}

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_PP_DEPRECATED_H
