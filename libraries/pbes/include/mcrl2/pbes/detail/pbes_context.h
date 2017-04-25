// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_context.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_CONTEXT_H
#define MCRL2_PBES_DETAIL_PBES_CONTEXT_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/pbes/propositional_variable.h"
#include <map>

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class pbes_context
{
  private:
    std::map<core::identifier_string, propositional_variable> m_propositional_variables;

  public:
    bool is_declared(const core::identifier_string& name) const
    {
      return m_propositional_variables.find(name) != m_propositional_variables.end();
    }

    data::sort_expression_list propositional_variable_sorts(const core::identifier_string& name) const
    {
      auto i = m_propositional_variables.find(name);
      assert(i != m_propositional_variables.end());
      return data::detail::parameter_sorts(i->second.parameters());
    }

    template <typename PropositionalVariableContainer>
    void add_propositional_variables(const PropositionalVariableContainer& propositional_variables, const data::sort_type_checker& sort_typechecker)
    {
      for (const propositional_variable& p: propositional_variables)
      {
        for (const data::variable& v: p.parameters())
        {
          sort_typechecker(v.sort());
        }

        auto i = m_propositional_variables.find(p.name());
        if (i == m_propositional_variables.end())
        {
          m_propositional_variables[p.name()] = p;
        }
        else
        {
          throw mcrl2::runtime_error("attempt to overload propositional variable " + core::pp(p.name()));
        }
      }
    }

    void clear()
    {
      m_propositional_variables.clear();
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_CONTEXT_H
