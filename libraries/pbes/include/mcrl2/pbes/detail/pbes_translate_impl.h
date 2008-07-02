// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/old_data/detail/find.h"
#include "mcrl2/old_data/utility.h"
#include "mcrl2/old_data/data_operators.h"
#include "mcrl2/old_data/data_expression.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/old_data/detail/sorted_sequence_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/*
inline
std::string pp(std::set<old_data::data_variable> s)
{
  return ::pp(old_data::data_variable_list(s.begin(), s.end()));
}
*/

inline
std::set<old_data::data_variable> compute_free_pbes_expression_variables(const pbes_expression& e)
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

  bool operator()(atermpp::aterm_appl t)
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

/// \brief Returns the variables corresponding to ass(f)
inline
old_data::data_variable_list mu_variables(modal::state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  old_data::data_assignment_list l = modal::state_frm::ass(f);
  old_data::data_variable_list result;
  for(old_data::data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->lhs());
  }
  return atermpp::reverse(result);
}

/// \brief Returns the data expressions corresponding to ass(f)
inline
old_data::data_expression_list mu_expressions(modal::state_formula f)
{
  assert(core::detail::gsIsStateMu(f) || core::detail::gsIsStateNu(f));
  old_data::data_assignment_list l = modal::state_frm::ass(f);
  old_data::data_expression_list result;
  for(old_data::data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_TRANSLATE_IMPL_H
