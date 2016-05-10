// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/action_context.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H
#define MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H

#include <map>
#include "mcrl2/data/sort_type_checker.h"
#include "mcrl2/process/action_label.h"

namespace mcrl2 {

namespace process {

namespace detail {

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

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
        core::identifier_string name = a.name();
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

    sorts_list matching_action_sorts(const core::identifier_string& name) const
    {
      auto range = m_actions.equal_range(name);
      assert(range.first != m_actions.end());
      sorts_list result;
      for (auto k = range.first; k != range.second; ++k)
      {
        const action_label& a = k->second;
        result.push_front(a.sorts());
      }
      return atermpp::reverse(result);
    }

    sorts_list matching_action_sorts(const core::identifier_string& name, const data::data_expression_list& parameters) const
    {
      sorts_list result;
      auto range = m_actions.equal_range(name);
      for (auto k = range.first; k != range.second; ++k)
      {
        const action_label& a = k->second;
        if (a.sorts().size() == parameters.size())
        {
          result.push_front(a.sorts());
        }
      }
      return atermpp::reverse(result);
    }

    void clear()
    {
      m_actions.clear();
    }
};

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ACTION_CONTEXT_H
