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

namespace atermpp {

  /// \cond INTERNAL_DOCS
  // needed for conversion of the return type of ATgetFirst
  template <class T>
  struct term_list_iterator_traits
  {
    typedef ATerm value_type;
  };
  /// \endcond

/// \brief Iterator for term_list.
template <typename Value>
class term_list_iterator: public boost::iterator_facade<
        term_list_iterator<Value>,         // Derived
        const Value,                       // Value
        boost::forward_traversal_tag,      // CategoryOrTraversal
        const Value                        // Reference
    >
{
 public:
    /// \brief Constructor.
    term_list_iterator()
      : m_list(ATempty)
    {}

    /// \brief Constructor.
    /// \param l A sequence of terms
    term_list_iterator(ATermList l)
      : m_list(l)
    {}

 private:
    friend class boost::iterator_core_access;

    /// \brief Equality operator
    /// \param other An iterator
    /// \return True if the iterators are equal
    bool equal(term_list_iterator const& other) const
    { return this->m_list == other.m_list; }

    /// \brief Dereference operator
    /// \return The value that the iterator references
    const Value dereference() const
    {
      return Value(typename term_list_iterator_traits<Value>::value_type(ATgetFirst(m_list)));
    }

    /// \brief Increments the iterator
    void increment()
    { m_list = ATgetNext(m_list); }

    ATermList m_list;
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_LIST_ITERATOR_H
