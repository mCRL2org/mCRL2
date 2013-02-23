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
  protected:
    size_t m_number;

    void free_function_symbol() const;

    template <bool CHECK>
    void increase_reference_count() const
    {
      assert(m_number<detail::function_lookup_table_size);
      if (CHECK) assert(detail::function_lookup_table[m_number].reference_count>0);
      detail::function_lookup_table[m_number].reference_count++;
    }

    void decrease_reference_count() const
    {
      assert(m_number<detail::function_lookup_table_size);
      assert(detail::function_lookup_table[m_number].reference_count>0);

      if (--detail::function_lookup_table[m_number].reference_count==0)
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
    /// \param quoted True if the function symbol is a quoted string.
    function_symbol(const std::string &name, const size_t arity);
    
    /// \brief default constructor
    /// \details This function is deprecated and should not be used
    /// \param n The number of an function_symbol
    /// \deprecated
    explicit function_symbol(const size_t n):m_number(n)
    {
      assert(detail::is_valid_function_symbol(m_number));
      increase_reference_count<false>();
    }

    /// \brief Copy constructor
    function_symbol(const function_symbol &f):m_number(f.m_number)
    {
      increase_reference_count<true>();
    }

    /// \brief Assignment operator.
    function_symbol &operator=(const function_symbol &f)
    {
      f.increase_reference_count<true>();
      decrease_reference_count(); // Decrease the reference count after increasing it, 
                                  // as otherwise the reference count can becomes 0 for
                                  // a short moment when x=x is executed and the reference
                                  // count of x is 1. x can then prematurely be garbage collected,
                                  // depending on the garbage collection scheme.. 
      m_number=f.m_number;
      return *this;
    }

    /// \brief Destructor
    ~function_symbol()
    {
      decrease_reference_count();
    } 

    /// \brief Return the name of the function_symbol.
    /// \return The name of the function symbol.
    const std::string &name() const
    {
      assert(detail::is_valid_function_symbol(m_number));
      return detail::function_lookup_table[m_number].name;
    }

    /// \brief Return the number of the function_symbol.
    /// \return The number of the function symbol.
    size_t number() const
    {
      assert(detail::is_valid_function_symbol(m_number));
      return m_number;
    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const
    {
      assert(detail::is_valid_function_symbol(m_number));
      return detail::function_lookup_table[m_number].arity;
    }


    /// \brief Equality test.
    /// \detail This operator compares the indices of the function symbols. This means
    ///         that this operation takes constant time.
    /// \returns True iff the function symbols are the same.
    bool operator ==(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number==f.m_number;
    }

    /// \brief Inequality test.
    /// \detail This operator takes constant time.
    /// \returns True iff the function symbols are not equal.
    bool operator !=(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number!=f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a lower index than the argument.
    bool operator <(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number<f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a higher index than the argument.
    bool operator >(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number>f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a lower or equal index than the argument.
    bool operator <=(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number<=f.m_number;
    }

    /// \brief Comparison operation.
    /// \detail This operator takes constant time.
    /// \returns True iff this function has a larger or equal index than the argument.
    bool operator >=(const function_symbol &f) const
    {
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number>=f.m_number;
    }

    /// \brief Swap this function with its argument.
    /// \detail More efficient than assigning twice.
    /// \parameter f The function symbol with which the swap takes place.
    void swap(function_symbol &f)
    {
      std::swap(f.m_number,m_number);
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
inline void swap(atermpp::function_symbol &t1, atermpp::function_symbol &t2)
{
  t1.swap(t2);
}
} // namespace std

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
