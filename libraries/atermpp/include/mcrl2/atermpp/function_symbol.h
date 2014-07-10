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
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol.h"

namespace atermpp
{

class function_symbol
{

  friend size_t detail::addressf(const function_symbol& t);
  protected:
    const detail::_function_symbol* m_function_symbol;

    void free_function_symbol() const;

    template <bool CHECK>
    void increase_reference_count() const
    {
      if (CHECK) assert(m_function_symbol->reference_count>0);
      m_function_symbol->reference_count++;
    }

    void decrease_reference_count() const
    {
      assert(m_function_symbol->reference_count>0);

      if (--m_function_symbol->reference_count==0)
      {
        free_function_symbol();
      }
    }


  public:
    /// \brief default constructor
    function_symbol();

    /// \brief Constructor.
    /// \param name A string
    /// \param arity The arity of the function.
    function_symbol(const std::string& name, const size_t arity);

    /// \brief default constructor
    /// \details This function is deprecated and should not be used
    /// \param n The number of an function_symbol
    /// \deprecated
    explicit function_symbol(const size_t n)
       : m_function_symbol(&detail::function_symbol_index_table[n>>FUNCTION_SYMBOL_BLOCK_CLASS]
                                         [n & FUNCTION_SYMBOL_BLOCK_MASK])
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(n==m_function_symbol->number);
      increase_reference_count<false>();
    }

    /// \brief Copy constructor
    function_symbol(const function_symbol& f):m_function_symbol(f.m_function_symbol)
    {
      increase_reference_count<true>();
    }

    /// \brief Assignment operator.
    function_symbol& operator=(const function_symbol& f)
    {
      f.increase_reference_count<true>();
      decrease_reference_count(); // Decrease the reference count after increasing it,
                                  // as otherwise the reference count can becomes 0 for
                                  // a short moment when x=x is executed and the reference
                                  // count of x is 1. x can then prematurely be garbage collected,
                                  // depending on the garbage collection scheme..
      m_function_symbol=f.m_function_symbol;
      return *this;
    }

    /// \brief Destructor
    ~function_symbol()
    {
      decrease_reference_count();
    }

    /// \brief Return the name of the function_symbol.
    /// \return The name of the function symbol.
    const std::string& name() const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->name;
    }

    /// \brief Return the number of the function_symbol.
    /// \return The number of the function symbol.
    size_t number() const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->number;
    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->arity;
    }


    /// \brief Equality test.
    /// \details This operator compares the indices of the function symbols. This means
    ///         that this operation takes constant time.
    /// \returns True iff the function symbols are the same.
    bool operator ==(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol==f.m_function_symbol;
    }

    /// \brief Inequality test.
    /// \details This operator takes constant time.
    /// \returns True iff the function symbols are not equal.
    bool operator !=(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol!=f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower index than the argument.
    bool operator <(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol<f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a higher index than the argument.
    bool operator >(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol>f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower or equal index than the argument.
    bool operator <=(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol<=f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a larger or equal index than the argument.
    bool operator >=(const function_symbol& f) const
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol>=f.m_function_symbol;
    }

    /// \brief Swap this function with its argument.
    /// \details More efficient than assigning twice.
    /// \param f The function symbol with which the swap takes place.
    void swap(function_symbol& f)
    {
      using std::swap;
      swap(f.m_function_symbol,m_function_symbol);
    }
};

/// \brief Sends the name of a function symbol to an ostream.
inline
std::ostream& operator<<(std::ostream& out, const function_symbol& t)
{
  return out << t.name();
}

} // namespace atermpp


namespace std
{

/// \brief Swaps two function symbols.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term

template <>
inline void swap(atermpp::function_symbol& t1, atermpp::function_symbol& t2)
{
  t1.swap(t2);
}
} // namespace std

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
