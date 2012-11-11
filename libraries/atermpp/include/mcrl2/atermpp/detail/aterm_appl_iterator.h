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

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/detail/aterm_appl.h"
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \brief Iterator for term_appl.
template <typename Term>
class term_appl_iterator: public boost::iterator_facade<
  term_appl_iterator<Term>,         // Derived
  const Term,                       // Value
  boost::random_access_traversal_tag,     // CategoryOrTraversal
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

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_ITERATOR_H
