// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_parameter_remover.h
/// \brief add your file description here.

// This is to deal with circular header file dependencies
#ifndef MCRL2_PBES_PBES_H
#include "mcrl2/pbes/pbes.h"
#endif

#ifndef MCRL2_PBES_DETAIL_PBES_PARAMETER_REMOVER_H
#define MCRL2_PBES_DETAIL_PBES_PARAMETER_REMOVER_H

#include <algorithm>
#include <vector>
#include "mcrl2/data/detail/assignment_functional.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// \brief Function object for removing parameters from PBES data types.
  /// These parameters can be either process parameters or free variables.
  /// Assignments to these parameters are removed as well.
  template <typename SetContainer>
  struct pbes_parameter_remover
  {
    const SetContainer& to_be_removed;
  
    pbes_parameter_remover(const SetContainer& to_be_removed_)
      : to_be_removed(to_be_removed_)
    {}

    /// \brief Removes parameters from a set container.
    template <typename SetContainer1>
    void remove_set_container(SetContainer1& c) const
    {
      for (typename SetContainer::const_iterator i = to_be_removed.begin(); i != to_be_removed.end(); ++i)
      {
        c.erase(*i);
      }
    }

    /// \brief Removes parameters from a list of variables.
    data::variable_list remove_list_copy(const data::variable_list& l) const
    {
      std::vector<data::variable> result;
      for (data::variable_list::const_iterator i = l.begin(); i != l.end(); ++i)
      {
        if (to_be_removed.find(*i) == to_be_removed.end())
        {
          result.push_back(*i);
        }
      }
      return data::variable_list(result.begin(), result.end());
    }

    /// \brief Removes parameters from a list of assignments.
    /// Assignments to removed parameters are removed.
    data::assignment_list remove_list_copy(const data::assignment_list& l) const
    {
      // TODO: make this implementation more efficient
      std::vector<data::assignment> a(l.begin(), l.end());
      a.erase(std::remove_if(a.begin(), a.end(), data::detail::has_left_hand_side_in(to_be_removed)), a.end());
      return data::assignment_list(a.begin(), a.end());
    }
  
    /// \brief Removes parameters from the elements of a term list
    template <typename TermList>
    void remove_list(TermList& l) const
    {
      l = remove_list_copy(l);
    }

    /// \brief Removes parameters from the elements of a container
    template <typename Container>
    void remove_container(Container& c) const
    {
      for (typename Container::iterator i = c.begin(); i != c.end(); ++i)
      {
        remove(*i);
      }
    }   

    /// \brief Removes parameters from a pbes expression
    /// \param t A pbes expression
    void remove(pbes_expression& t) const
    {
    } 

    /// \brief Removes parameters from a propositional variable declaration
    /// \param t A propositional variable declaration
    void remove(propositional_variable& v) const
    {
      v = propositional_variable(v.name(), remove_list_copy(v.parameters()));
    } 

    /// \brief Removes parameters from a propositional variable instantiation
    /// \param t A propositional variable instantiation
    void remove(propositional_variable_instantiation& v) const
    {
    } 

    /// \brief Removes parameters from a pbes equation
    /// \param e A pbes equation
    void remove(pbes_equation& e) const
    {
      remove(e.variable());
    } 
  
    /// \brief Removes parameters from a PBES
    /// \param spec A PBES
    template <typename Container>
    void remove(pbes<Container>& p) const
    {
      remove_container(p.equations());
      remove(p.initial_state());
      remove_set_container(p.global_variables());
    }

    template <typename Term>
    void operator()(Term& t)
    {
      remove(t);
    }
  };

  /// \brief Utility function to create an pbes_parameter_remover.
  template <typename SetContainer>    
  pbes_parameter_remover<SetContainer> make_pbes_parameter_remover(const SetContainer& to_be_removed)        
  {
    return pbes_parameter_remover<SetContainer>(to_be_removed);
  } 

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_PARAMETER_REMOVER_H
