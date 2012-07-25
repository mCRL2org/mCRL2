// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl_iterator.h
/// \brief Iterator for term_appl.

#ifndef MCRL2_ATERM_ATERM_APPL_ITERATOR_H
#define MCRL2_ATERM_ATERM_APPL_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

template <class Term>
class _aterm_appl:public _aterm
{
  public:
    Term        arg[1000];   /* This value 1000 is completely arbitrary, and should not be used
                                (therefore it is excessive). Using mallocs an array of the
                                appropriate length is declared, where it is possible that
                                the array has size 0, i.e. is absent. If the value is small
                                (it was 1), the clang compiler provides warnings. */
};

} // namespace detail
/// \endcond

/// \brief Iterator for term_appl.
template <typename Term>
class term_appl_iterator: public boost::iterator_facade<
  term_appl_iterator<Term>,         // Derived
  const Term,                       // Value
  boost::forward_traversal_tag,     // CategoryOrTraversal
  const Term&                       // Reference
  >
{
  public:
    /* /// \brief Constructor.
    term_appl_iterator()
    {}
    */

    /// \brief Constructor.
    /// \param t A term
    term_appl_iterator(Term* t)
      : m_term(t)
    {}

  private:
    friend class boost::iterator_core_access;

    /// \brief Equality check
    /// \param other An iterator
    /// \return True if the iterators are equal
    bool equal(term_appl_iterator const& other) const
    {
      return this->m_term == other.m_term;
    }

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const Term &dereference() const
    {
      return *m_term;
    }

    /// \brief Increments the iterator
    void increment()
    {
      m_term++;
    }

    /// \brief Decrements the iterator
    void decrement()
    {
      m_term--;
    }

    Term *m_term;
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H
