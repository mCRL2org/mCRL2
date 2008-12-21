// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_implementation.h
/// \brief This file contains a generic implementation of the PrintPart functions for
//both C and C++. It should NEVER be included directly. Use print.h.

#if defined(PRINT_C)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRINT_OUTTYPE FILE*
#define PRINT_FUNC(x) x ## _C
#endif

#if defined(PRINT_CXX)
#include <ostream>
#include <iostream>
#define PRINT_OUTTYPE std::ostream&
#define PRINT_FUNC(x) x ## _CXX
#endif

#include <assert.h>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/core/data_reconstruct.h"

using namespace mcrl2::utilities;

namespace mcrl2 {
  namespace core { 
    namespace detail {

//declarations
//------------

//The internal format mentioned below can be found in specs/mcrl2.internal.txt 

static void PRINT_FUNC(PrintPart_)(PRINT_OUTTYPE OutStream, const ATerm Part,
  t_pp_format pp_format);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATerm containing a part of the internal format
  Post:A textual representation of Part is written to OutStream, using method
       pp_format
*/

static void PRINT_FUNC(PrintPart_Appl)(PRINT_OUTTYPE OutStream,
  const ATermAppl Part, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATermAppl containing a part of the internal format
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

static void PRINT_FUNC(PrintPart_List)(PRINT_OUTTYPE OutStream,
  const ATermList Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
  const char *Terminator, const char *Separator);
/*Pre: OutStream points to a stream to which can be written
       Parts is an ATermList containing parts of the internal format
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

static void PRINT_FUNC(PrintPart_BagEnum)(PRINT_OUTTYPE OutStream,
  const ATermList Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
  const char *Terminator, const char *Separator);
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


static void PRINT_FUNC(PrintEqns)(PRINT_OUTTYPE OutStream,
  const ATermList DataEqns, t_pp_format, bool ShowSorts, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       DataEqns is an ATermList containing data equations from the internal
       format
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

static void PRINT_FUNC(PrintDecls)(PRINT_OUTTYPE OutStream,
  const ATermList Decls, t_pp_format pp_format,
  const char *Terminator, const char *Separator);
/*Pre: Decls is an ATermList containing action, operation, or variable
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

static void PRINT_FUNC(PrintDecl)(PRINT_OUTTYPE OutStream,
  const ATermAppl Decl, t_pp_format pp_format, const bool ShowSorts);
/*Pre: Decl is an ATermAppl that represents an action, operation, or variable
       declaration from the internal format
       pp_format != ppInternal
       ShowSorts indicates if the sort of the declaration should be shown
  Ret: A textual representation of the declaration, say Decl(x, S), is written
       to OutStream, i.e.:
       - "x: S", if ShowSorts
       - "x", otherwise
*/

static void PRINT_FUNC(PrintSortExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl SortExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintDataExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl DataExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintProcExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl ProcExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintStateFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintRegFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl RegFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintActFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl ActFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintPBExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel);
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

static void PRINT_FUNC(PrintLinearProcessSummand)(PRINT_OUTTYPE OutStream,
  const ATermAppl LinearProcessSummand, const ATermList VarDecls,
  t_pp_format pp_format, bool ShowSorts);
/*Pre: OutStream points to a stream to which can be written
       LinearProcessSummand is a linear process summand
       VarDecls is a list of variable declarations, or NULL
       pp_format != ppInternal
       ShowSorts indicates if the sorts of DataExpr should be shown
  Post:A textual representation of the expression is written to OutStream, in
       which:
       - ShowSorts is taken into account
       - if pp_format != ppDebug and VarDecls != NULL, the next state of the
         summand is printed without assignments
*/

static ATermList GetAssignmentsRHS(ATermList Assignments);
/*Pre: Assignments is a list of assignments of data expressions to data
       variables
  Ret: The right-hand sides the assignments
*/

static ATermList SubstituteAssignments_list(ATermList Vars,
  ATermList Assignments);
/*Pre: Vars is a list of data variables
       Assignments is a list of assignments of data expressions to data
       variables
  Ret: Vars in which each element v is replaced by the right-hand side of the
       first assignment in Assignments in which v occurs as a left-hand side,
       or by v itself, if it doesn't occur at all as a left-hand side.
*/

static ATermAppl SubstituteAssignments_appl(ATermAppl Var,
  ATermList Assignments);
/*Pre: Var is a data variable
       Assignments is a list of assignments of data expressions to data
       variables
  Ret: The right-hand side of the first assignment in Assignments in which Var
       occurs as a left-hand side. Var, if it doesn't occur at all as a
       left-hand side.
*/

static ATermList gsGroupDeclsBySort(ATermList Decls);
/*Pre: Decls is an ATermList containing declarations of the form
       Decl(Name, Sort) from the internal format
  Ret: a list containing the declarations from Decls, where declarations of the
       same sort are placed in sequence
*/

static bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part);
/*Pre: DataVarDecls represents the variables from an equation section, where
       the keys are the variable names and the values are the corresponding
       variables
       Part is an ATermAppl containing an equation of the internal format,
       or the elements it consists of
  Ret: all operations occurring in Part are consistent with the variables from
       the context
 */

static bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts);
/*Pre: DataVarDecls represents the variables from an equation section, where
       the keys are the variable names and the values are the
       corresponding variables
       Parts is an ATermList containing elements of an equation of the internal
       format
  Ret: all operations occurring in Parts are consistent with the variables from
       the context
 */

static bool gsIsOpIdNumericUpCast(ATermAppl Term);
//Ret: DataExpr is an operation identifier for a numeric upcast

static bool gsIsOpIdPrefix(ATermAppl Term);
//Ret: DataExpr is a prefix operation identifier

static bool gsIsOpIdInfix(ATermAppl Term);
//Ret: DataExpr is an infix operation identifier

static int gsPrecOpIdPrefix();
//Ret: Precedence of prefix operators
//     (higher than all infix operators and arguments thereof)

static int gsPrecOpIdInfix(ATermAppl OpIdName);
//Pre: OpIdName is the name of an infix operation identifier
//Ret: Precedence of the operation itself

static int gsPrecOpIdInfixLeft(ATermAppl OpIdName);
//Pre: OpIdInfix is the name of an infix operation identifier
//Ret: Precedence of the left argument of the operation

static int gsPrecOpIdInfixRight(ATermAppl OpIdName);
//Pre: OpIdInfix is the name of an infix operation identifier
//Ret: Precedence of the right argument of the operation

//implementation
//--------------

inline static void PRINT_FUNC(fprints)(PRINT_OUTTYPE OutStream,
  const char *Value)
//Pre:  OutStream and Value are not NULL
//Post: Value is written to OutStream in C/C++ style
{
#if (defined(PRINT_C) == defined(PRINT_CXX))
  assert(false);
#endif
#if defined(PRINT_C)
  fprintf(OutStream, "%s", Value);
#elif defined(PRINT_CXX)
  OutStream << Value;
#endif
}

inline static void PRINT_FUNC(dbg_prints)(const char *Value)
{
//Pre:  Value is not NULL
//Post: Value is written to stderr in C/C++ style if gsDebug is true
#if (defined(PRINT_C) == defined(PRINT_CXX))
  assert(false);
#endif
#if defined(PRINT_C)
//  if (gsDebug) fprintf(stderr, Value);
#elif defined(PRINT_CXX)
//  if (gsDebug) std::cerr << Value;
#endif
}

void PRINT_FUNC(PrintPart_)(PRINT_OUTTYPE OutStream, const ATerm Part,
  t_pp_format pp_format)
{
  if (pp_format == ppInternal) {
#if defined(PRINT_C)
    ATwriteToTextFile(Part, OutStream);
    fprintf(OutStream, "\n");
#elif defined(PRINT_CXX)
    OutStream << ATwriteToString(Part) << std::endl;
#endif
  } else {
    ATerm ReconstructedPart = Part;
    // Do not do data reconstruction in case of a debug print
    if (pp_format != ppDebug) {
      if (gsIsDataSpec((ATermAppl) Part) || gsIsProcSpec((ATermAppl) Part) ||
          gsIsLinProcSpec((ATermAppl) Part) || gsIsPBES((ATermAppl) Part))
      {
        ReconstructedPart = reconstruct_exprs(Part, (ATermAppl) Part);
      } else {
        ReconstructedPart = reconstruct_exprs(Part, NULL);
      }
    }
    if (ATgetType(ReconstructedPart) == AT_APPL) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, (ATermAppl) ReconstructedPart, pp_format,
        false, 0);
    } else if (ATgetType(ReconstructedPart) == AT_LIST) {
      PRINT_FUNC(fprints)(OutStream, "[");
      PRINT_FUNC(PrintPart_List)(OutStream, (ATermList) ReconstructedPart,
        pp_format, false, 0, "", ", ");
      PRINT_FUNC(fprints)(OutStream, "]");
    } else {
#if defined(PRINT_C)
      gsErrorMsg("ATerm Part is not an ATermAppl or an ATermList\n");
#elif defined(PRINT_CXX)
      std::cerr  << "error: ATerm Part is not an ATermAppl or an ATermList\n";
#endif
      assert(false);
    }
  }
}

