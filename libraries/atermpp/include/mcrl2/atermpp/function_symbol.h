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

#include <assert.h>
#include <string>
#include <cstdio>
#include <vector>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol.h"

namespace atermpp
{

class function_symbol
{
  protected:
    size_t m_number;

  public:
    /// \brief default constructor
    function_symbol():m_number(size_t(-1))
    {
    }

    /// \brief Constructor.
    /// \param name A string
    /// \param arity The arity of the function.
    /// \param quoted True if the function symbol is a quoted string.
    function_symbol(const std::string &name, const size_t arity);
    
    /// \brief default constructor
    /// \param n The number of an function_symbol
    function_symbol(const size_t n):m_number(n)
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      detail::increase_reference_count<false>(m_number);
    }

    /// \brief Copy constructor
    function_symbol(const function_symbol &f):m_number(f.m_number)
    {
      detail::increase_reference_count<true>(m_number);
    }

    /// \brief Assignment operator.
    function_symbol &operator=(const function_symbol &f)
    {
      detail::increase_reference_count<true>(f.m_number);
      detail::decrease_reference_count(m_number); // Decrease after increasing the number, as otherwise this goes wrong when 
                                          // carrying out x=x;
      m_number=f.m_number;
      return *this;
    }

    /// \brief Destructor
    ~function_symbol()
    {
      detail::decrease_reference_count(m_number);
    }

    /// \brief Return the name of the function_symbol.
    /// \return The name of the function symbol.
    const std::string &name() const;
    
    /// \brief Return the number of the function_symbol.
    /// \return The number of the function symbol.
    size_t number() const
    {
      // First check below is a trick, function symbols with small numbers may
      // be referred to, while the data structures containing the data about these
      // functions may already have destroyed.
      assert(m_number<=3 || m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      return m_number;
    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const;

    /// \brief Equality test.
    /// \detail This operator compares the indices of the function symbols. This means
    ///         that this operation takes constant time.
    /// \returns True iff the function symbols are the same.
    bool operator ==(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number==f.m_number;
    }

    /// \brief Inequality test.
    /// \detail This operator takes constant time.
    /// \returns True iff the function symbols are not equal.
    bool operator !=(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number!=f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a lower index than the argument.
    bool operator <(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number<f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a higher index than the argument.
    bool operator >(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number>f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a lower or equal index than the argument.
    bool operator <=(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number<=f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a larger or equal index than the argument.
    bool operator >=(const function_symbol &f) const
    {
      assert(m_number==size_t(-1) || detail::AT_isValidAFun(m_number));
      assert(f.m_number==size_t(-1) || detail::AT_isValidAFun(f.m_number));
      return m_number>=f.m_number;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const function_symbol& t)
{
  if (t==function_symbol())
  {
    return out << std::string("UNDEFINED");
  }
  return out << t.name();
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
