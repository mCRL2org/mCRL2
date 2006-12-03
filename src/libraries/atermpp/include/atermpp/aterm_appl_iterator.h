// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_appl_iterator.h
// date          : 20-11-2006
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file atermpp/aterm_appl_iterator.h
/// Contains a general term_list iterator.

#ifndef ATERMPP_TERM_APPL_ITERATOR_H
#define ATERMPP_TERM_APPL_ITERATOR_H

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

    /// INTERNAL ONLY
    bool equal(term_appl_iterator const& other) const
    { return this->m_term == other.m_term; }

    /// INTERNAL ONLY
    const Value dereference() const
    {
      return Value(*m_term);
    }

    /// INTERNAL ONLY
    void increment()
    { m_term = m_term + 1; }

    ATerm* m_term;
};

} // namespace atermpp

#endif // ATERMPP_TERM_APPL_ITERATOR_H
