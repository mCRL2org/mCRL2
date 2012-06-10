// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_list_iterator.h
/// \brief Iterator for term_list.

#ifndef MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H
#define MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{
  template <class Term>
  class _aterm_list;
}
/// \endcond

/// \brief Iterator for term_list.
template <typename Term>
class term_list_iterator: public boost::iterator_facade<
  term_list_iterator<Term>,         // Derived
  const Term,                       // Value
  boost::forward_traversal_tag,      // CategoryOrTraversal
  const Term                        // Reference
  >
{
  public:
    typedef typename boost::iterator_facade<term_list_iterator<Term>,
            const Term,
            boost::forward_traversal_tag,
            const Term>::difference_type iterator_type;

    /// \brief Constructor.
    term_list_iterator()
      : m_list(NULL)
    {}

    /// \brief Constructor.
    /// \param l A sequence of terms
    term_list_iterator(const aterm &l)
      : m_list(reinterpret_cast<detail::_aterm_list<Term>*>(&*l))
    { 
      assert(l.type()==AT_LIST);
    } 

  private:
    friend class boost::iterator_core_access;

    /// \brief Equality operator
    /// \param other An iterator
    /// \return True if the iterators are equal
    bool equal(term_list_iterator const& other) const
    {
      return this->m_list == other.m_list;
    }

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const Term &dereference() const
    {
      assert(m_list->function()==AS_LIST());
      return m_list->head;
    }

    /// \brief Increments the iterator
    void increment()
    {
      m_list = &*m_list->tail;
    }

    detail::_aterm_list<Term>* m_list;
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H