void PRINT_FUNC(PrintPart_Appl)(PRINT_OUTTYPE OutStream,
  const ATermAppl Part, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  if (gsIsString(Part)) {
    //print string
    PRINT_FUNC(fprints)(OutStream, ATgetName(ATgetAFun(Part)));
  } else if (gsIsSortExpr(Part)) {
    //print sort expression or unknown
    PRINT_FUNC(dbg_prints)("printing sort expression or unknown\n");
    PRINT_FUNC(PrintSortExpr)(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  } else if (gsIsStructCons(Part)) {
    //print structured sort constructor
    PRINT_FUNC(dbg_prints)("printing structured sort constructor\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList StructProjs = ATLgetArgument(Part, 1);
    if (ATgetLength(StructProjs) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, StructProjs,
        pp_format, ShowSorts, PrecLevel, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
    ATermAppl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser)) {
      PRINT_FUNC(fprints)(OutStream, "?");
      PRINT_FUNC(PrintPart_Appl)(OutStream, Recogniser,
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsStructProj(Part)) {
    //print structured sort projection
    PRINT_FUNC(dbg_prints)("printing structured sort projection\n");
    ATermAppl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection)) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, Projection,
        pp_format, ShowSorts, PrecLevel);
      PRINT_FUNC(fprints)(OutStream, ": ");
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, 0);
  } else if (gsIsNil(Part)) {
    //print nil
    PRINT_FUNC(dbg_prints)("printing nil\n");
    PRINT_FUNC(fprints)(OutStream, "nil");
  } else if (gsIsDataExpr(Part)) {
    //print data expression
    PRINT_FUNC(dbg_prints)("printing data expression\n");
    PRINT_FUNC(PrintDataExpr)(OutStream, Part, pp_format, ShowSorts, PrecLevel);

  } else if (gsIsIdInit(Part)) {
    //print identifier initialisation
    PRINT_FUNC(dbg_prints)("printing identifier initialisation\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " = ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, 0);
  } else if (gsIsMultAct(Part)) {
    //print multiaction
    PRINT_FUNC(dbg_prints)("printing multiaction\n");
    ATermList Actions = ATLgetArgument(Part, 0);
    int ActionsLength = ATgetLength(Actions);
    if (ActionsLength == 0) {
      PRINT_FUNC(fprints)(OutStream, "tau");
    } else {
      //ActionsLength > 0
      if (PrecLevel > 7) PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, Actions,
        pp_format, ShowSorts, PrecLevel, NULL, "|");
      if (PrecLevel > 7) PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsActId(Part)) {
    //print action identifier
    PRINT_FUNC(dbg_prints)("printing action identifier\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        PRINT_FUNC(fprints)(OutStream, ": ");
        PRINT_FUNC(PrintPart_List)(OutStream, SortExprs,
          pp_format, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsProcExpr(Part)) {
    //print process expression
    PRINT_FUNC(dbg_prints)("printing process expression\n");
    PRINT_FUNC(PrintProcExpr)(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  } else if (gsIsProcVarId(Part)) {
    //print process variable
    PRINT_FUNC(dbg_prints)("printing process variable\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        PRINT_FUNC(fprints)(OutStream, ": ");
        PRINT_FUNC(PrintPart_List)(OutStream, SortExprs,
          pp_format, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsMultActName(Part)) {
    //print multiaction name
    PRINT_FUNC(dbg_prints)("printing multiaction name\n");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(Part, 0),
      pp_format, ShowSorts, 0, NULL, " | ");
  } else if (gsIsRenameExpr(Part)) {
    //print renaming expression
    PRINT_FUNC(dbg_prints)("printing renaming expression\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " -> ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsCommExpr(Part)) {
    //print communication expression
    PRINT_FUNC(dbg_prints)("printing communication expression\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermAppl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult)) {
      PRINT_FUNC(fprints)(OutStream, " -> ");
      PRINT_FUNC(PrintPart_Appl)(OutStream, CommResult,
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsProcSpec(Part) || gsIsLinProcSpec(Part)) {
    //print process specification or linear process specification
    PRINT_FUNC(dbg_prints)("printing specification\n");
    for (int i = 0; i < 4; i++) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, i),
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsDataSpec(Part)) {
    //print data specification
    PRINT_FUNC(dbg_prints)("printing data specification\n");
    for (int i = 0; i < 4; i++) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, i),
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    PRINT_FUNC(dbg_prints)("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SortDecls) > 0) {
      PRINT_FUNC(fprints)(OutStream, "sort ");
      PRINT_FUNC(PrintPart_List)(OutStream, SortDecls,
        pp_format, ShowSorts, PrecLevel, ";\n", "     ");
      PRINT_FUNC(fprints)(OutStream, "\n");
    }
  } else if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    //print operation specification
    PRINT_FUNC(dbg_prints)("printing operation specification\n");
    ATermList OpIds = ATLgetArgument(Part, 0);
    if (ATgetLength(OpIds) > 0) {
      PRINT_FUNC(fprints)(OutStream, gsIsConsSpec(Part)?"cons ":"map  ");
      PRINT_FUNC(PrintDecls)(OutStream, OpIds, pp_format, ";\n", "     ");
      PRINT_FUNC(fprints)(OutStream, "\n");
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    PRINT_FUNC(dbg_prints)("printing equation specification\n");
    PRINT_FUNC(PrintEqns)(OutStream, ATLgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsActSpec(Part)) {
    //print action specification
    PRINT_FUNC(dbg_prints)("printing action specification\n");
    ATermList ActIds = ATLgetArgument(Part, 0);
    if (ATgetLength(ActIds) > 0) {
      PRINT_FUNC(fprints)(OutStream, "act  ");
      PRINT_FUNC(PrintDecls)(OutStream, ActIds, pp_format, ";\n", "     ");
      PRINT_FUNC(fprints)(OutStream, "\n");
    }
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    PRINT_FUNC(dbg_prints)("printing sort reference declaration\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " = ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsDataEqn(Part)) {
    //print data equation (without variables)
    PRINT_FUNC(dbg_prints)("printing data equation\n");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, Condition,
        pp_format, ShowSorts, 0);
      PRINT_FUNC(fprints)(OutStream, "  ->  ");
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 2),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, "  =  ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 3),
      pp_format, ShowSorts, 0);
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    PRINT_FUNC(dbg_prints)("printing process specification\n");
    PRINT_FUNC(PrintEqns)(OutStream, ATLgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsLinearProcess(Part)) {
    //print Linear process
    PRINT_FUNC(dbg_prints)("printing LPS\n");
    //print global variables
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      PRINT_FUNC(fprints)(OutStream, "var  ");
      PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
        pp_format, ";\n", "     ");
    }
    //print process name and variable declarations
    ATermList VarDecls = ATLgetArgument(Part, 1);
    int VarDeclsLength = ATgetLength(VarDecls);
    PRINT_FUNC(fprints)(OutStream, "proc P");
    if (VarDeclsLength > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintDecls)(OutStream, VarDecls, pp_format, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
    PRINT_FUNC(fprints)(OutStream, " =");
    //print summations
    ATermList Summands = ATLgetArgument(Part, 2);
    int SummandsLength = ATgetLength(Summands);
    if (SummandsLength == 0) {
      PRINT_FUNC(fprints)(OutStream, " delta@0;\n");
    } else {
      //SummandsLength > 0
      PRINT_FUNC(fprints)(OutStream, "\n       ");
      ATermList l = Summands;
      while (!ATisEmpty(l)) {
        if (!ATisEqual(l, Summands)) {
          PRINT_FUNC(fprints)(OutStream, "\n     + ");
        }
        PRINT_FUNC(PrintLinearProcessSummand)(OutStream, ATAgetFirst(l),
          VarDecls, pp_format, ShowSorts);
        l = ATgetNext(l);
      }
      PRINT_FUNC(fprints)(OutStream, ";\n");
    }
    PRINT_FUNC(fprints)(OutStream, "\n");
  } else if (gsIsProcEqn(Part)) {
    //print process equation (without free variables)
    PRINT_FUNC(dbg_prints)("printing process equation\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
    ATermList DataVarIds = ATLgetArgument(Part, 2);
    if (ATgetLength(DataVarIds) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintDecls)(OutStream, DataVarIds, pp_format, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
    PRINT_FUNC(fprints)(OutStream, " = ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 3),
      pp_format, ShowSorts, 0);
  } else if (gsIsLinearProcessSummand(Part)) {
    PRINT_FUNC(PrintLinearProcessSummand)(OutStream, Part, NULL, pp_format, ShowSorts);
    //print summand
  } else if (gsIsProcessInit(Part)) {
    //print initialisation
    PRINT_FUNC(dbg_prints)("printing initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      PRINT_FUNC(fprints)(OutStream, "var  ");
      PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
        pp_format, ";\n", "     ");
      PRINT_FUNC(fprints)(OutStream, "\n");
    }
    PRINT_FUNC(fprints)(OutStream, "init ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, ";\n");
  } else if (gsIsLinearProcessInit(Part)) {
    //print linear process initialisation
    PRINT_FUNC(dbg_prints)("printing LPS initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      PRINT_FUNC(fprints)(OutStream, "var  ");
      PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
        pp_format, ";\n", "     ");
    }
    PRINT_FUNC(fprints)(OutStream, "init P");
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      if (pp_format == ppDefault) {
        Args = GetAssignmentsRHS(Args);
      }
      PRINT_FUNC(PrintPart_List)(OutStream, Args,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
    PRINT_FUNC(fprints)(OutStream, ";\n");
  } else if (gsIsStateFrm(Part)) {
    //print state formula
    PRINT_FUNC(dbg_prints)("printing state formula\n");
    PRINT_FUNC(PrintStateFrm)(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  } else if (gsIsDataVarIdInit(Part)) {
    //print data variable identifier and initialisation
    PRINT_FUNC(dbg_prints)("printing data variable identifier and initialisation\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " = ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, 0);
  } else if (gsIsRegFrm(Part)) {
    //print regular formula
    PRINT_FUNC(dbg_prints)("printing regular formula\n");
    PRINT_FUNC(PrintRegFrm)(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  } else if (gsIsPBES(Part)) {
    //print PBES specification
    PRINT_FUNC(dbg_prints)("printing PBES specification\n");
    for (int i = 0; i < 3; i++) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, i),
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsPBEqnSpec(Part)) {
    //print parameterised boolean equation specification
    PRINT_FUNC(dbg_prints)("printing parameterised boolean equation specification\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      PRINT_FUNC(fprints)(OutStream, "var  ");
      PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
        pp_format, ";\n", "     ");
    }
    ATermList PBEqns = ATLgetArgument(Part, 1);
    if (ATgetLength(PBEqns) > 0) {
      PRINT_FUNC(fprints)(OutStream, "pbes ");
      PRINT_FUNC(PrintPart_List)(OutStream, PBEqns,
        pp_format, ShowSorts, PrecLevel, ";\n", "     ");
      PRINT_FUNC(fprints)(OutStream, "\n");
    }
  } else if (gsIsPBInit(Part)) {
    //print parameterised boolean initialisation
    PRINT_FUNC(dbg_prints)("printing parameterised boolean initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      PRINT_FUNC(fprints)(OutStream, "var  ");
      PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?Vars:gsGroupDeclsBySort(Vars),
        pp_format, ";\n", "     ");
    }
    PRINT_FUNC(fprints)(OutStream, "init ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, ";\n");
  } else if (gsIsPBEqn(Part)) {
    //print parameterised boolean equation
    PRINT_FUNC(dbg_prints)("printing parameterised boolean equation\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 1),
      pp_format, ShowSorts, PrecLevel);
    PRINT_FUNC(fprints)(OutStream, " =\n       ");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(Part, 2),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsFixpoint(Part)) {
    //print fixpoint
    PRINT_FUNC(dbg_prints)("printing fixpoint\n");
    if (gsIsNu(Part)) {
      PRINT_FUNC(fprints)(OutStream, "nu");
    } else {
      PRINT_FUNC(fprints)(OutStream, "mu");
    }
  } else if (gsIsPropVarDecl(Part)) {
    //print propositional variable declaration
    PRINT_FUNC(dbg_prints)("printing propositional variable declaration\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList DataVarIds = ATLgetArgument(Part, 1);
    if (ATgetLength(DataVarIds) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintDecls)(OutStream, DataVarIds, pp_format, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsPBExpr(Part)) {
    //print parameterised boolean expression
    PRINT_FUNC(dbg_prints)("printing parameterised boolean expression\n");
    PRINT_FUNC(PrintPBExpr)(OutStream, Part, pp_format, ShowSorts, PrecLevel);
  } else if (gsIsDataSpec(Part)) {
    //print data specification
    PRINT_FUNC(dbg_prints)("printing data specification\n");
    for (int i = 0; i < 4; i++) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, i),
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsActionRenameSpec(Part)) {
    //print action rename specification
    PRINT_FUNC(dbg_prints)("printing action rename specification\n");
    for (int i = 0; i < 3; i++) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, i),
        pp_format, ShowSorts, PrecLevel);
    }
  } else if (gsIsActionRenameRule(Part)) {
    //print action rename rule (without variables)
    PRINT_FUNC(dbg_prints)("printing action rename rule\n");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, Condition,
        pp_format, ShowSorts, 0);
      PRINT_FUNC(fprints)(OutStream, "  ->  ");
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 2),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, "  =>  ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Part, 3),
      pp_format, ShowSorts, 0);
  } else if (gsIsActionRenameRules(Part)) {
    //print action rename rules
    PRINT_FUNC(dbg_prints)("printing action rename rules\n");
    PRINT_FUNC(PrintEqns)(OutStream, ATLgetArgument(Part, 0),
      pp_format, ShowSorts, PrecLevel);
  } else {
#if defined(PRINT_C)
    gsErrorMsg("the term %T is not part of the internal format\n", Part);
#elif defined(PRINT_CXX)
    std::cerr
      << "error: the term "
      << ATwriteToString((ATerm) Part)
      << " is not part of the internal format\n";
#endif
    assert(false);
  }
}

