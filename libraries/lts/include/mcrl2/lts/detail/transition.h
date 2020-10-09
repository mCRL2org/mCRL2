// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef MCRL2_LTS_DETAIL_TRANSITION_H
#define MCRL2_LTS_DETAIL_TRANSITION_H

#include <set>
#include "mcrl2/lts/transition.h"

namespace mcrl2
{
namespace lts
{

namespace detail
{

inline std::size_t apply_hidden_labels(const std::size_t n, const std::set<std::size_t>& hidden_action_set)
{
  if (hidden_action_set.count(n)==0)
  {
    return n;
  }
  return lts::const_tau_label_index;  // 0 is the index of the internal action. 
}

class compare_transitions_slt
{
  protected:
    const std::set<std::size_t>& m_hide_action_set;

  public:
    compare_transitions_slt(const std::set<std::size_t>& hide_action_set)
     : m_hide_action_set(hide_action_set)
    {}

    bool operator()(const transition& t1, const transition& t2)
    {
      if (t1.from() != t2.from())
      {
        return t1.from() < t2.from();
      }
      else 
      {
        const std::size_t n1=apply_hidden_labels(t1.label(), m_hide_action_set);
        const std::size_t n2=apply_hidden_labels(t2.label(), m_hide_action_set);
        if (n1 != n2)
        {
          return n1 < n2;
        }
        else
        {
          return t1.to() < t2.to();
        }
      }
    }
};

class compare_transitions_lts
{
  protected:
    const std::set<std::size_t>& m_hide_action_set;

  public:
    compare_transitions_lts(const std::set<std::size_t>& hide_action_set)
     : m_hide_action_set(hide_action_set)
    {}

    bool operator()(const transition& t1, const transition& t2)
    {
      const std::size_t n1=apply_hidden_labels(t1.label(), m_hide_action_set);
      const std::size_t n2=apply_hidden_labels(t2.label(), m_hide_action_set);
      if (n1 != n2)
      {
        return n1 < n2;
      }
      else if (t1.to() != t2.to())
      {
        return t1.to() < t2.to();
      }
      else
      {
        return t1.from() < t2.from();
      }
    }
};

} // detail
} // lts
} // mcrl2

#endif
