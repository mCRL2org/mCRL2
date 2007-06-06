// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/function_symbol.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_function_symbol.h
/// Contains the definition of the aterm_function_symbol class.

#ifndef ATERM_FUNCTION_SYMBOL_H
#define ATERM_FUNCTION_SYMBOL_H

#include <string>
#include "aterm2.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                    function_symbol
  //---------------------------------------------------------//
  class function_symbol
  {
    protected:
      AFun m_function;
  
    public:
      function_symbol(const std::string& name, int arity, bool quoted = false)
        : m_function(ATmakeAFun(const_cast<char*>(name.c_str()), arity, quoted ? ATtrue : ATfalse))
      {}
      
      function_symbol(AFun function):
        m_function(function)
      {}
  
      /// Protect the function symbol.
      /// Just as aterms which are not on the stack or in registers must be protected through
      /// a call to protect, so must function_symbols be protected by calling protect.
      ///
      void protect()
      {
        ATprotectAFun(m_function);
      }
  
      /// Release an function_symbol's protection.
      ///
      void unprotect()
      {
        ATunprotectAFun(m_function);
      }
      
      /// Return the name of the function_symbol.
      ///
      std::string name() const
      {
        return std::string(ATgetName(m_function));
      }
      
      /// Return the arity (number of arguments) of the function symbol (function_symbol).
      ///
      unsigned int arity() const
      {
        return ATgetArity(m_function);
      }
      
      /// Determine if the function symbol (function_symbol) is quoted or not.
      ///
      bool is_quoted() const
      {
        return ATisQuoted(m_function);
      }

      /// Conversion to AFun.
      ///
      operator AFun() const
      { return m_function; }
      
      friend bool operator!=(const function_symbol& x, const function_symbol& y);
  };

  /// Tests equality of function symbols f1 and f2.
  /// Function symbols f1 and f2 are considered equal if they have the same name,
  /// the same arity and the same value for the quoted attribute.
  ///
  inline
  bool operator==(const function_symbol& x, const function_symbol& y)
  { 
    // return x.name() == y.name() && x.arity() == y.arity() && x.is_quoted() == y.is_quoted();
    return AFun(x) == AFun(y);
  }
  
  /// Returns !(x==y).
  ///
  inline
  bool operator!=(const function_symbol& x, const function_symbol& y)
  { return !(x == y); }
  
} // namespace atermpp

#endif // ATERM_FUNCTION_SYMBOL_H
