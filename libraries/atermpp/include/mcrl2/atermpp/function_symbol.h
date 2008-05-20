// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/function_symbol.h
/// \brief Function symbol class.

#ifndef MCRL2_ATERMPP_FUNCTION_SYMBOL_H
#define MCRL2_ATERMPP_FUNCTION_SYMBOL_H

#include <string>
#include "aterm2.h"

namespace atermpp
{
  /// Function symbol.
  ///
  class function_symbol
  {
    protected:
      AFun m_function;
  
    public:
      /// Constructor.
      ///
      function_symbol(const std::string& name, int arity, bool quoted = false)
        : m_function(ATmakeAFun(const_cast<char*>(name.c_str()), arity, quoted ? ATtrue : ATfalse))
      {}
      
      /// Constructor.
      ///
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

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
