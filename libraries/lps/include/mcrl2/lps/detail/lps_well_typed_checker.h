// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_well_typed_checker.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_WELL_TYPED_CHECKER_H
#define MCRL2_LPS_DETAIL_LPS_WELL_TYPED_CHECKER_H

#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  struct lps_well_typed_checker
  {
    /// \brief Checks if the sort of t has type real
    bool check_time(const data::data_expression& t, const std::string& type) const
    {
      if (!data::sort_real::is_real(t.sort()))
      {
        std::cerr << "is_well_typed(" << type << ") failed: time " << core::pp(t) << " doesn't have sort real." << std::endl;
        return false;
      }
      return true;
    }

    /// \brief Checks if the sort of t has type bool
    bool check_condition(const data::data_expression& t, const std::string& type) const
    {
      if (!data::sort_bool::is_bool(t.sort()))
      {
        std::cerr << "is_well_typed(" << type << ") failed: condition " << core::pp(t) << " doesn't have sort bool." << std::endl;
        return false;
      }
      return true;
    }

    /// \brief Checks if the assignments are well typed and have unique left hand sides
    bool check_assignments(const data::assignment_list& l, const std::string& type) const
    {
      if (!is_well_typed_container(l))
      {
        std::cerr << "is_well_typed(" << type << ") failed: the assignments " << core::pp(l) << " are not well typed." << std::endl;
        return false;
      }
      if (mcrl2::data::detail::sequence_contains_duplicates(
               boost::make_transform_iterator(l.begin(), data::detail::assignment_lhs()),
               boost::make_transform_iterator(l.end()  , data::detail::assignment_lhs())
              )
         )
      {
        std::cerr << "is_well_typed(" << type << ") failed: data assignments " << pp(l) << " don't have unique left hand sides." << std::endl;
        return false;
      }
      return true;
    }

    /// \brief Checks well typedness of the elements of a container
    template <typename Container>
    bool is_well_typed_container(const Container& c) const
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        if (!is_well_typed(*i))
        {
          return false;
        }
      }
      return true;
    }   

    /// \brief Checks well typedness of a sort expression
    /// \param d A sort expression
    bool is_well_typed(const data::sort_expression& d) const
    {                                         
      return true;
    } 
  
    /// \brief Checks well typedness of a variable
    /// \param d A variable
    bool is_well_typed(const data::variable& d) const
    {                                         
      return true;
    } 
  
    /// \brief Checks well typedness of a data expression
    /// \param d A data expression
    bool is_well_typed(const data::data_expression& d) const
    {                                         
      return true;
    } 
  
    /// \brief Traverses an assignment
    /// \param a An assignment
    bool is_well_typed(const data::assignment& a) const
    {
      if (a.lhs().sort() != a.rhs().sort())
      {
        std::clog << "is_well_typed(data_assignment) failed: the left and right hand sides "
           << mcrl2::core::pp(a.lhs()) << " and " << mcrl2::core::pp(a.rhs()) << " have different sorts." << std::endl;
        return false;
      }
      return true;
    } 

    /// \brief Traverses an action label
    bool is_well_typed(const action_label& l) const
    {
      return true;
    }

    /// \brief Traverses an action
    /// \param a An action
    bool is_well_typed(const action& a) const
    {
      return true;
    }

    /// \brief Checks well typedness of a deadlock
    /// \param d A deadlock
    /// \return Returns true if
    /// <ul>
    /// <li>the (optional) time has sort Real</li>
    /// </ul>
    bool is_well_typed(const deadlock& d) const
    {
      if (d.has_time())
      {
        check_time(d.time(), "deadlock");
      }
      return true;
    } 

    /// \brief Checks well typedness of a multi-action
    /// \param a A multi-action
    /// \return Returns true if
    /// <ul>
    /// <li>the (optional) time has sort Real</li>
    /// </ul>
    bool is_well_typed(const multi_action& a) const
    {
      if (a.has_time())
      {
        check_time(a.time(), "deadlock");
      }
      return true;
    } 

    /// \brief Checks well typedness of a summand
    /// \param s An action summand
    bool is_well_typed(const action_summand& s) const
    {
      if (!data::detail::unique_names(s.summation_variables()))
      {
        std::cerr << "is_well_typed(action_summand) failed: summation variables " << pp(s.summation_variables()) << " don't have unique names." << std::endl;
        return false;
      }
      if (!check_condition(s.condition(), "action_summand"))     { return false; }
      if (!is_well_typed(s.multi_action()))                      { return false; }
      if (!check_assignments(s.assignments(), "action_summand")) { return false; }
      return true;
    }

    /// \brief Checks well typedness of a summand
    /// \param s A summand
    bool is_well_typed(const deadlock_summand& s) const
    {
      if (!check_condition(s.condition(), "deadlock_summand")) { return false; }
      if (!is_well_typed(s.deadlock())) { return false; }
      return true;
    }
    
    /// \brief Checks well typedness of a process_initializer
    /// \param s A process_initializer
    /// <ul>
    /// <li>the left hand sides of the data assignments are unique</li>
    /// </ul>
    bool is_well_typed(const process_initializer& i) const
    {
      if (!check_assignments(i.assignments(), "process_initializer")) { return false; }
      return true;
    }

    /// \brief Checks well typedness of a linear process
    /// \param p A linear_process
    /// \return True if
    /// <ul>
    /// <li>the free variables occurring in the process are declared in global_variables()</li>
    /// <li>the process parameters have unique names</li>
    /// <li>the free variables have unique names</li>
    /// <li>process parameters and summation variables have different names</li>
    /// <li>the left hand sides of the assignments of summands are contained in the process parameters</li>
    /// <li>the summands are well typed</li>
    /// </ul>
    bool is_well_typed(const linear_process& p) const
    {
      // check 1)
      std::set<data::variable> declared_free_variables  = mcrl2::data::detail::make_set(p.global_variables());
      std::set<data::variable> occurring_free_variables = lps::find_free_variables(p);
      if (!(std::includes(declared_free_variables.begin(),
                          declared_free_variables.end(),
                          occurring_free_variables.begin(),
                          occurring_free_variables.end()
                         )
          ))
      {
        std::cerr << "is_well_typed(linear_process) failed: some of the free variables were not declared\n";
        std::cerr << "declared free variables: " << data::pp(declared_free_variables) << std::endl;
        std::cerr << "occurring free variables: " << data::pp(occurring_free_variables) << std::endl;
        return false;
      }

      // check 2)
      if (!mcrl2::data::detail::unique_names(p.process_parameters()))
      {
        std::cerr << "is_well_typed(linear_process) failed: process parameters " << data::pp(p.process_parameters()) << " don't have unique names." << std::endl;
        return false;
      }

      // check 3)
      if (!mcrl2::data::detail::unique_names(p.global_variables()))
      {
        std::cerr << "is_well_typed(linear_process) failed: free variables " << data::pp(p.process_parameters()) << " don't have unique names." << std::endl;
        return false;
      }

      // check 4)
      std::set<core::identifier_string> names;
      for (data::variable_list::const_iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
      {
        names.insert(i->name());
      }
      for (action_summand_vector::const_iterator i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
      {
        if (!mcrl2::data::detail::check_variable_names(i->summation_variables(), names))
        {
          std::cerr << "is_well_typed(linear_process) failed: some of the names of the summation variables " << data::pp(i->summation_variables()) << " also appear as process parameters." << std::endl;
          return false;
        }
      }

      // check 5)
      for (action_summand_vector::const_iterator i = p.action_summands().begin(); i != p.action_summands().end(); ++i)
      {
        if (!mcrl2::data::detail::check_assignment_variables(i->assignments(), p.process_parameters()))
        {
          std::cerr << "is_well_typed(linear_process) failed: some left hand sides of the assignments " << data::pp(i->assignments()) << " do not appear as process parameters." << std::endl;
          return false;
        }
      }

      // check 6)
      if (!is_well_typed_container(p.action_summands()))   { return false; }
      if (!is_well_typed_container(p.deadlock_summands())) { return false; }
      return true;
    }
                        
    /// \brief Checks well typedness of a linear process specification
    /// \param spec A linear process specification
    /// \return True if
    /// <ul>
    /// <li>the sorts occurring in the summation variables are declared in the data specification</li>
    /// <li>the sorts occurring in the process parameters are declared in the data specification </li>
    /// <li>the sorts occurring in the free variables are declared in the data specification     </li>
    /// <li>the sorts occurring in the action labels are declared in the data specification      </li>
    /// <li>the action labels occurring in the process are contained in action_labels()          </li>
    /// <li>the process is well typed                                                            </li>
    /// <li>the data specification is well typed                                                 </li>
    /// <li>the initial process is well typed                                                    </li>
    /// </ul>
    bool is_well_typed(const specification& spec) const
    {
      std::set<data::sort_expression> declared_sorts = mcrl2::data::detail::make_set(spec.data().sorts());
      std::set<action_label> declared_labels = mcrl2::data::detail::make_set(spec.action_labels());
      const action_summand_vector& action_summands = spec.process().action_summands();

      // check 1)
      for (action_summand_vector::const_iterator i = action_summands.begin(); i != action_summands.end(); ++i)
      {
        if (!(mcrl2::data::detail::check_variable_sorts(i->summation_variables(), declared_sorts)))
        {
          std::cerr << "is_well_typed(specification) failed: some of the sorts of the summation variables " << data::pp(i->summation_variables()) << " are not declared in the data specification " << data::pp(spec.data().sorts()) << std::endl;
          return false;
        }
      }

      // check 2)
      if (!(mcrl2::data::detail::check_variable_sorts(spec.process().process_parameters(), declared_sorts)))
      {
        std::cerr << "is_well_typed(specification) failed: some of the sorts of the process parameters " << data::pp(spec.process().process_parameters()) << " are not declared in the data specification " << data::pp(spec.data().sorts()) << std::endl;
        return false;
      }

      // check 3)
      if (!(mcrl2::data::detail::check_variable_sorts(spec.process().global_variables(), declared_sorts)))
      {
        std::cerr << "is_well_typed(specification) failed: some of the sorts of the free variables " << data::pp(spec.process().global_variables()) << " are not declared in the data specification " << data::pp(spec.data().sorts()) << std::endl;
        return false;
      }

      // check 4)
      if (!(detail::check_action_label_sorts(spec.action_labels(), declared_sorts)))
      {
        std::cerr << "is_well_typed(specification) failed: some of the sorts occurring in the action labels " << mcrl2::core::pp(spec.action_labels()) << " are not declared in the data specification " << data::pp(spec.data().sorts()) << std::endl;
        return false;
      }

      // check 5)
      for (action_summand_vector::const_iterator i = action_summands.begin(); i != action_summands.end(); ++i)
      {
        if (!(detail::check_action_labels(i->multi_action().actions(), declared_labels)))
        {
          std::cerr << "is_well_typed(specification) failed: some of the labels occurring in the actions " << mcrl2::core::pp(i->multi_action().actions()) << " are not declared in the action specification " << mcrl2::core::pp(spec.action_labels()) << std::endl;
          return false;
        }
      }
      if (!is_well_typed(spec.process()))         { return false; }
      if (!spec.data().is_well_typed())           { return false; }
      if (!is_well_typed(spec.initial_process())) { return false; }

      return true;
    }


    /// \brief Checks well typedness of a summand
    /// \deprecated
    /// \return Returns true if
    /// <ul>
    /// <li>the data assignments are well typed</li>
    /// <li>the (optional) time has sort Real</li>
    /// <li>the condition has sort Bool</li>
    /// <li>the summation variables have unique names</li>
    /// </ul>
    bool is_well_typed(const summand& s) const
    {
      // check 1)
      if (!check_assignments(s.assignments(), "summand")) { return false; }
      if (!check_time(s.time(), "summand"))               { return false; }
      if (!check_condition(s.condition(), "summand"))     { return false; }
      if (!data::detail::unique_names(s.summation_variables()))
      {
        std::cerr << "is_well_typed(summand) failed: summation variables " << pp(s.summation_variables()) << " don't have unique names." << std::endl;
        return false;
      }
      return true;
    }

    template <typename Term>
    bool operator()(const Term& t) const
    {
      return is_well_typed(t);
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_WELL_TYPED_CHECKER_H
