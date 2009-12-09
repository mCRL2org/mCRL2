// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_parameter_remover.h
/// \brief add your file description here.

// This is to deal with circular header file dependencies
#ifndef MCRL2_LPS_SPECIFICATION_H
#include "mcrl2/lps/specification.h"
#endif

#ifndef MCRL2_LPS_DETAIL_LPS_PARAMETER_REMOVER_H
#define MCRL2_LPS_DETAIL_LPS_PARAMETER_REMOVER_H

#include <algorithm>
#include <vector>
#include "mcrl2/data/detail/assignment_functional.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for removing parameters from LPS data types.
  /// These parameters can be either process parameters or free variables.
  /// Assignments to these parameters are removed as well.
  template <typename SetContainer>
  struct lps_parameter_remover
  {
    const SetContainer& to_be_removed;
  
    lps_parameter_remover(const SetContainer& to_be_removed_)
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

    /// \brief Removes parameters from a summand
    /// \param s A summand
    void remove(action_summand& s) const
    {
      remove_list(s.assignments());
    }

    /// \brief Removes parameters from a process_initializer
    /// \param s A process_initializer
    void remove(process_initializer& i) const
    {
      i = process_initializer(remove_list_copy(i.assignments()));
    }
  
    /// \brief Removes parameters from a linear_process
    /// \param s A linear_process
    void remove(linear_process& p) const
    {
      remove_list(p.process_parameters());
      remove_container(p.action_summands());
    }
  
    /// \brief Removes parameters from a linear process specification
    /// \param spec A linear process specification
    void remove(specification& spec) const
    {
      remove(spec.process());
      remove(spec.initial_process());
      remove_set_container(spec.global_variables());
    }
    
    template <typename Term>
    void operator()(Term& t)
    {
      remove(t);
    }
  };

  /// \brief Utility function to create an lps_parameter_remover.
  template <typename SetContainer>    
  lps_parameter_remover<SetContainer> make_lps_parameter_remover(const SetContainer& to_be_removed)        
  {
    return lps_parameter_remover<SetContainer>(to_be_removed);
  } 

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_PARAMETER_REMOVER_H