void PRINT_FUNC(PrintPart_List)(PRINT_OUTTYPE OutStream,
  const ATermList Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
  const char *Terminator, const char *Separator)
{
  ATermList l = Parts;
  while (!ATisEmpty(l)) {
    if (!ATisEqual(l, Parts) && Separator != NULL) {
      PRINT_FUNC(fprints)(OutStream, Separator);
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetFirst(l),
      pp_format, ShowSorts, PrecLevel);
    if (Terminator != NULL) {
      PRINT_FUNC(fprints)(OutStream, Terminator);
    }
    l = ATgetNext(l);
  }
}

void PRINT_FUNC(PrintPart_BagEnum)(PRINT_OUTTYPE OutStream,
  const ATermList Parts, t_pp_format pp_format, bool ShowSorts, int PrecLevel,
  const char *Terminator, const char *Separator)
{
  ATermList l = Parts;
  while (!ATisEmpty(l)) {
    if (!ATisEqual(l, Parts) && Separator != NULL) {
      PRINT_FUNC(fprints)(OutStream, Separator);
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetFirst(l),
      pp_format, ShowSorts, PrecLevel);
    l = ATgetNext(l);
    PRINT_FUNC(fprints)(OutStream, ": ");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetFirst(l),
      pp_format, ShowSorts, PrecLevel);
    if (Terminator != NULL) {
      PRINT_FUNC(fprints)(OutStream, Terminator);
    }
    l = ATgetNext(l);
  }
}

void PRINT_FUNC(PrintEqns)(PRINT_OUTTYPE OutStream, const ATermList Eqns,
  t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  if (pp_format == ppDebug) {
    ATermList l = Eqns;
    while (!ATisEmpty(l)) {
      ATermAppl Eqn = ATAgetFirst(l);
      ATermList DataDecls = ATLgetArgument(Eqn, 0);
      if (!ATisEmpty(DataDecls)) {
        PRINT_FUNC(fprints)(OutStream, "var  ");
        PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?DataDecls:gsGroupDeclsBySort(DataDecls),
          pp_format, ";\n", "     ");
      }
      if (gsIsDataEqn(Eqn)) {
        PRINT_FUNC(fprints)(OutStream, "eqn  ");
      } else if (gsIsProcEqn(Eqn)) {
        PRINT_FUNC(fprints)(OutStream, "proc ");
      } else if (gsIsActionRenameRule(Eqn)) {
        PRINT_FUNC(fprints)(OutStream, "rename\n     ");
      } else {
        assert(false);
      }
      PRINT_FUNC(PrintPart_Appl)(OutStream, Eqn,
        pp_format, ShowSorts, PrecLevel);
      PRINT_FUNC(fprints)(OutStream, ";\n\n");
      l = ATgetNext(l);
    }
  } else { //pp_format == ppDefault
    int EqnsLength = ATgetLength(Eqns);
    if (EqnsLength > 0) {
      int StartPrefix = 0;
      ATermTable VarDeclTable = ATtableCreate(63, 50);
      //VarDeclTable is a hash table with variable declarations as values, where
      //the name of each variable declaration is used a key.
      //Note that the hash table will be increased if at least 32 values are added,
      //This can be avoided by increasing the initial size.
      int i = 0;
      while (i < EqnsLength) {
        //StartPrefix represents the start index of the maximum consistent prefix
        //of variable declarations in Eqns to which Eqns(i) belongs
        //VarDeclTable represents the variable declarations of Eqns from
        //StartPrefix up to i.
        //Check consistency of Eqns(i) with VarDeclTable and add newly declared
        //variables to VarDeclTable.
        ATermAppl Eqn = ATAelementAt(Eqns, i);
        bool Consistent = gsHasConsistentContext(VarDeclTable, Eqn);
        if (Consistent) {
          //add new variables from Eqns(i) to VarDeclTable
          ATermList VarDecls = ATLgetArgument(Eqn, 0);
          int VarDeclsLength = ATgetLength(VarDecls);
          for (int j = 0; j < VarDeclsLength; j++) {
            ATermAppl VarDecl = ATAelementAt(VarDecls, j);
            ATermAppl VarDeclName = ATAgetArgument(VarDecl, 0);
            if (ATtableGet(VarDeclTable, (ATerm) VarDeclName) == NULL) {
              ATtablePut(VarDeclTable, (ATerm) VarDeclName, (ATerm) VarDecl);
            }
          }
          i++;
        }
        if (!Consistent || (i == EqnsLength)) {
          //VarDeclTable represents the maximum consistent prefix of variable
          //declarations of Eqns starting at StartPrefix. Print this prefixa and
          //the corresponding equations,and if necessary, update StartPrefix and
          //reset VarDeclTable.
          ATermList VarDecls = ATtableValues(VarDeclTable);
          if (ATgetLength(VarDecls) > 0) {
            PRINT_FUNC(fprints)(OutStream, "var  ");
            PRINT_FUNC(PrintDecls)(OutStream, (pp_format == ppDebug)?VarDecls:gsGroupDeclsBySort(VarDecls),
              pp_format, ";\n", "     ");
          }
          if (gsIsDataEqn(Eqn)) {
            PRINT_FUNC(fprints)(OutStream, "eqn  ");
          } else if (gsIsProcEqn(Eqn)) {
            PRINT_FUNC(fprints)(OutStream, "proc ");
          } else if (gsIsActionRenameRule(Eqn)) {
            PRINT_FUNC(fprints)(OutStream, "rename\n     ");
          } else {
            assert(false);
          }
          PRINT_FUNC(PrintPart_List)(OutStream, ATgetSlice(Eqns, StartPrefix, i),
             pp_format, ShowSorts, PrecLevel, ";\n", "     ");
          if (i < EqnsLength) {
            PRINT_FUNC(fprints)(OutStream, "\n");
            StartPrefix = i;
            ATtableReset(VarDeclTable);
          }
        }
      }
      //finalisation after printing all (>0) equations
      PRINT_FUNC(fprints)(OutStream, "\n");
      ATtableDestroy(VarDeclTable);
    }
  }
}

