// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/abstract.h
/// \brief The PBES abstract algorithm.

#ifndef MCRL2_PBES_ABSTRACT_H
#define MCRL2_PBES_ABSTRACT_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/pbes.h"
#include <algorithm>
#include <iostream>

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// \brief Visitor that implements the pbes-abstract algorithm.
struct pbes_abstract_builder: public pbes_expression_builder<pbes_abstract_builder>
{
  typedef pbes_expression_builder<pbes_abstract_builder> super;
  using super::apply;

  std::vector<data::variable_list> m_quantifier_stack;
  const std::vector<data::variable> m_selected_variables;
  const data::data_expression m_value;

  pbes_abstract_builder(const std::vector<data::variable>& selected_variables, bool value_true)
    : m_selected_variables(selected_variables),
      m_value(value_true ? data::true_() : data::false_())
  {}

  /// \brief Returns true if the m_quantifier_stack contains a given data variable
  bool is_bound(const data::variable& v) const
  {
    for (const data::variable_list& variables: m_quantifier_stack)
    {
      for (const data::variable& w: variables)
      {
        if (w == v)
        {
          return true;
        }
      }
    }
    return false;
  }

  /// \brief Adds a sequence of variables to the quantifier stack.
  void push_variables(const data::variable_list& variables)
  {
    m_quantifier_stack.push_back(variables);
  }

  /// \brief Removes the last added sequence of variables from the quantifier stack.
  void pop_variables()
  {
    m_quantifier_stack.pop_back();
  }

  /// \brief Visit data_expression node
  pbes_expression apply(const data::data_expression& d)
  {
    std::set<data::variable> FV = data::find_free_variables(d);
    for (const data::variable& v: FV)
    {
      if (std::find(m_selected_variables.begin(), m_selected_variables.end(), v) == m_selected_variables.end())
      {
        continue;
      }
      if (!is_bound(v))
      {
        //std::clog << "Reducing data expression " << data::pp(d) << " to " << data::pp(m_value) << "." << std::endl;
        return m_value;
      }
    }
    return d;
  }

  /// \brief Visit forall node
  pbes_expression apply(const forall& x)
  {
    push_variables(x.variables());
    pbes_expression new_expression = apply(x.body());
    pop_variables();
    return make_forall(x.variables(), new_expression);
  }

  /// \brief Visit exists node
  pbes_expression apply(const exists& x)
  {
    push_variables(x.variables());
    pbes_expression new_expression = apply(x.body());
    pop_variables();
    return make_exists(x.variables(), new_expression);
  }
};

} // namespace detail


/// \brief Algorithm class for the abstract algorithm
class pbes_abstract_algorithm
{
  public:
    /// \brief Runs the algorithm.
    /// \param p A PBES.
    /// \param parameter_map A map containing the parameters that should be expanded by the algorithm.
    /// \param value_true An indication whether the abstraction is towards true or towards false.
    void run(pbes& p,
             const detail::pbes_parameter_map& parameter_map,
             bool value_true
            )
    {
      for (pbes_equation& eqn: p.equations())
      {
        detail::pbes_parameter_map::const_iterator j = parameter_map.find(eqn.variable().name());
        if (j != parameter_map.end())
        {
          detail::pbes_abstract_builder builder(j->second, value_true);
          eqn.formula() = builder.apply(eqn.formula());
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ABSTRACT_H
