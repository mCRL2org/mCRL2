// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm.h
/// \brief The aterm class.

#ifndef MCRL2_ATERMPP_ATERM_H
#define MCRL2_ATERMPP_ATERM_H

#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <assert.h>

#include <boost/utility/enable_if.hpp>
#include "boost/type_traits/is_base_of.hpp"
#include "boost/type_traits/is_convertible.hpp"
#include "boost/static_assert.hpp"

#include "mcrl2/atermpp/detail/aterm.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

class aterm
{
  public:
    template < typename T >
    friend class term_appl; 

    template < typename T >
    friend class term_list; 
    
    friend void detail::simple_free_term(const detail::_aterm *t, const size_t arity);
   
    friend void detail::free_term(const detail::_aterm* t);

    friend void detail::initialise_aterm_administration();

    template <class Term, class Iter, class ATermConverter>
    friend const detail::_aterm *detail::make_list_backward(Iter first, Iter last, const ATermConverter &convert_to_aterm);

  protected:
    const detail::_aterm* m_term;

    static const detail::_aterm *undefined_aterm();
    static const detail::_aterm *empty_aterm_list();
 
    inline size_t decrease_reference_count() const
    {
      assert(m_term!=NULL);
      assert(m_term->reference_count()>0);
      m_term->decrease_reference_count();
      return m_term->reference_count();
    }

    template <bool CHECK>
    void increase_reference_count() const
    {
      assert(m_term!=NULL);
      if (CHECK) assert(m_term->reference_count()>0);
      m_term->increase_reference_count();
    }

    void copy_term(const aterm &t)
    {
      t.increase_reference_count<true>();
      decrease_reference_count();
      m_term=t.m_term;
    }

    // An aterm has a function symbol, which can also be an AS_EMPTY_LIST,
    // AS_INT and AS_LIST. 
    const function_symbol &function() const
    {
      return m_term->function();
    }
    
    /// \brief Constructor.
    /// \detail The function symbol must have arity 0. This function
    /// is for internal use only. Use term_appl(sym) in applications.
    /// \param sym A function symbol.
    aterm(const function_symbol &sym):m_term(detail::aterm0(sym))
    {
      increase_reference_count<false>();
    }
  
  public: // Should be protected, but this cannot yet be done due to a problem
          // in the compiling rewriter.
    aterm (const detail::_aterm *t):m_term(t)
    {
      // Note that reference_count can be 0, as this term can just be constructed,
      // and is now handed over to become a real aterm.
      assert(t!=NULL);
      increase_reference_count<false>();
    } 

  public:

    /// \brief Default constructor
    aterm():m_term(undefined_aterm())
    {
      increase_reference_count<false>();
    }

    /// \brief Copy constructor
    aterm(const aterm &t):m_term(t.m_term)
    {
      assert(t.address()!=NULL);
      increase_reference_count<true>();
    }

    /// \brief Assignment operator.
    /// \param t a term to be assigned.
    aterm &operator=(const aterm &t)
    {
      copy_term(t);
      return *this;
    }

    /// \brief Destructor.
    ~aterm ()
    {
      decrease_reference_count();
    }

    /// \brief Returns the type of this term.
    /// Result is one of AT_APPL, AT_INT,
    /// or AT_LIST.
    /// \detail Often it is more efficient to use the utility functions
    ///      type_is_appl, type_is_int or type_is_list.
    /// \return The type of the term.
    size_t type() const
    {
      return m_term->type(); 
    }

    /// \brief Returns whether this term has type AT_APPL.
    /// \detail This function is more efficient than using
    ///   type()==AT_APPL.
    /// \return True iff term is an term_appl.
    bool type_is_appl() const
    {
      return m_term->function().number()>detail::function_adm.AS_EMPTY_LIST.number(); 
    }

    /// \brief Returns whether this term has type AT_INT
    /// \detail This function is more efficient than using
    ///   type()==AT_INT.
    /// \return True iff term is an term_int.
    bool type_is_int() const
    {
      return m_term->function().number()==detail::function_adm.AS_INT.number(); 
    }

    /// \brief Returns whether this term has type AT_LIST
    /// \detail This function is more efficient than using
    ///   type()==AT_LIST.
    /// \return True iff term is an term_list.
    bool type_is_list() const
    {
      const size_t n=m_term->function().number();
      return n==detail::function_adm.AS_LIST.number()|| n==detail::function_adm.AS_EMPTY_LIST.number(); 
    }


    /// \brief Writes the term to a string.
    /// \return A string representation of the term.
    std::string to_string() const;