void PRINT_FUNC(PrintDecls)(PRINT_OUTTYPE OutStream, const ATermList Decls,
  t_pp_format pp_format, const char *Terminator, const char *Separator)
{
  if (!ATisEmpty(Decls)) {
    ATermAppl Decl = ATAgetFirst(Decls);
    ATermList NextDecls = ATgetNext(Decls);
    while (!ATisEmpty(NextDecls))
    {
      if (ATisEqual(ATgetArgument(Decl, 1),
          ATgetArgument(ATAgetFirst(NextDecls), 1))) {
        PRINT_FUNC(PrintDecl)(OutStream, Decl, pp_format, false);
        PRINT_FUNC(fprints)(OutStream, ",");
      } else {
        PRINT_FUNC(PrintDecl)(OutStream, Decl, pp_format, true);
        if (Terminator  != NULL) PRINT_FUNC(fprints)(OutStream, Terminator);
        if (Separator  != NULL) PRINT_FUNC(fprints)(OutStream, Separator);
      }
      Decl = ATAgetFirst(NextDecls);
      NextDecls = ATgetNext(NextDecls);
    }
    PRINT_FUNC(PrintDecl)(OutStream, Decl, pp_format, true);
    if (Terminator  != NULL) PRINT_FUNC(fprints)(OutStream, Terminator);
  }
}

void PRINT_FUNC(PrintDecl)(PRINT_OUTTYPE OutStream, const ATermAppl Decl,
  t_pp_format pp_format, const bool ShowSorts)
{
  PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Decl, 0),
    pp_format, ShowSorts, 0);
  if (ShowSorts) {
    if (gsIsActId(Decl)) {
      ATermList SortExprs = ATLgetArgument(Decl, 1);
      if (ATgetLength(SortExprs) > 0) {
        PRINT_FUNC(fprints)(OutStream, ": ");
        PRINT_FUNC(PrintPart_List)(OutStream, SortExprs,
          pp_format, ShowSorts, 2, NULL, " # ");
      }
    } else {
      PRINT_FUNC(fprints)(OutStream, ": ");
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(Decl, 1),
        pp_format, ShowSorts, 0);
    }
  }
}

static void PRINT_FUNC(PrintSortExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl SortExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsSortExpr(SortExpr));
  if (gsIsSortId(SortExpr)) {
    //print sort identifier
    PRINT_FUNC(dbg_prints)("printing standard sort identifier\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(SortExpr, 0),
      pp_format, ShowSorts, PrecLevel);
  } else if (gsIsSortArrow(SortExpr)) {
    //print product arrow sort
    PRINT_FUNC(dbg_prints)("printing product arrow sort\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(SortExpr, 0),
      pp_format, ShowSorts, 1, NULL, " # ");
    PRINT_FUNC(fprints)(OutStream, " -> ");
    PRINT_FUNC(PrintSortExpr)(OutStream, ATAgetArgument(SortExpr, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSortExprList(SortExpr)) {
    //print list sort
    PRINT_FUNC(dbg_prints)("printing list sort\n");
    PRINT_FUNC(fprints)(OutStream, "List(");
    PRINT_FUNC(PrintSortExpr)(OutStream, ATAgetArgument(SortExpr, 1),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSortExprSet(SortExpr)) {
    //print set sort
    PRINT_FUNC(dbg_prints)("printing set sort\n");
    PRINT_FUNC(fprints)(OutStream, "Set(");
    PRINT_FUNC(PrintSortExpr)(OutStream, ATAgetArgument(SortExpr, 1),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSortExprBag(SortExpr)) {
    //print bag sort
    PRINT_FUNC(dbg_prints)("printing bag sort\n");
    PRINT_FUNC(fprints)(OutStream, "Bag(");
    PRINT_FUNC(PrintSortExpr)(OutStream, ATAgetArgument(SortExpr, 1),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSortStruct(SortExpr)) {
    //print structured sort
    PRINT_FUNC(dbg_prints)("printing structured sort\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(fprints)(OutStream, "struct ");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(SortExpr, 0),
      pp_format, ShowSorts, PrecLevel, NULL, " | ");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSortUnknown(SortExpr)) {
    //print unknown
    PRINT_FUNC(dbg_prints)("printing unknown\n");
    PRINT_FUNC(fprints)(OutStream, "unknown");
  } else if (gsIsSortsPossible(SortExpr)) {
    //print possible sorts
    PRINT_FUNC(dbg_prints)("printing possible sorts\n");
    PRINT_FUNC(fprints)(OutStream, "{");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(SortExpr, 0),
      pp_format, ShowSorts, 0, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, "}");
  }
}

void PRINT_FUNC(PrintDataExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl DataExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsDataExpr(DataExpr));
  if (gsIsId(DataExpr) || gsIsOpId(DataExpr) || gsIsDataVarId(DataExpr) ||
    gsIsDataAppl(DataExpr)) {
    if (pp_format == ppDebug) {
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(DataExpr, 0),
        pp_format, ShowSorts, 0);
      if (gsIsDataAppl(DataExpr)) {
        PRINT_FUNC(fprints)(OutStream, "(");
        //gsIsDataAppl(DataExpr)
        PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(DataExpr, 1),
          pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, ")");
      }
    } else { //pp_format == ppDefault
      //print data expression in the external format, if possible
      ATermAppl Head;
      ATermList Args;
      if (!gsIsDataAppl(DataExpr)) {
        Head = DataExpr;
        Args = ATmakeList0();
      } else {
        Head = ATAgetArgument(DataExpr, 0);
        Args = ATLgetArgument(DataExpr, 1);
      }
      int ArgsLength = ATgetLength(Args);
      if (gsIsDataExprListEnum(DataExpr)) {
        //print list enumeration
        PRINT_FUNC(dbg_prints)("printing list enumeration\n");
        PRINT_FUNC(fprints)(OutStream, "[");
        PRINT_FUNC(PrintPart_List)(OutStream, Args,
          pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, "]");
      } else if (gsIsDataExprSetEnum(DataExpr)) {
        //print set/bag enumeration
        PRINT_FUNC(dbg_prints)("printing set enumeration\n");
        PRINT_FUNC(fprints)(OutStream, "{");
        PRINT_FUNC(PrintPart_List)(OutStream, Args,
          pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, "}");
      } else if (gsIsDataExprBagEnum(DataExpr)) {
        //print bag enumeration
        PRINT_FUNC(fprints)(OutStream, "{");
        PRINT_FUNC(PrintPart_BagEnum)(OutStream, Args,
          pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, "}");
      } else if (gsIsOpIdNumericUpCast(Head) && ArgsLength == 1) {
        //print upcast expression
        PRINT_FUNC(dbg_prints)("printing upcast expression\n");
        PRINT_FUNC(PrintDataExpr)(OutStream, ATAelementAt(Args, 0),
          pp_format, ShowSorts, PrecLevel);
      } else if (gsIsOpIdPrefix(Head) && ArgsLength == 1) {
        //print prefix expression
        PRINT_FUNC(dbg_prints)("printing prefix expression\n");
        PRINT_FUNC(PrintPart_Appl)(OutStream, Head,
          pp_format, ShowSorts, PrecLevel);
        PRINT_FUNC(PrintDataExpr)(OutStream, ATAelementAt(Args, 0),
          pp_format, ShowSorts, gsPrecOpIdPrefix());
      } else if (gsIsOpIdInfix(Head) && ArgsLength == 2) {
        //print infix expression
        PRINT_FUNC(dbg_prints)("printing infix expression\n");
        ATermAppl HeadName = ATAgetArgument(Head, 0);
        if (PrecLevel > gsPrecOpIdInfix(HeadName))
          PRINT_FUNC(fprints)(OutStream, "(");
        PRINT_FUNC(PrintDataExpr)(OutStream, ATAelementAt(Args, 0),
          pp_format, ShowSorts, gsPrecOpIdInfixLeft(HeadName));
        PRINT_FUNC(fprints)(OutStream, " ");
        PRINT_FUNC(PrintPart_Appl)(OutStream, Head, pp_format, ShowSorts, PrecLevel);
        PRINT_FUNC(fprints)(OutStream, " ");
        PRINT_FUNC(PrintDataExpr)(OutStream, ATAelementAt(Args, 1),
          pp_format, ShowSorts, gsPrecOpIdInfixRight(HeadName));
        if (PrecLevel > gsPrecOpIdInfix(HeadName))
          PRINT_FUNC(fprints)(OutStream, ")");
      } else if (gsIsId(DataExpr)) {
        //print untyped data variable or operation identifier
        PRINT_FUNC(dbg_prints)("printing untyped data variable or operation identifier\n");
        PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(DataExpr, 0),
          pp_format, ShowSorts, PrecLevel);
      } else if (gsIsOpId(DataExpr) || gsIsDataVarId(DataExpr)) {
        //print data variable or operation identifier
        PRINT_FUNC(dbg_prints)("printing data variable or operation identifier\n");
        PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(DataExpr, 0),
          pp_format, ShowSorts, PrecLevel);
        if (ShowSorts) {
          PRINT_FUNC(fprints)(OutStream, ": ");
          PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(DataExpr, 1),
            pp_format, ShowSorts, 0);
        }
      } else {
        //print data application
        PRINT_FUNC(dbg_prints)("printing data application\n");
        PRINT_FUNC(PrintDataExpr)(OutStream, Head, pp_format, ShowSorts, gsPrecOpIdPrefix());
        PRINT_FUNC(fprints)(OutStream, "(");
        PRINT_FUNC(PrintPart_List)(OutStream, Args, pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, ")");
      }
    }
  } else if (gsIsBinder(DataExpr)) {
    PRINT_FUNC(dbg_prints)("printing binder\n");
    ATermAppl BindingOperator = ATAgetArgument(DataExpr, 0);
    if (gsIsSetBagComp(BindingOperator) || gsIsSetComp(BindingOperator)
        || gsIsBagComp(BindingOperator)) {
      //print set/bag comprehension
      PRINT_FUNC(dbg_prints)("printing set/bag comprehension\n");
      PRINT_FUNC(fprints)(OutStream, "{ ");
      assert(!ATisEmpty(ATLgetArgument(DataExpr,1)));
      PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(DataExpr, 1),
        pp_format, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, " | ");
      PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(DataExpr, 2),
        pp_format, ShowSorts, 0);
      PRINT_FUNC(fprints)(OutStream, " }");
    } else if (gsIsLambda(BindingOperator) || gsIsForall(BindingOperator) 
            || gsIsExists(BindingOperator)) {
      //print lambda abstraction or universal/existential quantification
      PRINT_FUNC(dbg_prints)("printing lambda abstraction or universal/existential quantification\n");
      if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(fprints)(OutStream, (gsIsLambda(BindingOperator)?"lambda ":
                                     (gsIsForall(BindingOperator))?"forall ":
                                     "exists "));
      PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(DataExpr, 1),
        pp_format, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ". ");
      PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(DataExpr, 2),
        pp_format, ShowSorts, 1);
      if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsWhr(DataExpr)) {
    //print where clause
    PRINT_FUNC(dbg_prints)("printing where clause\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(DataExpr, 0),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, " whr ");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(DataExpr, 1),
      pp_format, ShowSorts, PrecLevel, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, " end");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  }
}

