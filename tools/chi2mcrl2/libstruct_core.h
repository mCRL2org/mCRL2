// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_core.h

#ifndef MCRL2_LIBSTRUCT_CORE_H
#define MCRL2_LIBSTRUCT_CORE_H

//This file describes the functions that can be used for the internal ATerm
//structure.

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <aterm2.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

// TODO: remove this function.
inline
void gsEnableCoreConstructorFunctions()
{}

//--- begin generated code
// Assignment
inline
AFun initAFunAssignment(AFun& f)
{
  f = ATmakeAFun("Assignment", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunAssignment()
{
  static AFun AFunAssignment = initAFunAssignment(AFunAssignment);
  return AFunAssignment;
}

inline
bool gsIsAssignment(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunAssignment();
}

// BinaryExpression
inline
AFun initAFunBinaryExpression(AFun& f)
{
  f = ATmakeAFun("BinaryExpression", 4, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunBinaryExpression()
{
  static AFun AFunBinaryExpression = initAFunBinaryExpression(AFunBinaryExpression);
  return AFunBinaryExpression;
}

inline
bool gsIsBinaryExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunBinaryExpression();
}

// DataVarExprID
inline
AFun initAFunDataVarExprID(AFun& f)
{
  f = ATmakeAFun("DataVarExprID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarExprID()
{
  static AFun AFunDataVarExprID = initAFunDataVarExprID(AFunDataVarExprID);
  return AFunDataVarExprID;
}

inline
bool gsIsDataVarExprID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarExprID();
}

// DataVarID
inline
AFun initAFunDataVarID(AFun& f)
{
  f = ATmakeAFun("DataVarID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarID()
{
  static AFun AFunDataVarID = initAFunDataVarID(AFunDataVarID);
  return AFunDataVarID;
}

inline
bool gsIsDataVarID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarID();
}

// Expression
inline
AFun initAFunExpression(AFun& f)
{
  f = ATmakeAFun("Expression", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunExpression()
{
  static AFun AFunExpression = initAFunExpression(AFunExpression);
  return AFunExpression;
}

inline
bool gsIsExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunExpression();
}

// Nil
inline
AFun initAFunNil(AFun& f)
{
  f = ATmakeAFun("Nil", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNil()
{
  static AFun AFunNil = initAFunNil(AFunNil);
  return AFunNil;
}

inline
bool gsIsNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNil();
}

// ProcSpec
inline
AFun initAFunProcSpec(AFun& f)
{
  f = ATmakeAFun("ProcSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcSpec()
{
  static AFun AFunProcSpec = initAFunProcSpec(AFunProcSpec);
  return AFunProcSpec;
}

inline
bool gsIsProcSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcSpec();
}

// Skip
inline
AFun initAFunSkip(AFun& f)
{
  f = ATmakeAFun("Skip", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSkip()
{
  static AFun AFunSkip = initAFunSkip(AFunSkip);
  return AFunSkip;
}

inline
bool gsIsSkip(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSkip();
}

// StatementSpec
inline
AFun initAFunStatementSpec(AFun& f)
{
  f = ATmakeAFun("StatementSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunStatementSpec()
{
  static AFun AFunStatementSpec = initAFunStatementSpec(AFunStatementSpec);
  return AFunStatementSpec;
}

inline
bool gsIsStatementSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunStatementSpec();
}

// Type
inline
AFun initAFunType(AFun& f)
{
  f = ATmakeAFun("Type", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunType()
{
  static AFun AFunType = initAFunType(AFunType);
  return AFunType;
}

inline
bool gsIsType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunType();
}

// UnaryExpression
inline
AFun initAFunUnaryExpression(AFun& f)
{
  f = ATmakeAFun("UnaryExpression", 3, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunUnaryExpression()
{
  static AFun AFunUnaryExpression = initAFunUnaryExpression(AFunUnaryExpression);
  return AFunUnaryExpression;
}

inline
bool gsIsUnaryExpression(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunUnaryExpression();
}

// VarSpec
inline
AFun initAFunVarSpec(AFun& f)
{
  f = ATmakeAFun("VarSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunVarSpec()
{
  static AFun AFunVarSpec = initAFunVarSpec(AFunVarSpec);
  return AFunVarSpec;
}

inline
bool gsIsVarSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunVarSpec();
}

inline
ATermAppl gsMakeAssignment(ATermList Expr_0, ATermList Expr_1)
{
  return ATmakeAppl2(gsAFunAssignment(), (ATerm) Expr_0, (ATerm) Expr_1);
}

inline
ATermAppl gsMakeBinaryExpression(ATermAppl String_0, ATermAppl Expr_1, ATermAppl Expr_2, ATermAppl TypeID_3)
{
  return ATmakeAppl4(gsAFunBinaryExpression(), (ATerm) String_0, (ATerm) Expr_1, (ATerm) Expr_2, (ATerm) TypeID_3);
}

inline
ATermAppl gsMakeDataVarExprID(ATermAppl VarID_0, ATermAppl Exp_1)
{
  return ATmakeAppl2(gsAFunDataVarExprID(), (ATerm) VarID_0, (ATerm) Exp_1);
}

inline
ATermAppl gsMakeDataVarID(ATermList String_0, ATermAppl TypeExp_1)
{
  return ATmakeAppl2(gsAFunDataVarID(), (ATerm) String_0, (ATerm) TypeExp_1);
}

inline
ATermAppl gsMakeExpression(ATermAppl String_0, ATermAppl TypeID_1)
{
  return ATmakeAppl2(gsAFunExpression(), (ATerm) String_0, (ATerm) TypeID_1);
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(gsAFunNil());
}

inline
ATermAppl gsMakeProcSpec(ATermList DecSpec_0, ATermList StatementSpec_1)
{
  return ATmakeAppl2(gsAFunProcSpec(), (ATerm) DecSpec_0, (ATerm) StatementSpec_1);
}

inline
ATermAppl gsMakeSkip()
{
  return ATmakeAppl0(gsAFunSkip());
}

inline
ATermAppl gsMakeStatementSpec(ATermList Statement_0)
{
  return ATmakeAppl1(gsAFunStatementSpec(), (ATerm) Statement_0);
}

inline
ATermAppl gsMakeType(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunType(), (ATerm) String_0);
}

inline
ATermAppl gsMakeUnaryExpression(ATermAppl String_0, ATermAppl Expr_1, ATermAppl TypeID_2)
{
  return ATmakeAppl3(gsAFunUnaryExpression(), (ATerm) String_0, (ATerm) Expr_1, (ATerm) TypeID_2);
}

inline
ATermAppl gsMakeVarSpec(ATermList VarExpID_0)
{
  return ATmakeAppl1(gsAFunVarSpec(), (ATerm) VarExpID_0);
}
//--- end generated code

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

inline
ATermAppl gsString2ATermAppl(const char *s)
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();   
  }
}

inline
bool gsIsString(ATermAppl term)
//Ret: term is a quoted constant
{
  AFun head = ATgetAFun(term);
  return ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue));
}

inline
char *gsATermAppl2String(ATermAppl term)
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise
{
  if (gsIsString(term)) {
    return ATgetName(ATgetAFun(term));
  } else {
    return NULL;
  }
}

inline
bool gsIsNumericString(const char* s)
//Ret: true if s is of form "0 | -? [1-9][0-9]*", false otherwise
{
  if (s == NULL) return false;
  if (s[0] == '\0') return false;
  if (s[0] == '-') ++s;
  if (s[0] == '\0') return false;
  if (s[0] == '0') {
    ++s;
    if (s[0] == '\0') return true;
    else return false;
  }
  for (; s[0] != '\0'; ++s)
    if(!isdigit(s[0])) return false;
  return true;
}

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise

#ifdef __cplusplus
}
#endif

#endif // MCRL2_LIBSTRUCT_CORE_H
