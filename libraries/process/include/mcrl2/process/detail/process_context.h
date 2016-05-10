// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/process_context.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_PROCESS_CONTEXT_H
#define MCRL2_PROCESS_DETAIL_PROCESS_CONTEXT_H

#include <algorithm>
#include <map>
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/data/detail/data_typechecker.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/detail/action_context.h"

namespace mcrl2 {

namespace process {

namespace detail {

class process_context
{
  private:
    std::multimap<core::identifier_string, process_identifier>m_process_identifiers ;

    // returns true if all left hand sides of assignments appear as the name of a variable in parameters
    bool is_matching_assignment(const data::untyped_identifier_assignment_list& assignments, const data::variable_list& parameters) const
    {
      for (const data::untyped_identifier_assignment& a: assignments)
      {
        if (std::find_if(parameters.begin(), parameters.end(), [&](const data::variable& v) { return a.lhs() == v.name(); }) == parameters.end())
        {
          return false;
        }
      }
      return true;
    }

    template <typename Container>
    data::sort_expression_list parameter_sorts(const Container& parameters) const
    {
      data::sort_expression_list sorts;
      for (const data::data_expression& e: parameters)
      {
        sorts.push_front(e.sort());
      }
      return atermpp::reverse(sorts);
    }

  public:
    bool is_declared(const core::identifier_string& name) const
    {
      return m_process_identifiers.find(name) != m_process_identifiers.end();
    }

    template <typename ProcessIdentifierContainer>
    void add_process_identifiers(const ProcessIdentifierContainer& ids, const action_context& action_ctx, const data::sort_type_checker& sort_typechecker)
    {
      for (const process_identifier& id: ids)
      {
        const core::identifier_string& name = id.name();

        if (action_ctx.is_declared(name))
        {
          throw mcrl2::runtime_error("declaration of both process and action " + core::pp(name));
        }

        // Insert id in m_process_identifiers; N.B. Before that check if it already exists
        auto range = m_process_identifiers.equal_range(id.name());
        if (range.first != m_process_identifiers.end())
        {
          for (auto i = range.first; i != range.second; ++i)
          {
            if (i->second == id)
            {
              throw mcrl2::runtime_error("double declaration of process " + process::pp(id));
            }
          }
        }
        m_process_identifiers.insert(range.first, std::make_pair(id.name(), id));

        for (const data::variable& v: id.variables())
        {
          sort_typechecker(v.sort());
        }

        //check that all formal parameters of the process are unique.
        if (!data::detail::unique_variables(id.variables()))
        {
          throw mcrl2::runtime_error("the formal variables in process " + process::pp(id) + " are not unique");
        }
      }
    }

    // returns the equation parameters that correspond to the untyped process assignment x
    data::variable_list matching_process_parameters(const untyped_process_assignment& x) const
    {
      auto range = m_process_identifiers.equal_range(x.name());
      if (range.first == m_process_identifiers.end())
      {
        throw mcrl2::runtime_error("process " + core::pp(x.name()) + " not declared");
      }
      std::vector<process_identifier> result;
      for (auto k = range.first; k != range.second; ++k)
      {
        const process_identifier& id = k->second;
        if (is_matching_assignment(x.assignments(), id.variables()))
        {
          result.push_back(id);
        }
      }
      if (result.empty())
      {
        throw mcrl2::runtime_error("no process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".\n");
      }
      if (result.size() > 1)
      {
        throw mcrl2::runtime_error("ambiguous process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".");
      }
      return result.front().variables();
    }

    process_instance make_process_instance(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters) const
    {
      auto range = m_process_identifiers.equal_range(name);
      assert(range.first != m_process_identifiers.end());
      for (auto k = range.first; k != range.second; ++k)
      {
        const process_identifier& id = k->second;
        if (parameter_sorts(id.variables()) == formal_parameters)
        {
          return process_instance(id, actual_parameters);
        }
      }
      throw mcrl2::runtime_error("no matching process found for " + core::pp(name) + "(" + data::pp(formal_parameters) + ")");
    }

    sorts_list matching_process_sorts(const core::identifier_string& name, const data::data_expression_list& parameters) const
    {
      sorts_list result;
      auto range = m_process_identifiers.equal_range(name);
      for (auto k = range.first; k != range.second; ++k)
      {
        const process_identifier& id = k->second;
        if (id.variables().size() == parameters.size())
        {
          result.push_front(parameter_sorts(id.variables()));
        }
      }
      return atermpp::reverse(result);
    }

    void clear()
    {
      m_process_identifiers.clear();
    }
};

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_PROCESS_CONTEXT_H