static void PRINT_FUNC(PrintProcExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl ProcExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsProcExpr(ProcExpr));
  if (gsIsParamId(ProcExpr) || gsIsAction(ProcExpr) || gsIsProcess(ProcExpr)) {
    //print action or process reference
    PRINT_FUNC(dbg_prints)("printing action or process reference\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(ProcExpr, 1);
    if (ATgetLength(Args) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, Args,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsDelta(ProcExpr)) {
    //print delta
    PRINT_FUNC(dbg_prints)("printing delta\n");
    PRINT_FUNC(fprints)(OutStream, "delta");
  } else if (gsIsTau(ProcExpr)) {
    //print tau
    PRINT_FUNC(dbg_prints)("printing tau\n");
    PRINT_FUNC(fprints)(OutStream, "tau");
  } else if (gsIsChoice(ProcExpr)) {
    //print choice
    PRINT_FUNC(dbg_prints)("printing choice\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 1);
    PRINT_FUNC(fprints)(OutStream, " + ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSum(ProcExpr)) {
    //print summation
    PRINT_FUNC(dbg_prints)("printing summation\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(fprints)(OutStream, "sum ");
    PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(ProcExpr, 0),
      pp_format, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, ". ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 1);
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsMerge(ProcExpr) || gsIsLMerge(ProcExpr)) {
    //print merge or left merge
    PRINT_FUNC(dbg_prints)("printing merge or left merge\n");
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 3);
    if (gsIsMerge(ProcExpr)) {
      PRINT_FUNC(fprints)(OutStream, " || ");
    } else {
      PRINT_FUNC(fprints)(OutStream, " ||_ ");
    }
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 2);
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsBInit(ProcExpr)) {
    //print bounded initialisation
    PRINT_FUNC(dbg_prints)("printing bounded initialisation\n");
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 4);
    PRINT_FUNC(fprints)(OutStream, " << ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 3);
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsIfThen(ProcExpr)) {
    //print if then 
    PRINT_FUNC(dbg_prints)("printing if then\n");
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
    PRINT_FUNC(fprints)(OutStream, " -> ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 5);
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsIfThenElse(ProcExpr)) {
    //print if then else
    PRINT_FUNC(dbg_prints)("printing if then else\n");
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
    PRINT_FUNC(fprints)(OutStream, " -> ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 5);
    ATermAppl ProcExprElse = ATAgetArgument(ProcExpr, 2);
    PRINT_FUNC(fprints)(OutStream, " <> ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ProcExprElse,
      pp_format, ShowSorts, 5);
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSeq(ProcExpr)) {
    //print sequential composition
    PRINT_FUNC(dbg_prints)("printing sequential composition\n");
    if (PrecLevel > 5) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 6);
    PRINT_FUNC(fprints)(OutStream, " . ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 5);
    if (PrecLevel > 5) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsAtTime(ProcExpr)) {
    //print at expression
    PRINT_FUNC(dbg_prints)("printing at expression\n");
    if (PrecLevel > 6) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 6);
    PRINT_FUNC(fprints)(OutStream, " @ ");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
    if (PrecLevel > 6) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsSync(ProcExpr)) {
    //print sync
    PRINT_FUNC(dbg_prints)("printing sync\n");
    if (PrecLevel > 7) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 8);
    PRINT_FUNC(fprints)(OutStream, " | ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 7);
    if (PrecLevel > 7) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsBlock(ProcExpr) || gsIsHide(ProcExpr) || gsIsRename(ProcExpr) ||
      gsIsComm(ProcExpr) || gsIsAllow(ProcExpr)) {
    //print process quantification
    PRINT_FUNC(dbg_prints)("printing process quantification\n");
    if (gsIsBlock(ProcExpr)) {
      PRINT_FUNC(fprints)(OutStream, "block");
    } else if (gsIsHide(ProcExpr)) {
      PRINT_FUNC(fprints)(OutStream, "hide");
    } else if (gsIsRename(ProcExpr)) {
      PRINT_FUNC(fprints)(OutStream, "rename");
    } else if (gsIsComm(ProcExpr)) {
      PRINT_FUNC(fprints)(OutStream, "comm");
    } else {
      PRINT_FUNC(fprints)(OutStream, "allow");
    }
    PRINT_FUNC(fprints)(OutStream, "({");
    PRINT_FUNC(PrintPart_List)(OutStream, ATLgetArgument(ProcExpr, 0),
      pp_format, ShowSorts, 0, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, "}, ");
    PRINT_FUNC(PrintProcExpr)(OutStream, ATAgetArgument(ProcExpr, 1),
      pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  }
}

static void PRINT_FUNC(PrintStateFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl StateFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsStateFrm(StateFrm));
  if (gsIsDataExpr(StateFrm)) {
    //print data expression
    PRINT_FUNC(dbg_prints)("printing data expression\n");
    PRINT_FUNC(fprints)(OutStream, "val(");
    PRINT_FUNC(PrintDataExpr)(OutStream, StateFrm, pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateTrue(StateFrm)) {
    //print true
    PRINT_FUNC(dbg_prints)("printing true\n");
    PRINT_FUNC(fprints)(OutStream, "true");
  } else if (gsIsStateFalse(StateFrm)) {
    //print false
    PRINT_FUNC(dbg_prints)("printing false\n");
    PRINT_FUNC(fprints)(OutStream, "false");
  } else if (gsIsStateYaled(StateFrm)) {
    //print yaled
    PRINT_FUNC(dbg_prints)("printing yaled\n");
    PRINT_FUNC(fprints)(OutStream, "yaled");
  } else if (gsIsStateDelay(StateFrm)) {
    //print delay
    PRINT_FUNC(dbg_prints)("printing delay\n");
    PRINT_FUNC(fprints)(OutStream, "delay");
  } else if (gsIsStateYaledTimed(StateFrm)) {
    //print timed yaled
    PRINT_FUNC(dbg_prints)("printing timed yaled\n");
    PRINT_FUNC(fprints)(OutStream, "yaled @ ");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
  } else if (gsIsStateDelayTimed(StateFrm)) {
    //print timed delay
    PRINT_FUNC(dbg_prints)("printing timed delay\n");
    PRINT_FUNC(fprints)(OutStream, "delay @ ");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
  } else if (gsIsStateVar(StateFrm)) {
    //print fixpoint variable
    PRINT_FUNC(dbg_prints)("printing fixpoint variable\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(StateFrm, 1);
    if (ATgetLength(Args) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, Args,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm)) {
    //print quantification
    PRINT_FUNC(dbg_prints)("printing quantification\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    if (gsIsStateForall(StateFrm)) {
      PRINT_FUNC(fprints)(OutStream, "forall ");
    } else {
      PRINT_FUNC(fprints)(OutStream, "exists ");
    }
    PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(StateFrm, 0),
      pp_format, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, ". ");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm)) {
    //print fixpoint quantification
    PRINT_FUNC(dbg_prints)("printing fixpoint quantification\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    if (gsIsStateNu(StateFrm)) {
      PRINT_FUNC(fprints)(OutStream, "nu ");
    } else {
      PRINT_FUNC(fprints)(OutStream, "mu ");
    }
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList DataVarInits = ATLgetArgument(StateFrm, 1);
    if (ATgetLength(DataVarInits) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, DataVarInits,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
    PRINT_FUNC(fprints)(OutStream, ". ");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 2),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateImp(StateFrm)) {
    //print implication
    PRINT_FUNC(dbg_prints)("printing implication\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, 2);
    PRINT_FUNC(fprints)(OutStream, " => ");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 1),
      pp_format, ShowSorts, 1);
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm)) {
    //print conjunction or disjunction
    PRINT_FUNC(dbg_prints)("printing conjunction or disjunction\n");
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, 3);
    if (gsIsStateAnd(StateFrm)) {
      PRINT_FUNC(fprints)(OutStream, " && ");
    } else {
      PRINT_FUNC(fprints)(OutStream, " || ");
    }
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 1),
      pp_format, ShowSorts, 2);
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateNot(StateFrm)) {
    //print negation
    PRINT_FUNC(dbg_prints)("printing negation\n");
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(fprints)(OutStream, "!");
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, 3);
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsStateMust(StateFrm) || gsIsStateMay(StateFrm)) {
    //print must or may
    PRINT_FUNC(dbg_prints)("printing must or may\n");
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, "(");
    if (gsIsStateMust(StateFrm)) {
      PRINT_FUNC(fprints)(OutStream, "[");
    } else {
      PRINT_FUNC(fprints)(OutStream, "<");
    }
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(StateFrm, 0),
      pp_format, ShowSorts, 0);
    if (gsIsStateMust(StateFrm)) {
      PRINT_FUNC(fprints)(OutStream, "]");
    } else {
      PRINT_FUNC(fprints)(OutStream, ">");
    }
    PRINT_FUNC(PrintStateFrm)(OutStream, ATAgetArgument(StateFrm, 1),
      pp_format, ShowSorts, 3);
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, ")");
  }
}

