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
ATermAppl constructNil();
ATermAppl constructSkip();
ATermAppl constructDataVarID();
ATermAppl constructVarSpec();
ATermAppl constructProcSpec();
ATermAppl constructType();
ATermAppl constructSpecChi();
ATermAppl constructVarID();
ATermAppl constructTypeExp();
ATermAppl constructTypeID();
ATermAppl constructStatementSpec();

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

// DataVarID
inline
ATermAppl initConstructDataVarID(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunDataVarID(), reinterpret_cast<ATerm>(constructString()), reinterpret_cast<ATerm>(constructTypeExp()));
  ATprotect(reinterpret_cast<ATerm*>(&t));
  return t;
}

inline
ATermAppl constructDataVarID()
{
  static ATermAppl t = initConstructDataVarID(t);
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

// ProcSpec
inline
ATermAppl initConstructProcSpec(ATermAppl& t)
{
  t = ATmakeAppl2(gsAFunProcSpec(), reinterpret_cast<ATerm>(constructVarSpec()), reinterpret_cast<ATerm>(constructStatementSpec()));
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

// StatementSpec
inline
ATermAppl constructStatementSpec()
{
  return constructSkip();
}
//--- end generated code

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_CONSTRUCTORS
