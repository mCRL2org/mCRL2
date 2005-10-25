// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/aterm_list_iterator.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file atermpp/term_list_iterator.h
/// Contains a general term_list iterator.

#ifndef ATERMPP_TERM_LIST_ITERATOR_H
#define ATERMPP_TERM_LIST_ITERATOR_H

#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"

namespace atermpp {

//---------------------------------------------------------//
//                     term_list_iterator
//---------------------------------------------------------//
template <typename Value>
class term_list_iterator: public boost::iterator_facade<
        term_list_iterator<Value>,        // Derived
        const Value,                       // Value
        boost::forward_traversal_tag,      // CategoryOrTraversal
        const Value                        // Reference
    >
{
 public:
    term_list_iterator()
      : m_list(ATempty)
    {}

    term_list_iterator(ATermList l)
      : m_list(l)
    {}

//    template <typename Term>
//    term_list_iterator(term_list<Term> l)
//      : m_list(l.to_ATermList())
//    {}

 private:
    friend class boost::iterator_core_access;

    /// INTERNAL ONLY
    bool equal(term_list_iterator const& other) const
    { return this->m_list == other.m_list; }

    /// INTERNAL ONLY
    const Value dereference() const
    {
      return Value(ATgetFirst(m_list));
    }

    /// INTERNAL ONLY
    void increment()
    { m_list = ATgetNext(m_list); }

    ATermList m_list;
};

} // namespace atermpp

#endif // ATERMPP_TERM_LIST_ITERATOR_H