    /// \brief Equality function on two aterms.
    /// \detail Terms are stored in a maximally shared way. This
    ///         means that this equality operator can be calculated
    ///         in constant time.
    /// \param t A term to which the current term is compared.
    /// \return true iff t is equal to the current term.
    bool operator ==(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term==t.m_term;
    }

    /// \brief Inequality operator on two aterms.
    /// \detail See note at the == operator. This operator requires constant time.
    /// \param t A term to which the current term is compared.
    /// \return false iff t is equal to the current term.
    bool operator !=(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term!=t.m_term;
    }

    /// \brief Comparison operator for two aterms.
    /// \detail This operator requires constant time. It compares
    ///         the addresses where terms are stored. That means
    ///         that the outcome of this operator is only stable
    ///         as long as aterms are not garbage collected.
    /// \param t A term to which the current term is compared.
    /// \return True iff the current term is smaller than the argument.
    bool operator <(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term<t.m_term;
    }

    /// \brief Comparison operator for two aterms.
    /// \detail This operator requires constant time. See note at the operator <.
    /// \param t A term to which the current term is compared.
    /// \return True iff the current term is larger than the argument.
    bool operator >(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term>t.m_term;
    }

    /// \brief Comparison operator for two aterms.
    /// \detail This operator requires constant time. See note at the operator <.
    /// \param t A term to which the current term is compared.
    /// \return True iff the current term is smaller or equal than the argument.
    bool operator <=(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term<=t.m_term;
    }

    /// \brief Comparison operator for two aterms.
    /// \detail This operator requires constant time. See note at the operator <.
    /// \param t A term to which the current term is compared.
    /// \return True iff the current term is larger or equalthan the argument.
    bool operator >=(const aterm &t) const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      return m_term>=t.m_term;
    }

    /// \brief Provide the current address of this aterm.
    /// \details This address will be stable as long as this aterm
    ///          exists, i.e., has a reference count larger than 0.
    /// \return A void* pointer, representing the machine address of the current aterm.
    const detail::_aterm* address() const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      return m_term;
    }

    /// \brief Returns true if this term is not equal to the term assigned by
    ///        the default constructor, i.e. *this!=aterm().
    /// \details This operation is more efficient than comparing the current
    ///          term with an aterm().
    /// \return A boolean indicating whether this term equals the default constructor.
    bool defined() const
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      return this->function().number()!=detail::function_adm.AS_DEFAULT.number();
    }

    /// \brief Swaps this term with its argument.
    /// \details This operation is more efficient than exchanging terms by an assignment,
    ///          as swapping does not require to change the protection of terms.
    /// \param t The term with which this term is swapped.
    void swap(aterm &t) 
    {
      assert(m_term!=NULL && m_term->reference_count()>0);
      assert(t.m_term!=NULL && t.m_term->reference_count()>0);
      const detail::_aterm* tmp(t.m_term);
      t.m_term=m_term;
      m_term=tmp; 
    }
};

/// \brief A cheap cast from one aterm based type to another
/// \details When casting one aterm based type into another, generally
///          a new aterm is constructed, and the old one is destroyed.
///          This can cause undesired overhead, for instance due to
///          increasing and decreasing of reference counts. This
///          cast changes the type, without changing the aterm itself.
///          It can only be used if the input and output types inherit
///          from aterms, and contain no additional information than a
///          single aterm.
/// \param   A term of a type inheriting from an aterm.
/// \return  A term of type ATERM_TYPE_OUT.
template <class ATERM_TYPE_OUT>
const ATERM_TYPE_OUT &aterm_cast(const aterm &t) 
{
  BOOST_STATIC_ASSERT((boost::is_base_of<aterm, ATERM_TYPE_OUT>::value));
  BOOST_STATIC_ASSERT((sizeof(ATERM_TYPE_OUT)==sizeof(aterm)));
  return (ATERM_TYPE_OUT &)t;
}

inline
std::ostream& operator<<(std::ostream& out, const aterm& t)
{
  return out << t.to_string();
}
} // namespace atermpp


namespace std
{

/// \brief Swaps two terms of a type derived from an aterm.
/// \details This operation is more efficient than exchanging terms by an assignment,
///          as swapping does not require to change the protection of terms.
/// \param t1 The first term
/// \param t2 The second term

inline void swap(atermpp::aterm &t1, atermpp::aterm &t2)
{
  t1.swap(t2);
}
} // namespace std 


#include "mcrl2/atermpp/detail/aterm_implementation.h"

#endif // MCRL2_ATERMPP_ATERM_H
