// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file quantifier_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H
#define MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H

#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace lps {

namespace detail {

struct quantifier_visitor: public pbes_expression_visitor
{
  std::set<data_variable> variables;

  bool visit_forall(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    variables.insert(v.begin(), v.end());
    return false;
  }

  bool visit_exists(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    variables.insert(v.begin(), v.end());
    return false;
  }
};  

} // namespace detail

} // namespace lps

#endif // MCRL2_PBES_DETAIL_QUANTIFIER_VISITOR_H
