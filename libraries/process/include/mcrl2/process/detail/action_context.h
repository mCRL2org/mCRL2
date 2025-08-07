// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/action_context.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H
#define MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H

#include "mcrl2/data/typecheck.h"
#include "mcrl2/process/action_label.h"

namespace mcrl2::process::detail
{

class action_context
{
  private:
    std::multimap<core::identifier_string, action_label> m_actions;

  public:
    bool is_declared(const core::identifier_string& name) const
    {
      return m_actions.find(name) != m_actions.end();
    }

    // Adds the elements of actions to action_map
    // Throws an exception if the sorts of the actions are not declared
    template <typename ActionLabelContainer>
    void add_context_action_labels(const ActionLabelContainer& actions, const data::sort_type_checker& sort_typechecker)
    {
      for (const process::action_label& a: actions)
      {
        for (const data::sort_expression& s: a.sorts())
        {
          sort_typechecker(s);
        }

        // Insert a in m_action_context; N.B. Before that check if it already exists
        auto range = m_actions.equal_range(a.name());
        if (range.first != m_actions.end())
        {
          for (auto i = range.first; i != range.second; ++i)
          {
            if (i->second == a)
            {
              throw mcrl2::runtime_error("double declaration of action " + process::pp(a));
            }
          }
        }
        m_actions.insert(range.first, std::make_pair(a.name(), a));
      }
    }

    std::set<data::sort_expression_list> matching_action_sorts(const core::identifier_string& name) const
    {
      auto range = m_actions.equal_range(name);
      assert(range.first != m_actions.end());
      std::set<data::sort_expression_list> result;
      for (auto k = range.first; k != range.second; ++k)
      {
        const action_label& a = k->second;
        result.insert(a.sorts());
      }
      return result;
    }

    std::set<data::sort_expression_list> matching_action_sorts(const core::identifier_string& name, const data::data_expression_list& parameters) const
    {
      std::set<data::sort_expression_list> result;
      auto range = m_actions.equal_range(name);
      for (auto k = range.first; k != range.second; ++k)
      {
        const action_label& a = k->second;
        if (a.sorts().size() == parameters.size())
        {
          result.insert(a.sorts());
        }
      }
      return result;
    }

    void clear()
    {
      m_actions.clear();
    }
};

} // namespace mcrl2::process::detail

#endif // MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H
