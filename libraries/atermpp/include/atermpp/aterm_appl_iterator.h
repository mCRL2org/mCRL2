// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/aterm_appl_iterator.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H
#define MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"

namespace atermpp {

//---------------------------------------------------------//
//                     term_appl_iterator
//---------------------------------------------------------//
template <typename Value>
class term_appl_iterator: public boost::iterator_facade<
        term_appl_iterator<Value>,         // Derived
        const Value,                       // Value
        boost::forward_traversal_tag,      // CategoryOrTraversal
        const Value                        // Reference
    >
{
 public:
    term_appl_iterator()
    {}

    term_appl_iterator(ATerm* t)
      : m_term(t)
    {}

 private:
    friend class boost::iterator_core_access;

    /// \internal
    bool equal(term_appl_iterator const& other) const
    { return this->m_term == other.m_term; }

    /// \internal
    const Value dereference() const
    {
      return Value(*m_term);
    }

    /// \internal
    void increment()
    { m_term++; }

    /// \internal
    void decrement()
    { m_term--; }

    ATerm* m_term;
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H
