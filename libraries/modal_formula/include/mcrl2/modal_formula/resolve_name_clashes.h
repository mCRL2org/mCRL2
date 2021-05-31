// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/state_formula_name_clash_resolver.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_RESOLVE_NAME_CLASHES_H
#define MCRL2_MODAL_FORMULA_RESOLVE_NAME_CLASHES_H

#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2::state_formulas {

namespace detail
{

template <typename Derived>
class state_variable_name_clash_resolver: public state_formulas::state_formula_builder<Derived>
{
  public:
    typedef state_formulas::state_formula_builder<Derived> super;

    using super::enter;
    using super::leave;
    using super::apply;

    typedef std::map<core::identifier_string, std::vector<core::identifier_string> > name_map;

    /// \brief The stack of names.
    name_map m_names;

    /// \brief Generator for fresh variable names.
    utilities::number_postfix_generator m_generator;

    /// \brief Pops the name of the stack
    void pop(const core::identifier_string& name)
    {
      m_names[name].pop_back();
    }

    /// \brief Pushes name on the stack.
    void push(const core::identifier_string& name)
    {
      std::vector<core::identifier_string>& names = m_names[name];
      if (names.empty())
      {
        names.push_back(name);
      }
      else
      {
        names.push_back(m_generator(std::string(name) + "_"));
      }
    }

    void enter(const mu& x)
    {
      push(x.name());
    }

    void leave(const mu& x)
    {
      pop(x.name());
    }

    void enter(const nu& x)
    {
      push(x.name());
    }

    void leave(const nu& x)
    {
      pop(x.name());
    }

    // Rename variable
    state_formula apply(const mu& x)
    {
      enter(x);
      // N.B. If the two lines below are replace by
      //   state_formula result = mu(m_names[x.name()].back(), x.assignments(), (*this)(x.operand()));
      // a memory error occurs with the clang and intel compilers!
      core::identifier_string name = m_names[x.name()].back();
      state_formula result = mu(name, x.assignments(), apply(x.operand()));
      leave(x);
      return result;
    }

    // Rename variable
    state_formula apply(const nu& x)
    {
      enter(x);
      // N.B. If the two lines below are replace by
      //   state_formula result = nu(m_names[x.name()].back(), x.assignments(), (*this)(x.operand()));
      // a memory error occurs with the clang and intel compilers!
      core::identifier_string name = m_names[x.name()].back();
      state_formula result = nu(name, x.assignments(), apply(x.operand()));
      leave(x);
      return result;
    }

    // Rename variable
    state_formula apply(const variable& x)
    {
      return variable(m_names[x.name()].back(), x.arguments());
    }
};

class state_formula_data_variable_name_clash_resolver: public state_formulas::data_expression_builder<state_formula_data_variable_name_clash_resolver>
{
  public:
    typedef state_formulas::data_expression_builder<state_formula_data_variable_name_clash_resolver> super;

    using super::enter;
    using super::leave;
    using super::apply;

    data::set_identifier_generator& generator;
    std::multiset<data::variable> bound_variables;
    std::map<data::variable, std::vector<data::variable>> substitutions;

    explicit state_formula_data_variable_name_clash_resolver(data::set_identifier_generator& generator_)
     : generator(generator_)
    {}

    void insert(const data::variable& v)
    {
      if (utilities::detail::contains(bound_variables, v))
      {
        substitutions[v].push_back(data::variable(generator(v.name()), v.sort()));
      }
      bound_variables.insert(v);
    }

    void erase(const data::variable& v)
    {
      bound_variables.erase(v);
      auto i = substitutions.find(v);
      if (i != substitutions.end())
      {
        i->second.pop_back();
        if (i->second.empty())
        {
          substitutions.erase(i);
        }
      }
    }

