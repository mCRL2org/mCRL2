///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/term_list_iterator.h
/// Contains a general aterm_list iterator.

#ifndef MCRL2_TERM_LIST_ITERATOR_H
#define MCRL2_TERM_LIST_ITERATOR_H

#include <list>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"

namespace mcrl2 {

using atermpp::aterm_list;
using atermpp::aterm_appl;

//---------------------------------------------------------//
//                     term_list_iterator
//---------------------------------------------------------//
template <typename Value>
class term_list_iterator: public boost::iterator_facade<
        term_list_iterator<Value>,         // Derived
        const Value,                       // Value
        boost::forward_traversal_tag,      // CategoryOrTraversal
        const Value                        // Reference
    >
{
 public:
    term_list_iterator()
      : m_list(ATempty)
    {}

    term_list_iterator(aterm_list l)
      : m_list(l.list())
    {}

 private:
    friend class boost::iterator_core_access;

    /// INTERNAL ONLY
    bool equal(term_list_iterator const& other) const
    { return this->m_list == other.m_list; }

    /// INTERNAL ONLY
    const Value dereference() const
    { return Value(aterm_appl(void2appl(term2void(ATgetFirst(m_list))))); }

    /// INTERNAL ONLY
    void increment()
    { m_list = ATgetNext(m_list); }

    ATermList m_list;
};

} // namespace mcrl

#endif // MCRL2_TERM_LIST_ITERATOR_H
