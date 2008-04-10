// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_translate_impl.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
#define MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <map>

#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/data_operators.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

using atermpp::aterm_appl;
using atermpp::make_list;
using atermpp::make_substitution;
using modal::state_formula;

/*
inline
std::string pp(std::set<data::data_variable> s)
{
  return ::pp(data::data_variable_list(s.begin(), s.end()));
}
*/

inline
std::set<data::data_variable> compute_free_pbes_expression_variables(const pbes_expression& e)
{
  free_variable_visitor visitor;
  visitor.visit(e);
  return visitor.result;
}

/// \brief Visitor that collects the names of propositional variables + instantiations
struct prop_var_visitor
{
  std::set<core::identifier_string>& m_identifiers;

  prop_var_visitor(std::set<core::identifier_string>& identifiers)
    : m_identifiers(identifiers)
  {}

  bool operator()(aterm_appl t)
  {
    bool result = true;
    if (is_propositional_variable(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable(t).name());
    }
    if (is_propositional_variable_instantiation(t))
    {
      result = false; // stop the recursion
      m_identifiers.insert(propositional_variable_instantiation(t).name());
    }
    return result;
  }
};

/// \brief Returns the names of all propositional variables that occur in the term t
template <typename Term>
std::set<core::identifier_string> propositional_variable_names(Term t)
{
  std::set<core::identifier_string> result;
  atermpp::for_each(t, prop_var_visitor(result));
  return result;
}

/// \brief Returns the variables corresponding to mu_params(f)
inline
data::data_variable_list mu_variables(state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  data::data_assignment_list l = modal::state_frm::mu_params(f);
  data::data_variable_list result;
  for(data::data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->lhs());
  }
  return atermpp::reverse(result);
}

/// \brief Returns the data expressions corresponding to mu_params(f)
inline
data::data_expression_list mu_expressions(state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  data::data_assignment_list l = modal::state_frm::mu_params(f);
  data::data_expression_list result;
  for(data::data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
