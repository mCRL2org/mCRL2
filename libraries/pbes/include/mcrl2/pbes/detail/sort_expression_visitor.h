// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/sort_expression_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_SORT_EXPRESSION_VISITOR_H
#define MCRL2_PBES_DETAIL_SORT_EXPRESSION_VISITOR_H

#include <set>
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Term>
struct sort_expression_visitor: public pbes_expression_visitor<Term>
{
  typedef pbes_expression_visitor<Term> super;
  typedef typename super::term_type term_type;
  typedef typename super::data_term_type data_term_type;
  typedef typename super::propositional_variable_type propositional_variable_type;

  std::set<data::sort_expression> result;

  /// \brief Visit data_expression node
  /// \param e A term
  /// \param d A data term
  /// \return The result of visiting the node
  bool visit_data_expression(const term_type& e, const data_term_type& d)
  {
    result.insert(d.sort());
    return true;
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_SORT_EXPRESSION_VISITOR_H
