// Author(s): Jan Friso Groote. Based on pbes/detail/pbes_context by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/pres_context.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_PRES_CONTEXT_H
#define MCRL2_PRES_DETAIL_PRES_CONTEXT_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/pbes/propositional_variable.h"





namespace mcrl2::pres_system::detail {

class pres_context
{
  private:
    std::map<core::identifier_string, pbes_system::propositional_variable> m_propositional_variables;

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
      for (const pbes_system::propositional_variable& p: propositional_variables)
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

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_PRES_CONTEXT_H
