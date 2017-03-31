// Author(s): Wieger Wesselink, Jan Frsio Groote
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

#include <cassert>
#include <string>
#include <iostream>
#include <unordered_map>
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/detail/function_symbol.h"

namespace atermpp
{
class aterm;
void detail::initialise_aterm_administration();
void detail::initialise_function_map_administration();

class function_symbol
{

  friend size_t detail::addressf(const function_symbol& t);
  friend class function_symbol_generator;
  friend size_t detail::get_sufficiently_large_postfix_index(const std::string& prefix_);

  protected:
    
    typedef std::pair<detail::function_symbol_store_class::iterator, bool> function_symbol_iterator_bool_pair;
    
    detail::_function_symbol* m_function_symbol;
    static bool m_function_symbol_store_is_defined; // This variable indicates that the function_symbol_store
                                                    // is defined. It is maintained in the function_symbol_store_class.

    /// This is a trick to do initialise_aterm_administration before function symbols are accessed.
    /* static detail::function_symbol_store_class function_symbol_store_helper()
    {
      detail::function_symbol_store_class f_store(m_function_symbol_store_is_defined);
      detail::initialise_aterm_administration();
      detail::initialise_function_map_administration();
      return f_store;
    } */

    static detail::function_symbol_store_class& function_symbol_store()
    {
      static detail::function_symbol_store_class f_store(m_function_symbol_store_is_defined);
      return f_store;
    }

    static const function_symbol& AS_DEFAULT()
    {
      static function_symbol f("<undefined_term>",0);
      return f;
    }

    // Constructor for internal use only
    function_symbol(const std::string& name_, const size_t arity_, const bool check_for_registered_functions);

    // Constructor for internal use only.
    function_symbol(detail::_function_symbol* f)
     : m_function_symbol(f)
    {
      increase_reference_count<true>();
    }

    void free_function_symbol()
    {
      assert(is_valid());
      function_symbol_store().erase(m_function_symbol->first);
    }

    template <bool CHECK>
    void increase_reference_count() 
    {
      assert(m_function_symbol_store_is_defined);
      if (CHECK) assert(m_function_symbol->second.reference_count()>0);
      m_function_symbol->second.reference_count()++;
    }

    void decrease_reference_count() 
    {
      assert(m_function_symbol_store_is_defined);
      assert(m_function_symbol->second.reference_count()>0);

      if (--m_function_symbol->second.reference_count()==0)
      {
        free_function_symbol();
      }
    }

    bool is_valid() const
    {
      assert(m_function_symbol_store_is_defined);
      /* This function must exist in the store */
      assert(function_symbol_store().count(m_function_symbol->first)>0);
      /* The reference count must be larger than 1, which ought to be an invariant
         for all functions_symbols in function_symbol_store. */
      assert(m_function_symbol->second.reference_count()>0);
      return true;
    }

    /// A special function symbol constructor for use in the function symbol
    /// generator. This constructor assumes that the name and arity combination
    /// does not exist yet, and that the prefix and number combination is neatly
    /// recoreded in the appropriate number generator as being used. Furthermore,
    /// it takes a pointer to a char* to represent its string.
    function_symbol(const char* name_begin, const char* name_end, const size_t arity_); 

  public:
    /// \brief default constructor
    function_symbol() 
      : m_function_symbol(AS_DEFAULT().m_function_symbol)
    {
      increase_reference_count<true>();
    }

    /// \brief Constructor.
    /// \param name A string.
    /// \param arity_ The arity of the function.
    function_symbol(const std::string& name, const size_t arity_)
     : function_symbol(name, arity_, true)
    {
// std::cerr << "CONSTRUCT " << m_function_symbol << "    " << name << ": " << arity_ << "\n";
      increase_reference_count<false>();
    }

    /// \brief Copy constructor
    function_symbol(const function_symbol& f)
      : m_function_symbol(f.m_function_symbol)
    {
      increase_reference_count<true>();
    }

    /// \brief Assignment operator.
    function_symbol& operator=(const function_symbol& f)
    {
      const_cast<function_symbol&>(f).increase_reference_count<true>();
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
      // Some compilers destruct function symbols after 
      // destructing the function symbol store when terminating
      // the program. The test below avoids decreasing reference
      // counts in freed memory. 
      if (m_function_symbol_store_is_defined)
      {
// std::cerr << "DESTRUCT " << m_function_symbol << "\n";
// std::cerr << name() << ": " << arity() << "\n";
        decrease_reference_count();
      }
    }

    /// \brief Return the name of the function_symbol.
    /// \return The name of the function symbol.
    const std::string& name() const
    {
      assert(is_valid());
      return m_function_symbol->first.name();
    }

    /// \brief Return the arity (number of arguments) of the function symbol (function_symbol).
    /// \return The arity of the function symbol.
    size_t arity() const
    {
      assert(is_valid());
      return m_function_symbol->first.arity();
    }


    /// \brief Equality test.
    /// \details This operator compares the indices of the function symbols. This means
    ///         that this operation takes constant time.
    /// \returns True iff the function symbols are the same.
    bool operator ==(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
      return m_function_symbol==f.m_function_symbol;
    }

    /// \brief Inequality test.
    /// \details This operator takes constant time.
    /// \returns True iff the function symbols are not equal.
    bool operator !=(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
      return m_function_symbol!=f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower index than the argument.
    bool operator <(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
      return m_function_symbol<f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a higher index than the argument.
    bool operator >(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
      return m_function_symbol>f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a lower or equal index than the argument.
    bool operator <=(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
      return m_function_symbol<=f.m_function_symbol;
    }

    /// \brief Comparison operation.
    /// \details This operator takes constant time.
    /// \returns True iff this function has a larger or equal index than the argument.
    bool operator >=(const function_symbol& f) const
    {
      assert(is_valid());
      assert(f.is_valid());
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
std::ostream& operator<<(std::ostream& out, const function_symbol& f)
{
  return out << f.name();
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
inline void swap(atermpp::function_symbol& f1, atermpp::function_symbol& f2)
{
  f1.swap(f2);
}
} // namespace std

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_H
