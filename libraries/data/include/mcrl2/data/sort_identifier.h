// Author(s): Wieger Wesselink
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

/// The namespace of the mCRL2 tool set (will be renamed to mcrl2).
namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;

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
      : sort_expression(detail::constructSortId())
    {}

    /// Constructor.
    ///
    sort_identifier(ATermAppl t)
      : sort_expression(t)
    {
      assert(detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(aterm_appl t)
      : sort_expression(t)
    {
      assert(detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(identifier_string name)
      : sort_expression(gsMakeSortId(name))
    {
      assert(detail::check_rule_SortId(m_term));
    }

    /// Constructor.
    ///
    sort_identifier(std::string s)
      : sort_expression(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}

    /// Returns the name of the sort identifier.
    ///
    identifier_string name() const
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
  return gsIsSortId(t);
}

} // namespace lps

/// \internal
namespace atermpp
{
using lps::sort_identifier;

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

#endif // MCRL2_DATA_SORT_ID_H