static void PRINT_FUNC(PrintRegFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl RegFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsRegFrm(RegFrm));
  if (gsIsActFrm(RegFrm)) {
    //print regular formula
    PRINT_FUNC(dbg_prints)("printing action formula\n");
    PRINT_FUNC(PrintActFrm)(OutStream, RegFrm, pp_format, ShowSorts, 0);
  } else if (gsIsRegNil(RegFrm)) {
    //print nil
    PRINT_FUNC(dbg_prints)("printing nil\n");
    PRINT_FUNC(fprints)(OutStream, "nil");
  } else if (gsIsRegAlt(RegFrm)) {
    //print alternative composition
    PRINT_FUNC(dbg_prints)("printing alternative composition\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(RegFrm, 0),
      pp_format, ShowSorts, 1);
    PRINT_FUNC(fprints)(OutStream, " + ");
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(RegFrm, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsRegSeq(RegFrm)) {
    //print sequential composition
    PRINT_FUNC(dbg_prints)("printing sequential composition\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(RegFrm, 0),
      pp_format, ShowSorts, 2);
    PRINT_FUNC(fprints)(OutStream, " . ");
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(RegFrm, 1),
      pp_format, ShowSorts, 1);
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsRegTransOrNil(RegFrm) || gsIsRegTrans(RegFrm)) {
    //print (reflexive) transitive closure
    PRINT_FUNC(dbg_prints)("printing (reflexive) transitive closure\n");
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintRegFrm)(OutStream, ATAgetArgument(RegFrm, 0),
      pp_format, ShowSorts, 2);
    if (gsIsRegTransOrNil(RegFrm)) {
      PRINT_FUNC(fprints)(OutStream, "*");
    } else {
      PRINT_FUNC(fprints)(OutStream, "+");
    }
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, ")");
  }
}

static void PRINT_FUNC(PrintActFrm)(PRINT_OUTTYPE OutStream,
  const ATermAppl ActFrm, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsActFrm(ActFrm));
  if (gsIsDataExpr(ActFrm)) {
    //print data expression
    PRINT_FUNC(dbg_prints)("printing data expression\n");
    PRINT_FUNC(fprints)(OutStream, "val(");
    PRINT_FUNC(PrintDataExpr)(OutStream, ActFrm, pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsMultAct(ActFrm)) {
    //print multiaction
    PRINT_FUNC(PrintPart_Appl)(OutStream, ActFrm, pp_format, ShowSorts, 0);
  } else if (gsIsActTrue(ActFrm)) {
    //print true
    PRINT_FUNC(dbg_prints)("printing true\n");
    PRINT_FUNC(fprints)(OutStream, "true");
  } else if (gsIsActFalse(ActFrm)) {
    //print false
    PRINT_FUNC(dbg_prints)("printing false\n");
    PRINT_FUNC(fprints)(OutStream, "false");
  } else if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm)) {
    //print quantification
    PRINT_FUNC(dbg_prints)("printing quantification\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    if (gsIsActForall(ActFrm)) {
      PRINT_FUNC(fprints)(OutStream, "forall ");
    } else {
      PRINT_FUNC(fprints)(OutStream, "exists ");
    }
    PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(ActFrm, 0),
      pp_format, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, ". ");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsActImp(ActFrm)) {
    //print implication
    PRINT_FUNC(dbg_prints)("printing implication\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 0),
      pp_format, ShowSorts, 2);
    PRINT_FUNC(fprints)(OutStream, " => ");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 1),
      pp_format, ShowSorts, 1);
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm)) {
    //print conjunction or disjunction
    PRINT_FUNC(dbg_prints)("printing conjunction or disjunction\n");
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 0),
      pp_format, ShowSorts, 3);
    if (gsIsActAnd(ActFrm)) {
      PRINT_FUNC(fprints)(OutStream, " && ");
    } else {
      PRINT_FUNC(fprints)(OutStream, " || ");
    }
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 1),
      pp_format, ShowSorts, 2);
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsActAt(ActFrm)) {
    //print at expression
    PRINT_FUNC(dbg_prints)("printing at expression\n");
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 0),
      pp_format, ShowSorts, 3);
    PRINT_FUNC(fprints)(OutStream, " @ ");
    PRINT_FUNC(PrintDataExpr)(OutStream, ATAgetArgument(ActFrm, 1),
      pp_format, ShowSorts, gsPrecOpIdPrefix());
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsActNot(ActFrm)) {
    //print negation
    PRINT_FUNC(dbg_prints)("printing negation\n");
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(fprints)(OutStream, "!");
    PRINT_FUNC(PrintActFrm)(OutStream, ATAgetArgument(ActFrm, 0),
      pp_format, ShowSorts, 4);
    if (PrecLevel > 4) PRINT_FUNC(fprints)(OutStream, ")");
  }
}

static void PRINT_FUNC(PrintPBExpr)(PRINT_OUTTYPE OutStream,
  const ATermAppl PBExpr, t_pp_format pp_format, bool ShowSorts, int PrecLevel)
{
  assert(gsIsPBExpr(PBExpr));
  if (gsIsDataExpr(PBExpr)) {
    //print data expression
    PRINT_FUNC(dbg_prints)("printing data expression\n");
    PRINT_FUNC(fprints)(OutStream, "val(");
    PRINT_FUNC(PrintDataExpr)(OutStream, PBExpr, pp_format, ShowSorts, 0);
    PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsPBESTrue(PBExpr)) {
    //print true
    PRINT_FUNC(dbg_prints)("printing true\n");
    PRINT_FUNC(fprints)(OutStream, "true");
  } else if (gsIsPBESFalse(PBExpr)) {
    //print false
    PRINT_FUNC(dbg_prints)("printing false\n");
    PRINT_FUNC(fprints)(OutStream, "false");
  } else if (gsIsPropVarInst(PBExpr)) {
    //print propositional variable instance
    PRINT_FUNC(dbg_prints)("printing propositional variable instance\n");
    PRINT_FUNC(PrintPart_Appl)(OutStream, ATAgetArgument(PBExpr, 0),
      pp_format, ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(PBExpr, 1);
    if (ATgetLength(Args) > 0) {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, Args,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
  } else if (gsIsPBESForall(PBExpr) || gsIsPBESExists(PBExpr)) {
    //print quantification
    PRINT_FUNC(dbg_prints)("printing quantification\n");
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, "(");
    if (gsIsPBESForall(PBExpr)) {
      PRINT_FUNC(fprints)(OutStream, "forall ");
    } else {
      PRINT_FUNC(fprints)(OutStream, "exists ");
    }
    PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(PBExpr, 0),
      pp_format, NULL, ", ");
    PRINT_FUNC(fprints)(OutStream, ". ");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 1),
      pp_format, ShowSorts, 0);
    if (PrecLevel > 0) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsPBESImp(PBExpr)) {
    //print implication
    PRINT_FUNC(dbg_prints)("printing implication\n");
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 0),
      pp_format, ShowSorts, 2);
    PRINT_FUNC(fprints)(OutStream, " => ");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 1),
      pp_format, ShowSorts, 1);
    if (PrecLevel > 1) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsPBESAnd(PBExpr) || gsIsPBESOr(PBExpr)) {
    //print conjunction or disjunction
    PRINT_FUNC(dbg_prints)("printing conjunction or disjunction\n");
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 0),
      pp_format, ShowSorts, 3);
    if (gsIsPBESAnd(PBExpr)) {
      PRINT_FUNC(fprints)(OutStream, " && ");
    } else {
      PRINT_FUNC(fprints)(OutStream, " || ");
    }
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 1),
      pp_format, ShowSorts, 2);
    if (PrecLevel > 2) PRINT_FUNC(fprints)(OutStream, ")");
  } else if (gsIsPBESNot(PBExpr)) {
    //print negation
    PRINT_FUNC(dbg_prints)("printing negation\n");
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, "(");
    PRINT_FUNC(fprints)(OutStream, "!");
    PRINT_FUNC(PrintPBExpr)(OutStream, ATAgetArgument(PBExpr, 0),
      pp_format, ShowSorts, 3);
    if (PrecLevel > 3) PRINT_FUNC(fprints)(OutStream, ")");
  }
}

