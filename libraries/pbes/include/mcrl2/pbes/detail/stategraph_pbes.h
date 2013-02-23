// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H

#include <cassert>
#include <iostream>
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

typedef std::vector<std::pair<propositional_variable_instantiation, pbes_expression> > predicate_variable_vector;

inline
std::string print_equation(const pbes_equation& eq)
{
  return (eq.symbol().is_mu() ? "mu " : "nu ")  + pbes_system::pp(eq.variable()) + " = " + pbes_system::pp(eq.formula());
}

class stategraph_equation: public pbes_equation
{
  protected:
    predicate_variable_vector m_predvars;
    std::vector<data::variable> m_parameters;
    pbes_expression m_condition;

  public:
    stategraph_equation(const pbes_equation& eqn)
      : pbes_equation(eqn)
    {
      pbes_system::detail::guard_traverser f;
      f(eqn.formula());
      m_predvars = f.expression_stack.back().guards;
      m_condition = f.expression_stack.back().condition;
      data::variable_list params = variable().parameters();
      m_parameters = std::vector<data::variable>(params.begin(), params.end());
    }

    bool is_simple() const
    {
      for (predicate_variable_vector::const_iterator i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        // TODO check this
        if (!pbes_system::is_false(i->second))
        {
          return false;
        }
      }
      return true;
    }

    const pbes_expression& simple_guard() const
    {
      return m_condition;
    }

    const std::vector<data::variable>& parameters() const
    {
      return m_parameters;
    }

    const predicate_variable_vector& predicate_variables() const
    {
      return m_predvars;
    }

    predicate_variable_vector& predicate_variables()
    {
      return m_predvars;
    }

    std::string print() const
    {
      std::ostringstream out;
      out << "equation = " << print_equation(*this) << std::endl;
      out << "guards:" << std::endl;
      for (predicate_variable_vector::const_iterator i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        out << "variable = " << pbes_system::pp(i->first) << " guard = " << pbes_system::pp(i->second) << std::endl;
      }
      out << "simple = " << std::boolalpha << is_simple() << std::endl;
      return out.str();
    }
};

// explicit representation of a pbes in STATEGRAPH format
class stategraph_pbes
{
  protected:
    data::data_specification m_data;
    std::vector<stategraph_equation> m_equations;
    std::set<data::variable> m_global_variables;
    pbes_expression m_initial_state;

  public:
    stategraph_pbes()
    {}

    /// \brief Constructor
    /// \pre The pbes p must be in STATEGRAPH format
    stategraph_pbes(const pbes<>& p)
      : m_data(p.data()), m_global_variables(p.global_variables()), m_initial_state(p.initial_state())
    {
      const std::vector<pbes_equation>& equations = p.equations();
      for (std::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(stategraph_equation(*i));
      }
    }

    const std::vector<stategraph_equation>& equations() const
    {
      return m_equations;
    }

    std::vector<stategraph_equation>& equations()
    {
      return m_equations;
    }

    const std::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    std::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    const pbes_expression& initial_state() const
    {
      return m_initial_state;
    }

    const data::data_specification& data() const
    {
      return m_data;
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
