// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_identifier.h
/// \brief Class sort_identifier.

#ifndef MCRL2_DATA_SORT_ID_H
#define MCRL2_DATA_SORT_ID_H

#include "mcrl2/data/sort_expression.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;
using namespace core::detail;
using core::identifier_string;

class sort_identifier;

/// \brief singly linked list of sorts
///
typedef term_list<sort_identifier> sort_identifier_list;

/// \brief sort id.
///
//<SortId>       ::= SortId(<String>)
class sort_identifier: public sort_expression
{
  public:
    /// Constructor.
    ///
    sort_identifier()
      : sort_expression(core::detail::constructSortId())
    {}

    /// Constructor.
    ///
    sort_identifier(ATermAppl t)
      : sort_expression(t)
    {
      assert(core::detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(aterm_appl t)
      : sort_expression(t)
    {
      assert(core::detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(identifier_string name)
      : sort_expression(core::detail::gsMakeSortId(name))
    {
      assert(core::detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(std::string s)
      : sort_expression(core::detail::gsMakeSortId(core::detail::gsString2ATermAppl(s.c_str())))
    {}

    /// Returns the name of the sort identifier.
    ///
    core::identifier_string name() const
    {
      return atermpp::arg1(*this);
    }
    
    /// \overload
    ///
    bool is_arrow() const
    {
      return false;
    }
};

/// \brief Returns true if the term t is a sort identifier.
inline
bool is_sort_identifier(aterm_appl t)
{
  return core::detail::gsIsSortId(t);
}

} // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{
using mcrl2::data::sort_identifier;

template<>
struct aterm_traits<sort_identifier>
{
  typedef ATermAppl aterm_type;
  static void protect(sort_identifier t)   { t.protect(); }
  static void unprotect(sort_identifier t) { t.unprotect(); }
  static void mark(sort_identifier t)      { t.mark(); }
  static ATerm term(sort_identifier t)     { return t.term(); }
  static ATerm* ptr(sort_identifier& t)    { return &t.term(); }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_DATA_SORT_ID_H