    data::assignment_list apply_assignments(const data::assignment_list& x)
    {
      auto sigma = [&](const data::variable& v) -> data::data_expression
      {
        auto i = substitutions.find(v);
        if (i == substitutions.end())
        {
          return v;
        }
        return i->second.back();
      };

      return data::assignment_list(x.begin(), x.end(), [&](const data::assignment& a)
        {
          return data::assignment(atermpp::down_cast<data::variable>(sigma(a.lhs())), data::replace_free_variables(a.rhs(), sigma));
        }
      );
    }

    data::variable_list apply_variables(const data::variable_list& x)
    {
      auto sigma = [&](const data::variable& v) -> data::data_expression
      {
        auto i = substitutions.find(v);
        if (i == substitutions.end())
        {
          return v;
        }
        return i->second.back();
      };

      return data::variable_list(x.begin(), x.end(), [&](const data::variable& v)
                                   {
                                     return atermpp::down_cast<data::variable>(sigma(v));
                                   }
      );
    }

    state_formula apply(const mu& x)
    {
      for (const data::assignment& a: x.assignments())
      {
        insert(a.lhs());
      }
      state_formula result = mu(x.name(), apply_assignments(x.assignments()), apply(x.operand()));
      for (const data::assignment& a: x.assignments())
      {
        erase(a.lhs());
      }
      return result;
    }

    state_formula apply(const nu& x)
    {
      for (const data::assignment& a: x.assignments())
      {
        insert(a.lhs());
      }
      state_formula result = nu(x.name(), apply_assignments(x.assignments()), apply(x.operand()));
      for (const data::assignment& a: x.assignments())
      {
        erase(a.lhs());
      }
      return result;
    }

    state_formula apply(const forall& x)
    {
      for (const data::variable& v: x.variables())
      {
        insert(v);
      }
      state_formula result = forall(apply_variables(x.variables()), apply(x.body()));
      for (const data::variable& v: x.variables())
      {
        erase(v);
      }
      return result;
    }

    state_formula apply(const exists& x)
    {
      for (const data::variable& v: x.variables())
      {
        insert(v);
      }
      state_formula result = exists(apply_variables(x.variables()), apply(x.body()));
      for (const data::variable& v: x.variables())
      {
        erase(v);
      }
      return result;
    }

    action_formulas::action_formula apply(const action_formulas::forall& x)
    {
      for (const data::variable& v: x.variables())
      {
        insert(v);
      }
      action_formulas::action_formula result = action_formulas::forall(apply_variables(x.variables()), apply(x.body()));
      for (const data::variable& v: x.variables())
      {
        erase(v);
      }
      return result;
    }

    action_formulas::action_formula apply(const action_formulas::exists& x)
    {
      for (const data::variable& v: x.variables())
      {
        insert(v);
      }
      action_formulas::action_formula result = action_formulas::exists(apply_variables(x.variables()), apply(x.body()));
      for (const data::variable& v: x.variables())
      {
        erase(v);
      }
      return result;
    }

    data::data_expression apply(const data::data_expression& x)
    {
      auto sigma = [&](const data::variable& v) -> data::data_expression
      {
        auto i = substitutions.find(v);
        if (i == substitutions.end())
        {
          return v;
        }
        return i->second.back();
      };

      return data::replace_free_variables(x, sigma);
    }
};

} // namespace detail

/// \brief Resolves name clashes in state variables of formula x
inline
state_formula resolve_state_variable_name_clashes(const state_formula& x)
{
  return core::make_apply_builder<detail::state_variable_name_clash_resolver>().apply(x);
}

/// \brief Resolves name clashes in data variables of formula x
inline
state_formula resolve_state_formula_data_variable_name_clashes(const state_formula& x, const std::set<core::identifier_string>& context_ids = std::set<core::identifier_string>())
{
  data::set_identifier_generator generator;
  generator.add_identifiers(state_formulas::find_identifiers(x));
  generator.add_identifiers(context_ids);
  detail::state_formula_data_variable_name_clash_resolver f(generator);
  return f.apply(x);
}

} // namespace mcrl2::state_formulas

#endif // MCRL2_MODAL_FORMULA_RESOLVE_NAME_CLASHES_H
