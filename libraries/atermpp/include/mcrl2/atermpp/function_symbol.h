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

  friend size_t detail::address(const function_symbol &t);
  protected:
#ifdef FUNCTION_SYMBOL_AS_POINTER
    const detail::_function_symbol* m_function_symbol;
#else
    size_t m_number;
#endif

    void free_function_symbol() const;

    template <bool CHECK>
    void increase_reference_count() const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      if (CHECK) assert(m_function_symbol->reference_count>0);
      m_function_symbol->reference_count++;
#else
      assert(m_number<detail::function_lookup_table_size);
      if (CHECK) assert(detail::function_lookup_table[m_number].reference_count>0);
      detail::function_lookup_table[m_number].reference_count++;
#endif
    }

    void decrease_reference_count() const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(m_function_symbol->reference_count>0);

      if (--m_function_symbol->reference_count==0)
      {
        free_function_symbol();
      }
#else
      assert(m_number<detail::function_lookup_table_size);
      assert(detail::function_lookup_table[m_number].reference_count>0);

      if (--detail::function_lookup_table[m_number].reference_count==0)
      {
        free_function_symbol();
      }
#endif

    }


  public:
    /// \brief default constructor
    function_symbol();

    /// \brief Constructor.
    /// \param name A string
    /// \param arity The arity of the function.
    function_symbol(const std::string &name, const size_t arity);

    /// \brief default constructor
    /// \details This function is deprecated and should not be used
    /// \param n The number of an function_symbol
    /// \deprecated
#ifdef FUNCTION_SYMBOL_AS_POINTER
    explicit function_symbol(const size_t n)
       : m_function_symbol(&detail::function_symbol_index_table[n>>FUNCTION_SYMBOL_BLOCK_CLASS]
                                         [n & FUNCTION_SYMBOL_BLOCK_MASK])
    {
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(n==m_function_symbol->number);
      increase_reference_count<false>();
    }
#else
    explicit function_symbol(const size_t n):m_number(n)
    {
      assert(detail::is_valid_function_symbol(m_number));
      increase_reference_count<false>();
    }
#endif

    /// \brief Copy constructor
#ifdef FUNCTION_SYMBOL_AS_POINTER
    function_symbol(const function_symbol &f):m_function_symbol(f.m_function_symbol)
    {
      increase_reference_count<true>();
    }
#else
    function_symbol(const function_symbol &f):m_number(f.m_number)
    {
      increase_reference_count<true>();
    }
#endif


    /// \brief Assignment operator.
    function_symbol &operator=(const function_symbol &f)
    {
      f.increase_reference_count<true>();
      decrease_reference_count(); // Decrease the reference count after increasing it,
                                  // as otherwise the reference count can becomes 0 for
                                  // a short moment when x=x is executed and the reference
                                  // count of x is 1. x can then prematurely be garbage collected,
                                  // depending on the garbage collection scheme..
#ifdef FUNCTION_SYMBOL_AS_POINTER
      m_function_symbol=f.m_function_symbol;
#else
      m_number=f.m_number;
#endif
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
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->name;
#else
      assert(detail::is_valid_function_symbol(m_number));
      return detail::function_lookup_table[m_number].name;
#endif
    }

    /// \brief Return the number of the function_symbol.
    /// \return The number of the function symbol.
    size_t number() const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->number;
#else
      assert(detail::is_valid_function_symbol(m_number));
      return m_number;
#endif

    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      return m_function_symbol->arity;
#else
      assert(detail::is_valid_function_symbol(m_number));
      return detail::function_lookup_table[m_number].arity;
#endif

    }


    /// \brief Equality test.
    /// \details This operator compares the indices of the function symbols. This means
    ///         that this operation takes constant time.
    /// \returns True iff the function symbols are the same.
    bool operator ==(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol==f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number==f.m_number;
#endif

    }

    /// \brief Inequality test.
    /// \details This operator takes constant time.
    /// \returns True iff the function symbols are not equal.
    bool operator !=(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol!=f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number!=f.m_number;
#endif

    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower index than the argument.
    bool operator <(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol<f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number<f.m_number;
#endif

    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a higher index than the argument.
    bool operator >(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol>f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number>f.m_number;
#endif

    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower or equal index than the argument.
    bool operator <=(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol<=f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number<=f.m_number;
#endif

    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a larger or equal index than the argument.
    bool operator >=(const function_symbol &f) const
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      assert(detail::is_valid_function_symbol(m_function_symbol));
      assert(detail::is_valid_function_symbol(f.m_function_symbol));
      return m_function_symbol>=f.m_function_symbol;
#else
      assert(detail::is_valid_function_symbol(m_number));
      assert(detail::is_valid_function_symbol(f.m_number));
      return m_number>=f.m_number;
#endif

    }

    /// \brief Swap this function with its argument.
    /// \details More efficient than assigning twice.
    /// \param f The function symbol with which the swap takes place.
    void swap(function_symbol &f)
    {
#ifdef FUNCTION_SYMBOL_AS_POINTER
      std::swap(f.m_function_symbol,m_function_symbol);
#else
      std::swap(f.m_number,m_number);
#endif
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
