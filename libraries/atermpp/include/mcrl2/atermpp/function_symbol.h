// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
  /// \brief Function symbol.
  class function_symbol
  {
    protected:
      /// The wrapped AFun value.
      AFun m_function;
  
    public:
      /// \brief Constructor.
      /// \param name A string
      /// \param arity The arity of the function.
      /// \param quoted True if the function symbol is a quoted string.
      function_symbol(const std::string& name, int arity, bool quoted = false)
        : m_function(ATmakeAFun(const_cast<char*>(name.c_str()), arity, quoted ? ATtrue : ATfalse))
      {}
      
      /// \brief Constructor.
      /// \param function The wrapped AFun value.
      function_symbol(AFun function):
        m_function(function)
      {}
  
      /// \brief Protect the function symbol.
      /// Just as aterms which are not on the stack or in registers must be protected through
      /// a call to protect, so must function_symbols be protected by calling protect.
      void protect()
      {
        ATprotectAFun(m_function);
      }
  
      /// \brief Release a function symbol's protection.
      void unprotect()
      {
        ATunprotectAFun(m_function);
      }
      
      /// \brief Return the name of the function_symbol.
      /// \return The name of the function symbol.
      std::string name() const
      {
        return std::string(ATgetName(m_function));
      }
      
      /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
      /// \return The arity of the function symbol.
      unsigned int arity() const
      {
        return ATgetArity(m_function);
      }
      
      /// \brief Determine if the function symbol (function_symbol) is quoted or not.
      /// \return True if the function symbol is quoted.
      bool is_quoted() const
      {
        return ATisQuoted(m_function);
      }

      /// \brief Conversion operator
      /// \return The wrapped AFun value
      operator AFun() const
      { return m_function; }
      
      friend bool operator!=(const function_symbol& x, const function_symbol& y);
  };

  /// \brief Equality operator.
  /// Function symbols x and y are considered equal if they have the same name,
  /// the same arity and the same value for the quoted attribute.
  /// \param x A function symbol.
  /// \param y A function symbol.
  /// \return True if the function symbols are equal.
  inline
  bool operator==(const function_symbol& x, const function_symbol& y)
  { 
    // return x.name() == y.name() && x.arity() == y.arity() && x.is_quoted() == y.is_quoted();
    return AFun(x) == AFun(y);
  }
  
  /// \brief Inequality operator.
  /// Function symbols x and y are considered equal if they have the same name,
  /// the same arity and the same value for the quoted attribute.
  /// \param x A function symbol.
  /// \param y A function symbol.
  /// \return True if the function symbols are not equal.
  inline
  bool operator!=(const function_symbol& x, const function_symbol& y)
  { return !(x == y); }
  
} // namespace atermpp

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
