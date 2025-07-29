// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_variable_context.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_CONTEXT_H
#define MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_CONTEXT_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2::state_formulas::detail {

class state_variable_context
{
  private:
    std::map<core::identifier_string, data::sort_expression_list> m_state_variables;

  public:
    bool is_declared(const core::identifier_string& name) const
    {
      return m_state_variables.find(name) != m_state_variables.end();
    }

    void add_state_variable(const core::identifier_string& name, const data::variable_list& parameters, const data::sort_type_checker& sort_typechecker)
    {
      data::sort_expression_list sorts = data::detail::parameter_sorts(parameters);
      for (const data::sort_expression& s: sorts)
      {
        sort_typechecker(s);
      }
      m_state_variables[name] = sorts;
    }

    data::sort_expression_list matching_state_variable_sorts(const core::identifier_string& name, const data::data_expression_list& arguments) const
    {
      auto i = m_state_variables.find(name);
      if (i == m_state_variables.end())
      {
        throw mcrl2::runtime_error("undefined state variable " + core::pp(name) + " with arguments " + data::pp(arguments));
      }
      const data::sort_expression_list& result = i->second;
      if (result.size() != arguments.size())
      {
        throw mcrl2::runtime_error("incorrect number of arguments for state variable " + core::pp(name));
      }
      return result;
    }

    void clear()
    {
      m_state_variables.clear();
    }
};

} // namespace mcrl2::state_formulas::detail





#endif // MCRL2_MODAL_FORMULA_DETAIL_STATE_VARIABLE_CONTEXT_H