void PRINT_FUNC(PrintLinearProcessSummand)(PRINT_OUTTYPE OutStream,
  const ATermAppl Summand, const ATermList VarDecls,
  t_pp_format pp_format, bool ShowSorts)
{
  assert(gsIsLinearProcessSummand(Summand));
  PRINT_FUNC(dbg_prints)("printing LPS summand\n");
  //print data summations
  ATermList SumVarDecls = ATLgetArgument(Summand, 0);
  if (ATgetLength(SumVarDecls) > 0) {
    PRINT_FUNC(fprints)(OutStream, "sum ");
    PRINT_FUNC(PrintDecls)(OutStream, SumVarDecls, pp_format, NULL, ",");
    PRINT_FUNC(fprints)(OutStream, ".\n         ");
  }
  //print condition
  ATermAppl Cond = ATAgetArgument(Summand, 1);
  if (!gsIsNil(Cond)) {
    PRINT_FUNC(PrintDataExpr)(OutStream, Cond, pp_format, ShowSorts, gsPrecOpIdPrefix());
    PRINT_FUNC(fprints)(OutStream, " ->\n         ");
  }
  //print multiaction
  ATermAppl MultAct = ATAgetArgument(Summand, 2);
  ATermAppl Time = ATAgetArgument(Summand, 3);
  bool IsTimed = !gsIsNil(Time);
  PRINT_FUNC(PrintPart_Appl)(OutStream, MultAct, pp_format, ShowSorts,
    (IsTimed)?6:5);
  //print time
  if (IsTimed) {
    PRINT_FUNC(fprints)(OutStream, " @ ");
    PRINT_FUNC(PrintDataExpr)(OutStream, Time, pp_format, ShowSorts, gsPrecOpIdPrefix());
  }
  //print process reference
  if (!gsIsDelta(MultAct)) {
    PRINT_FUNC(fprints)(OutStream, " .\n         ");
    PRINT_FUNC(fprints)(OutStream, "P");
    ATermList Assignments = ATLgetArgument(Summand, 4);
    if (VarDecls != NULL && pp_format == ppDefault) {
      if (ATgetLength(VarDecls) > 0) {
        PRINT_FUNC(fprints)(OutStream, "(");
        PRINT_FUNC(PrintPart_List)(OutStream,
          SubstituteAssignments_list(VarDecls, Assignments),
          pp_format, ShowSorts, 0, NULL, ", ");
        PRINT_FUNC(fprints)(OutStream, ")");
      }
    } else {
      PRINT_FUNC(fprints)(OutStream, "(");
      PRINT_FUNC(PrintPart_List)(OutStream, Assignments,
        pp_format, ShowSorts, 0, NULL, ", ");
      PRINT_FUNC(fprints)(OutStream, ")");
    }
  }
}

ATermList GetAssignmentsRHS(ATermList Assignments) {
  ATermList l = ATmakeList0();
  while (!ATisEmpty(Assignments)) {
    l = ATinsert(l, ATgetArgument(ATAgetFirst(Assignments), 1));
    Assignments = ATgetNext(Assignments);
  }
  return ATreverse(l);
}

ATermList SubstituteAssignments_list(ATermList Vars, ATermList Assignments)
{
  ATermList l = ATmakeList0();
  while (!ATisEmpty(Vars)) {
    l = ATinsert(l, (ATerm) SubstituteAssignments_appl(ATAgetFirst(Vars), Assignments));
    Vars = ATgetNext(Vars);
  }
  return ATreverse(l);
}

ATermAppl SubstituteAssignments_appl(ATermAppl Var, ATermList Assignments)
{
  ATermAppl Result = Var;
  bool found = false;
  while (!ATisEmpty(Assignments) && !found) {
    ATermAppl Assignment = ATAgetFirst(Assignments);
    if (ATisEqual(Var, ATAgetArgument(Assignment, 0))) {
      Result = ATAgetArgument(Assignment, 1);
      found = true;
    }
    Assignments = ATgetNext(Assignments);
  }
  return Result;
}

ATermList gsGroupDeclsBySort(ATermList Decls)
{
  if (!ATisEmpty(Decls)) {
    ATermTable SortDeclsTable = ATtableCreate(2*ATgetLength(Decls), 50);
    //Add all variable declarations from Decls to hash table
    //SortDeclsTable
    while (!ATisEmpty(Decls))
    {
      ATermAppl Decl = ATAgetFirst(Decls);
      ATermAppl DeclSort = ATAgetArgument(Decl, 1);
      ATermList CorDecls = ATLtableGet(SortDeclsTable, (ATerm) DeclSort);
      ATtablePut(SortDeclsTable, (ATerm) DeclSort,
        (CorDecls == NULL)
          ?(ATerm) ATmakeList1((ATerm) Decl)
          :(ATerm) ATinsert(CorDecls, (ATerm) Decl)
      );
      Decls = ATgetNext(Decls);
    }
    //Return the hash table as a list of variable declarations
    ATermList DeclSorts = ATtableKeys(SortDeclsTable);
    ATermList Result = ATmakeList0();
    while (!ATisEmpty(DeclSorts))
    {
      Result = ATconcat(
        ATLtableGet(SortDeclsTable, ATgetFirst(DeclSorts)),
        Result);
      DeclSorts = ATgetNext(DeclSorts);
    }
    ATtableDestroy(SortDeclsTable);
    return ATreverse(Result);
  } else {
    //Decls is empty
    return Decls;
  }
}

bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part)
{
  bool Result = true;
  if (gsIsDataEqn(Part) || gsIsProcEqn(Part)) {
    //check consistency of DataVarDecls with the variable declarations
    ATermList VarDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(VarDecls);
    for (int i = 0; i < n && Result; i++) {
      //check consistency of variable VarDecls(j) with VarDeclTable
      ATermAppl VarDecl = ATAelementAt(VarDecls, i);
      ATermAppl CorVarDecl =
        ATAtableGet(DataVarDecls, ATgetArgument(VarDecl, 0));
      if (CorVarDecl != NULL) {
        //check consistency of VarDecl with CorVarDecl
        Result = (ATisEqual(VarDecl, CorVarDecl) == ATtrue);
      }
    }
  } else if (gsIsOpId(Part) || gsIsId(Part))  {
    //Part may be an operation; check that its name does not occur in
    //DataVarDecls
    Result = (ATtableGet(DataVarDecls, ATgetArgument(Part, 0)) == NULL);
  }
  //check consistency in the arguments of Part
  if (Result) {
    AFun Head = ATgetAFun(Part);
    int NrArgs = ATgetArity(Head);      
    for (int i = 0; i < NrArgs && Result; i++) {
      ATerm Arg = ATgetArgument(Part, i);
      if (ATgetType(Arg) == AT_APPL)
        Result = gsHasConsistentContext(DataVarDecls, (ATermAppl) Arg);
      else //ATgetType(Arg) == AT_LIST
        Result = gsHasConsistentContextList(DataVarDecls, (ATermList) Arg);
    }
  }
  return Result;
}

bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts)
{
  bool Result = true;
  ATermList l = Parts;
  while (!ATisEmpty(l) && Result) {
    Result = gsHasConsistentContext(DataVarDecls, ATAgetFirst(l));
    l = ATgetNext(l);
  }
  return Result;
}

bool gsIsOpIdNumericUpCast(ATermAppl Term)
{
  if (!gsIsOpId(Term)) {
    return false;
  }
  return
    (Term == gsMakeOpIdPos2Nat())  ||
    (Term == gsMakeOpIdPos2Int())  ||
    (Term == gsMakeOpIdPos2Real()) ||
    (Term == gsMakeOpIdNat2Int())  ||
    (Term == gsMakeOpIdNat2Real()) ||
    (Term == gsMakeOpIdInt2Real())
    ;
}

bool gsIsOpIdPrefix(ATermAppl Term)
{
  if (!gsIsOpId(Term)) {
    return false;
  }
  ATermAppl TermSort = ATAgetArgument(Term, 1);
  if (!gsIsSortArrow(TermSort)) {
    return false;
  }
  if (ATgetLength(ATLgetArgument(TermSort, 0)) != 1) {
    return false;
  }
  ATermAppl OpIdName = ATAgetArgument(Term, 0);
  return
     (OpIdName == gsMakeOpIdNameNot())      ||
     (OpIdName == gsMakeOpIdNameNeg())      ||
     (OpIdName == gsMakeOpIdNameListSize()) ||
     (OpIdName == gsMakeOpIdNameSetCompl()); 
}

