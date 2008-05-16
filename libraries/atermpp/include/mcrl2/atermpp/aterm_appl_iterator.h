// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_appl_iterator.h
/// \brief Iterator for term_appl.

#ifndef MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H
#define MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp {

/// Iterator for term_appl.
///
template <typename Value>
class term_appl_iterator: public boost::iterator_facade<
        term_appl_iterator<Value>,         // Derived
        const Value,                       // Value
        boost::forward_traversal_tag,      // CategoryOrTraversal
        const Value                        // Reference
    >
{
 public:
    /// Constructor.
    ///
    term_appl_iterator()
    {}

    /// Constructor.
    ///
    term_appl_iterator(ATerm* t)
      : m_term(t)
    {}

 private:
    friend class boost::iterator_core_access;

    /// \cond INTERNAL_DOCS
    bool equal(term_appl_iterator const& other) const
    { return this->m_term == other.m_term; }

    const Value dereference() const
    {
      return Value(*m_term);
    }

    void increment()
    { m_term++; }

    void decrement()
    { m_term--; }
    /// \endcond

    ATerm* m_term;
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_APPL_ITERATOR_H
