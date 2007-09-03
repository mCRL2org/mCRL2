// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/detail/constructors.h
/// \brief Add your file description here.

#ifndef LPS_DETAIL_CONSTRUCTORS
#define LPS_DETAIL_CONSTRUCTORS

#include "libstruct_core.h" // gsString2ATermAppl

namespace lps {

namespace detail {

// String
inline
ATermAppl initConstructString(ATermAppl& t)
{
  t = gsString2ATermAppl("@NoValue");
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructString()
{
  static ATermAppl t = initConstructString(t);
  return t;
}

// List
inline
ATermList constructList()
{
  return ATmakeList0();
}

//--- begin generated code
ATermAppl constructUnaryExpression();
ATermAppl constructAssignment();
ATermAppl constructNil();
ATermAppl constructSkip();
ATermAppl constructExpression();
ATermAppl constructDataVarID();
ATermAppl constructStatementSpec();
ATermAppl constructDataVarExprID();
ATermAppl constructVarSpec();
ATermAppl constructBinaryExpression();
ATermAppl constructProcSpec();
ATermAppl constructType();
ATermAppl constructSpecChi();
ATermAppl constructDecSpec();
ATermAppl constructVarExpID();
ATermAppl constructVarID();
ATermAppl constructTypeExp();
ATermAppl constructTypeID();
ATermAppl constructExpr();
ATermAppl constructStatement();

// UnaryExpression
inline
ATermAppl initConstructUnaryExpression(ATermAppl& t)
{
  t = ATmakeAppl3(gsAFunUnaryExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructUnaryExpression()
{
  static ATermAppl t = initConstructUnaryExpression(t);
  return t;
}

// Assignment
inline
ATermAppl initConstructAssignment(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunAssignment(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructAssignment()
{
  static ATermAppl t = initConstructAssignment(t);
  return t;
}

// Nil
inline
ATermAppl initConstructNil(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunNil());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructNil()
{
  static ATermAppl t = initConstructNil(t);
  return t;
}

// Skip
inline
ATermAppl initConstructSkip(ATermAppl& t)
{
  t = ATmakeAppl0(gsAFunSkip());
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructSkip()
{
  static ATermAppl t = initConstructSkip(t);
  return t;
}

// Expression
inline
ATermAppl initConstructExpression(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructExpression()
{
  static ATermAppl t = initConstructExpression(t);
  return t;
}

// DataVarID
inline
ATermAppl initConstructDataVarID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarID(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructTypeExp()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarID()
{
  static ATermAppl t = initConstructDataVarID(t);
  return t;
}

// StatementSpec
inline
ATermAppl initConstructStatementSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunStatementSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructStatementSpec()
{
  static ATermAppl t = initConstructStatementSpec(t);
  return t;
}

// DataVarExprID
inline
ATermAppl initConstructDataVarExprID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarExprID(), reinterpret_cast<ATerm>(constructVarID()), reinterpret_cast<ATerm>(constructExp()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarExprID()
{
  static ATermAppl t = initConstructDataVarExprID(t);
  return t;
}

// VarSpec
inline
ATermAppl initConstructVarSpec(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunVarSpec(), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructVarSpec()
{
  static ATermAppl t = initConstructVarSpec(t);
  return t;
}

// BinaryExpression
inline
ATermAppl initConstructBinaryExpression(ATermAppl& t)
{
  t = ATmakeAppl4(gsAFunBinaryExpression(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructExpr()), reinterpret_cast<ATerm>(constructTypeID()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructBinaryExpression()
{
  static ATermAppl t = initConstructBinaryExpression(t);
  return t;
}

// ProcSpec
inline
ATermAppl initConstructProcSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcSpec(), reinterpret_cast<ATerm>(constructList()), reinterpret_cast<ATerm>(constructList()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructProcSpec()
{
  static ATermAppl t = initConstructProcSpec(t);
  return t;
}

// Type
inline
ATermAppl initConstructType(ATermAppl& t)
{
  t = ATmakeAppl1(gsAFunType(), reinterpret_cast<ATerm>(constructString()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructType()
{
  static ATermAppl t = initConstructType(t);
  return t;
}

// SpecChi
inline
ATermAppl constructSpecChi()
{
  return constructProcSpec();
}

// DecSpec
inline
ATermAppl constructDecSpec()
{
  return constructVarSpec();
}

// VarExpID
inline
ATermAppl constructVarExpID()
{
  return constructDataVarExprID();
}

// VarID
inline
ATermAppl constructVarID()
{
  return constructDataVarID();
}

// TypeExp
inline
ATermAppl constructTypeExp()
{
  return constructTypeID();
}

// TypeID
inline
ATermAppl constructTypeID()
{
  return constructType();
}

// Expr
inline
ATermAppl constructExpr()
{
  return constructExpression();
}

// Statement
inline
ATermAppl constructStatement()
{
  return constructSkip();
}
//--- end generated code

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_CONSTRUCTORS