bool gsIsOpIdInfix(ATermAppl Term)
{
  if (!gsIsOpId(Term)) {
    return false;
  }
  ATermAppl TermSort = ATAgetArgument(Term, 1);
  if (!gsIsSortArrow(TermSort)) {
    return false;
  }
  if (ATgetLength(ATLgetArgument(TermSort, 0)) != 2) {
    return false;
  }
  ATermAppl OpIdName = ATAgetArgument(Term, 0);
  return
     (OpIdName == gsMakeOpIdNameImp())          ||
     (OpIdName == gsMakeOpIdNameAnd())          ||
     (OpIdName == gsMakeOpIdNameOr())           ||
     (OpIdName == gsMakeOpIdNameEq())           ||
     (OpIdName == gsMakeOpIdNameNeq())          ||
     (OpIdName == gsMakeOpIdNameLT())           ||
     (OpIdName == gsMakeOpIdNameLTE())          ||
     (OpIdName == gsMakeOpIdNameGT())           ||
     (OpIdName == gsMakeOpIdNameGTE())          ||
     (OpIdName == gsMakeOpIdNameEltIn())        ||
     (OpIdName == gsMakeOpIdNameSubSetEq())     ||
     (OpIdName == gsMakeOpIdNameSubSet())       ||
     (OpIdName == gsMakeOpIdNameSubBagEq())     ||
     (OpIdName == gsMakeOpIdNameSubBag())       ||
     (OpIdName == gsMakeOpIdNameCons())         ||
     (OpIdName == gsMakeOpIdNameSnoc())         ||
     (OpIdName == gsMakeOpIdNameConcat())       ||
     (OpIdName == gsMakeOpIdNameAdd())          ||
     (OpIdName == gsMakeOpIdNameSubt())         ||
     (OpIdName == gsMakeOpIdNameSetUnion())     ||
     (OpIdName == gsMakeOpIdNameSetDiff())      ||
     (OpIdName == gsMakeOpIdNameBagUnion())     ||
     (OpIdName == gsMakeOpIdNameBagDiff())      ||
     (OpIdName == gsMakeOpIdNameDiv())          ||
     (OpIdName == gsMakeOpIdNameMod())          ||
     (OpIdName == gsMakeOpIdNameMult())         ||
     (OpIdName == gsMakeOpIdNameEltAt())        ||
     (OpIdName == gsMakeOpIdNameSetIntersect()) ||
     (OpIdName == gsMakeOpIdNameBagIntersect());
}

int gsPrecOpIdPrefix()
{
  return 13;
}

int gsPrecOpIdInfix(ATermAppl OpIdName)
{
  if (OpIdName == gsMakeOpIdNameImp()) {
    return 2;
  } else if ((OpIdName == gsMakeOpIdNameAnd()) || (OpIdName == gsMakeOpIdNameOr())) {
    return 3;
  } else if ((OpIdName == gsMakeOpIdNameEq()) || (OpIdName == gsMakeOpIdNameNeq())) {
    return 4;
  } else if (
      (OpIdName == gsMakeOpIdNameLT()) || (OpIdName == gsMakeOpIdNameLTE()) ||
      (OpIdName == gsMakeOpIdNameGT()) || (OpIdName == gsMakeOpIdNameGTE()) ||
      (OpIdName == gsMakeOpIdNameEltIn()) ||
      (OpIdName == gsMakeOpIdNameSubSetEq()) || (OpIdName == gsMakeOpIdNameSubSet()) ||
      (OpIdName == gsMakeOpIdNameSubBagEq()) || (OpIdName == gsMakeOpIdNameSubBag())
      ) {
    return 5;
  } else if ((OpIdName == gsMakeOpIdNameCons())) {
    return 6;
  } else if ((OpIdName == gsMakeOpIdNameSnoc())) {
    return 7;
  } else if ((OpIdName == gsMakeOpIdNameConcat())) {
    return 8;
  } else if (
      (OpIdName == gsMakeOpIdNameAdd()) || (OpIdName == gsMakeOpIdNameSubt()) ||
      (OpIdName == gsMakeOpIdNameSetUnion()) || (OpIdName == gsMakeOpIdNameSetDiff()) ||
      (OpIdName == gsMakeOpIdNameBagUnion()) || (OpIdName == gsMakeOpIdNameBagDiff())
      ) {
    return 9;
  } else if ((OpIdName == gsMakeOpIdNameDiv()) || (OpIdName == gsMakeOpIdNameMod())) {
    return 10;
  } else if (
      (OpIdName == gsMakeOpIdNameMult()) || (OpIdName == gsMakeOpIdNameEltAt()) ||
      (OpIdName == gsMakeOpIdNameSetIntersect()) ||
      (OpIdName == gsMakeOpIdNameBagIntersect())
      ){
    return 11;
  } else {
    //something went wrong
    return -1;
  }
}

int gsPrecOpIdInfixLeft(ATermAppl OpIdName)
{
  if (OpIdName == gsMakeOpIdNameImp()) {
    return 3;
  } else if ((OpIdName == gsMakeOpIdNameAnd()) || (OpIdName == gsMakeOpIdNameOr())) {
    return 4;
  } else if ((OpIdName == gsMakeOpIdNameEq()) || (OpIdName == gsMakeOpIdNameNeq())) {
    return 5;
  } else if (
      (OpIdName == gsMakeOpIdNameLT()) || (OpIdName == gsMakeOpIdNameLTE()) ||
      (OpIdName == gsMakeOpIdNameGT()) || (OpIdName == gsMakeOpIdNameGTE()) ||
      (OpIdName == gsMakeOpIdNameEltIn()) ||
      (OpIdName == gsMakeOpIdNameSubSetEq()) || (OpIdName == gsMakeOpIdNameSubSet()) ||
      (OpIdName == gsMakeOpIdNameSubBagEq()) || (OpIdName == gsMakeOpIdNameSubBag())
      ) {
    return 6;
  } else if ((OpIdName == gsMakeOpIdNameCons())) {
    return 9;
  } else if ((OpIdName == gsMakeOpIdNameSnoc())) {
    return 7;
  } else if ((OpIdName == gsMakeOpIdNameConcat())) {
    return 8;
  } else if (
      (OpIdName == gsMakeOpIdNameAdd()) || (OpIdName == gsMakeOpIdNameSubt()) ||
      (OpIdName == gsMakeOpIdNameSetUnion()) || (OpIdName == gsMakeOpIdNameSetDiff()) ||
      (OpIdName == gsMakeOpIdNameBagUnion()) || (OpIdName == gsMakeOpIdNameBagDiff())
      ) {
    return 9;
  } else if ((OpIdName == gsMakeOpIdNameDiv()) || (OpIdName == gsMakeOpIdNameMod())) {
    return 10;
  } else if (
      (OpIdName == gsMakeOpIdNameMult()) || (OpIdName == gsMakeOpIdNameEltAt()) ||
      (OpIdName == gsMakeOpIdNameSetIntersect()) ||
      (OpIdName == gsMakeOpIdNameBagIntersect())
      ){
    return 11;
  } else {
    //something went wrong
    return -1;
  }
}

int gsPrecOpIdInfixRight(ATermAppl OpIdName)
{
  if (OpIdName == gsMakeOpIdNameImp()) {
    return 2;
  } else if ((OpIdName == gsMakeOpIdNameAnd()) || (OpIdName == gsMakeOpIdNameOr())) {
    return 3;
  } else if ((OpIdName == gsMakeOpIdNameEq()) || (OpIdName == gsMakeOpIdNameNeq())) {
    return 4;
  } else if (
      (OpIdName == gsMakeOpIdNameLT()) || (OpIdName == gsMakeOpIdNameLTE()) ||
      (OpIdName == gsMakeOpIdNameGT()) || (OpIdName == gsMakeOpIdNameGTE()) ||
      (OpIdName == gsMakeOpIdNameEltIn()) ||
      (OpIdName == gsMakeOpIdNameSubSetEq()) || (OpIdName == gsMakeOpIdNameSubSet()) ||
      (OpIdName == gsMakeOpIdNameSubBagEq()) || (OpIdName == gsMakeOpIdNameSubBag())
      ) {
    return 6;
  } else if ((OpIdName == gsMakeOpIdNameCons())) {
    return 6;
  } else if ((OpIdName == gsMakeOpIdNameSnoc())) {
    return 9;
  } else if ((OpIdName == gsMakeOpIdNameConcat())) {
    return 9;
  } else if (
      (OpIdName == gsMakeOpIdNameAdd()) || (OpIdName == gsMakeOpIdNameSubt()) ||
      (OpIdName == gsMakeOpIdNameSetUnion()) || (OpIdName == gsMakeOpIdNameSetDiff()) ||
      (OpIdName == gsMakeOpIdNameBagUnion()) || (OpIdName == gsMakeOpIdNameBagDiff())
      ) {
    return 10;
  } else if ((OpIdName == gsMakeOpIdNameDiv()) || (OpIdName == gsMakeOpIdNameMod())) {
    return 11;
  } else if (
      (OpIdName == gsMakeOpIdNameMult()) || (OpIdName == gsMakeOpIdNameEltAt()) ||
      (OpIdName == gsMakeOpIdNameSetIntersect()) ||
      (OpIdName == gsMakeOpIdNameBagIntersect())
      ){
    return 12;
  } else {
    //something went wrong
    return -1;
  }
}
    } //namespace detail
  }   //namespace core
}     //namespace mcrl2
