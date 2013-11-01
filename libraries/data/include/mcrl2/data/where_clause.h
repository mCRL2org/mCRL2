// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/where_clause.h
/// \brief The class where_clause.

#ifndef MCRL2_DATA_WHERE_CLAUSE_H
#define MCRL2_DATA_WHERE_CLAUSE_H

#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/assignment.h"

namespace mcrl2
{

namespace data
{

//--- start generated class where_clause ---//
/// \brief A where expression
class where_clause: public data_expression
{
  public:
    /// \brief Default constructor.
    where_clause()
      : data_expression(core::detail::constructWhr())
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit where_clause(const atermpp::aterm& term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Whr(*this));
    }

    /// \brief Constructor.
    where_clause(const data_expression& body, const assignment_expression_list& declarations)
      : data_expression(core::detail::gsMakeWhr(body, declarations))
    {}

    /// \brief Constructor.
    template <typename Container>
    where_clause(const data_expression& body, const Container& declarations, typename atermpp::detail::enable_if_container<Container, assignment_expression>::type* = 0)
      : data_expression(core::detail::gsMakeWhr(body, assignment_expression_list(declarations.begin(), declarations.end())))
    {}

    const data_expression& body() const
    {
      return atermpp::aterm_cast<const data_expression>(atermpp::arg1(*this));
    }

    const assignment_expression_list& declarations() const
    {
      return atermpp::aterm_cast<const assignment_expression_list>(atermpp::list_arg2(*this));
    }
//--- start user section where_clause ---//
    const assignment_list& assignments() const
    {
      return atermpp::aterm_cast<const assignment_list>(atermpp::list_arg2(*this));
    }
//--- end user section where_clause ---//
};

/// \brief swap overload
inline void swap(where_clause& t1, where_clause& t2)
{
  t1.swap(t2);
}

//--- end generated class where_clause ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_WHERE_CLAUSE_H

