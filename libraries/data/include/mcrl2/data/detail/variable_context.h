// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/variable_context.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_VARIABLE_CONTEXT_H
#define MCRL2_DATA_DETAIL_VARIABLE_CONTEXT_H

#include "mcrl2/data/variable.h"

namespace mcrl2::data
{

class data_type_checker;

namespace detail {

// Throws an exception if the names of the variables are not unique
inline
void check_duplicate_variable_names(const data::variable_list& x, const std::string& msg)
{
  std::set<core::identifier_string> names;
  for (const data::variable& v: x)
  {
    auto p = names.insert(v.name());
    if (!p.second)
    {
      throw mcrl2::runtime_error("Duplicate " + msg + " " + std::string(v.name()) + " encountered");
    }
  }
}

class variable_context
{
  private:
    std::map<core::identifier_string, sort_expression> m_variables;

  protected:
    void typecheck_variable(const data_type_checker& typechecker, const variable& v) const;

  public:
    variable_context()
    { }

    explicit variable_context(const std::map<core::identifier_string, sort_expression>& variables)
      : m_variables(variables)
    { }

    const std::map<core::identifier_string, sort_expression>& context() const
    {
      return m_variables;
    }

    // Adds the elements of variables to variable_map without checking.
    template <typename VariableContainer>
    void add_context_variables(const VariableContainer& variables)
    {
      for (const data::variable& v: variables)
      {
          m_variables[v.name()] = v.sort();
      }
    }


    // Adds the elements of variables to variable_map
    // Throws an exception if a typecheck error is encountered
    template <typename VariableContainer>
    void add_context_variables(const VariableContainer& variables, const data_type_checker& typechecker)
    {
      // first remove the existing entries
      for (const data::variable& v: variables)
      {
        m_variables.erase(v.name());
      }

      for (const data::variable& v: variables)
      {
        typecheck_variable(typechecker, v);
        auto i = m_variables.find(v.name());
        if (i == m_variables.end())
        {
          m_variables[v.name()] = v.sort();
        }
        else
        {
          throw mcrl2::runtime_error("attempt to overload global variable " + core::pp(v.name()));
        }
      }
    }

    void clear()
    {
      m_variables.clear();
    }
};

} // namespace detail

} // namespace mcrl2::data

#endif // MCRL2_DATA_DETAIL_VARIABLE_CONTEXT_H
