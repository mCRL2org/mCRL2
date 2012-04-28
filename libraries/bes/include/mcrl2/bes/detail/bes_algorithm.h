// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/bes_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_BES_DETAIL_BES_ALGORITHM_H
#define MCRL2_BES_DETAIL_BES_ALGORITHM_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/print.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

/// \brief Algorithm class for algorithms on linear process specifications.
template <typename Container = std::vector<boolean_equation> >
class bes_algorithm
{
  protected:
    /// \brief The specification that is processed by the algorithm
    boolean_equation_system<Container>& m_bes;

  public:
    /// \brief Constructor
    bes_algorithm(boolean_equation_system<Container>& bes)
      : m_bes(bes)
    {}

    /// \brief Flag for verbose output
    bool verbose() const
    {
      return mCRL2logEnabled(log::verbose);
    }

    /// \brief Perform reachability analysis on equations of m_bes, and remove
    /// those that are unreachable.
    bool remove_unreachable_equations()
    {
      std::deque<boolean_variable> todo;
      std::set<boolean_variable> reachable;

      // For efficiency reasons, store equation X = phi as mapping X -> X = phi
      std::map<boolean_variable, boolean_equation> equations;
      for(typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
      {
        equations[i->variable()] = *i;
      }

      reachable = find_boolean_variables(m_bes.initial_state());
      todo.insert(todo.end(), reachable.begin(), reachable.end());

      while(!todo.empty())
      {
        boolean_equation eqn = equations[todo.front()];
        todo.pop_front();
        std::set<boolean_variable> occ = find_boolean_variables(eqn.formula());
        for(std::set<boolean_variable>::const_iterator i = occ.begin(); i != occ.end(); ++i)
        {
          bool fresh = reachable.insert(*i).second;
          if(fresh)
          {
            todo.push_back(*i);
          }
        }
      }

      if(reachable.size() == m_bes.equations().size())
      {
        return true;
      }

      Container reachable_equations;
      std::set<boolean_variable> unreachable_equations;
      for(typename Container::const_iterator i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
      {
        if(reachable.find(i->variable()) != reachable.end())
        {
          reachable_equations.insert(reachable_equations.end(), *i);
        }
        else
        {
          unreachable_equations.insert(i->variable());
        }
      }

      m_bes.equations() = reachable_equations;

      mCRL2log(log::verbose) << "Removed the following unreachable equations: " << bes::pp(unreachable_equations) << std::endl;

      return false;
    }

};

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
