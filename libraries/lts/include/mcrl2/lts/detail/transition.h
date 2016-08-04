// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef MCRL2_LTS_DETAIL_TRANSITION_H
#define MCRL2_LTS_DETAIL_TRANSITION_H

#include "map"
#include "mcrl2/lts/transition.h"

namespace mcrl2
{
namespace lts
{

namespace detail
{

inline size_t apply_map(const size_t n, const std::map<size_t,size_t>& mapping)
{
  const std::map<size_t,size_t>::const_iterator i=mapping.find(n);
  if (i==mapping.end())
  {
    return n;
  }
  return i->second;
}

class compare_transitions_slt
{
  protected:
    const std::map<size_t,size_t>& m_hide_action_map;

  public:
    compare_transitions_slt(const std::map<size_t,size_t>& hide_action_map)
     : m_hide_action_map(hide_action_map)
    {}

    bool operator()(const transition& t1, const transition& t2)
    {
      if (t1.from() != t2.from())
      {
        return t1.from() < t2.from();
      }
      else 
      {
        const size_t n1=apply_map(t1.label(), m_hide_action_map);
        const size_t n2=apply_map(t2.label(), m_hide_action_map);
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
    const std::map<size_t,size_t>& m_hide_action_map;

  public:
    compare_transitions_lts(const std::map<size_t,size_t>& hide_action_map)
     : m_hide_action_map(hide_action_map)
    {}

    bool operator()(const transition& t1, const transition& t2)
    {
      const size_t n1=apply_map(t1.label(), m_hide_action_map);
      const size_t n2=apply_map(t2.label(), m_hide_action_map);
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
