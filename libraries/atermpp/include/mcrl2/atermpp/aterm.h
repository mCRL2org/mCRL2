// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the ATerm library by Paul Klint and others.
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
#include <iostream>

#include "mcrl2/atermpp/detail/aterm.h"

/// \brief The main namespace for the aterm++ library.
namespace atermpp
{

class aterm
{
  public:
    /* template < typename T >
    friend class term_appl;

    template < typename T >
    friend class term_list; */

  protected:
    detail::_aterm *m_term;

    void decrease_reference_count()
    {
      if (m_term!=NULL)
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"decrease reference count %ld  %p\n",m_term->reference_count,m_term);
#endif
        assert(m_term->reference_count>0);
        if (0== --m_term->reference_count)
        {
          detail::free_term(m_term);
          return;
        }
      }
    }

    template <bool CHECK>
    static void increase_reference_count(detail::_aterm* t)
    {
      if (t!=NULL)
      {
#ifdef PRINT_GC_INFO
fprintf(stderr,"increase reference count %ld  %p\n",t->reference_count,t);
#endif
        if (CHECK) assert(t->reference_count>0);
        t->reference_count++;
      }

    }

    void copy_term(detail::_aterm* t)
    {
      increase_reference_count<true>(t);
      decrease_reference_count();
      m_term=t;
    }

    /// \brief Constructor.
    /// \detail The function symbol must have arity 0. This function
    /// is for internal use only. Use term_appl(sym) in applications.
    /// \param sym A function symbol.
    aterm(const function_symbol &sym);

  public:  // Functions below should become protected.
    detail::_aterm & operator *() const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      return *m_term;
    }

    detail::_aterm * operator ->() const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      return m_term;
    }

  public:

    aterm ():m_term(NULL)
    {}

    aterm (const aterm &t):m_term(t.m_term)
    {
      increase_reference_count<true>(m_term);
    }

    aterm (detail::_aterm *t):m_term(t)
    {
      // Note that reference_count can be 0, as this term can just be constructed,
      // and is now handed over to become a real aterm.
      increase_reference_count<false>(m_term);
    }

    aterm &operator=(const aterm &t)
    {
      copy_term(t.m_term);
      return *this;
    }

    ~aterm ()
    {
      decrease_reference_count();
    }

    /// \brief Returns the function symbol belonging to a term.
    /// \return The function symbol of this term.
    const function_symbol &function() const
    {
      return m_term->m_function_symbol;
    }

    /// \brief Returns the type of this term.
    /// Result is one of AT_APPL, AT_INT,
    /// or AT_LIST.
    /// \detail Often it is more efficient to use function_symbol(),
    /// and check whether the function symbol matches AS_INT for an 
    /// AT_INT, AS_LIST or AS_EMPTY_LIST for AT_LIST, or something else
    /// for AT_APPL.
    /// \return The type of the term.
    size_t type() const
    {
      return m_term->type(); 
    }

    /// \brief Writes the term to a string.
    /// \return A string representation of the term.
    std::string to_string() const;

    bool operator ==(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term==t.m_term;
    }

    bool operator !=(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term!=t.m_term;
    }

    bool operator <(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term<t.m_term;
    }

    bool operator >(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term>t.m_term;
    }

    bool operator <=(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term<=t.m_term;
    }

    bool operator >=(const aterm &t) const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      assert(t.m_term==NULL || t.m_term->reference_count>0);
      return m_term>=t.m_term;
    }

    /// \brief Test on whether an the ATerm is not equal to NULL.
    bool is_defined() const
    {
      assert(m_term==NULL || m_term->reference_count>0);
      return m_term!=NULL && m_term->reference_count>0;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const aterm& t)
{
  return out << t.to_string();
}
	
} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_H
