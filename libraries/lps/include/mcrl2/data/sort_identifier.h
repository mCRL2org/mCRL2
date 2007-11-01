// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_identifier.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_SORT_ID_H
#define MCRL2_DATA_SORT_ID_H

#include "mcrl2/data/sort_expression.h"

/// The namespace of the mCRL2 tool set (will be renamed to mcrl2).
namespace lps {

using atermpp::aterm;
using atermpp::aterm_appl;
using atermpp::term_list;

class sort_identifier;

///////////////////////////////////////////////////////////////////////////////
// sort_id_list
/// \brief singly linked list of sorts
///
typedef term_list<sort_identifier> sort_identifier_list;

///////////////////////////////////////////////////////////////////////////////
// sort
/// \brief sort id.
///
//<SortId>       ::= SortId(<String>)
class sort_identifier: public sort_expression
{
  public:
    sort_identifier()
      : aterm_appl(detail::constructSortId())
    {}

    /// Constructs a sort with internal representation t.
    ///
    sort_identifier(ATermAppl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortId(m_term));
    }

    sort_identifier(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_SortId(m_term));
    }

    sort_identifier(identifier_string name)
      : aterm_appl(gsMakeSortId(name))
    {
      assert(detail::check_rule_SortId(m_term));
    }

    /// Constructs a sort from a string.
    ///
    sort_identifier(std::string s)
      : aterm_appl(gsMakeSortId(gsString2ATermAppl(s.c_str())))
    {}

    /// Returns the name of the sort id.
    identifier_string name() const
    {
      return atermpp::arg1(*this);
    }
    
    /// Returns true if it is a sort of type A -> B.
    ///
    bool is_arrow() const
    {
      return false;
    }
};

/// \brief Returns true if the term t is a sort
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
