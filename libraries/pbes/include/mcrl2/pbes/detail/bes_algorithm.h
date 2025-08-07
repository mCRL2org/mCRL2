// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/bes_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_BES_ALGORITHM_H
#define MCRL2_PBES_DETAIL_BES_ALGORITHM_H

#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/print.h"

namespace mcrl2::pbes_system::detail
{

/// \brief Algorithm class for algorithms on linear process specifications.
class bes_algorithm
{
  protected:
    /// \brief The specification that is processed by the algorithm
    pbes& m_bes;

  public:
    /// \brief Constructor
    bes_algorithm(pbes& bes)
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
      std::deque<propositional_variable_instantiation> todo;
      std::set<propositional_variable_instantiation> reachable;

      // For efficiency reasons, store equation X = phi as mapping X -> X = phi
      std::map<propositional_variable_instantiation, pbes_equation> equations;
      for(const pbes_equation& e: m_bes.equations())
      {
        equations[propositional_variable_instantiation(e.variable().name())] = e;
      }

      reachable = find_propositional_variable_instantiations(m_bes.initial_state());
      todo.insert(todo.end(), reachable.begin(), reachable.end());

      while(!todo.empty())
      {
        pbes_equation eqn = equations[todo.front()];
        todo.pop_front();
        std::set<propositional_variable_instantiation> occ = find_propositional_variable_instantiations(eqn.formula());
        for(const propositional_variable_instantiation& v: occ)
        {
          bool fresh = reachable.insert(v).second;
          if(fresh)
          {
            todo.push_back(v);
          }
        }
      }

      if(reachable.size() == m_bes.equations().size())
      {
        return true;
      }

      std::vector<pbes_equation> reachable_equations;
      std::set<propositional_variable> unreachable_equations;
      for(auto i = m_bes.equations().begin(); i != m_bes.equations().end(); ++i)
      {
        if(reachable.find(propositional_variable_instantiation(i->variable().name())) != reachable.end())
        {
          reachable_equations.insert(reachable_equations.end(), *i);
        }
        else
        {
          unreachable_equations.insert(i->variable());
        }
      }

      m_bes.equations() = reachable_equations;

      mCRL2log(log::verbose) << "Removed the following unreachable equations: " << pbes_system::pp(unreachable_equations) << std::endl;

      return false;
    }

};

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_BES_ALGORITHM_H
