// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_replacer.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_REPLACER_H
#define MCRL2_LPS_DETAIL_LPS_REPLACER_H

#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template <typename Substitution>
  struct lps_replacer
  {
    const Substitution& sigma;
    
    /// If true, the process parameters, global variables and the left hand
    /// sides of assignments are replaced as well. For this it is needed
    /// that the result of the substitution is a variable.
    bool replace_parameters;
  
    lps_replacer(const Substitution& sigma_, bool replace_parameters_ = false)
      : sigma(sigma_),
        replace_parameters(replace_parameters_)
    {}

    /// \brief Applies the substitution to the elements of a term list
    template <typename TermList>
    TermList substitute_list_copy(const TermList& l) const
    {
      // TODO: how to make this function efficient?
      typedef typename TermList::value_type value_type;
      atermpp::vector<value_type> v(l.begin(), l.end());
      for (typename std::vector<value_type>::iterator i = v.begin(); i != v.end(); ++i)
      {
        substitute(*i);
      }
      return TermList(v.begin(), v.end());
    }   

    /// \brief Applies the substitution to the elements of a term list
    template <typename TermList>
    void substitute_list(TermList& l) const
    {
      l = substitute_list_copy(l);
    }

    /// \brief Applies the substitution to the elements of a container
    template <typename Container>
    void substitute_container(Container& c) const
    {
      for (typename Container::iterator i = c.begin(); i != c.end(); ++i)
      {
        substitute(*i);
      }
    }   

    /// \brief Applies the substitution to a data variable
    /// \param d A data variable
    data::variable substitute_copy(const data::variable& d) const
    {
      return sigma(d);
    } 

    /// \brief Applies the substitution to a data variable
    /// \param d A data variable
    void substitute(data::variable& d) const
    {                                         
      d = substitute_copy(d);
    } 
  
    /// \brief Applies the substitution to a data expression
    /// \param d A data expression
    data::data_expression substitute_copy(const data::data_expression& d) const    
    {                                         
      return data::replace_free_variables(d, sigma);
    } 

    /// \brief Applies the substitution to a data expression
    /// \param d A data expression
    void substitute(data::data_expression& d) const    
    {                                         
      d = substitute_copy(d);
    } 
  
    /// \brief Applies the substitution to an assignment
    /// \param a An assignment
    void substitute(data::assignment& a) const
    {
      if (replace_parameters)
      {
        data::variable v = substitute_copy(a.lhs());
        a = data::assignment(v, substitute_copy(a.rhs()));
      }
      else
      {
        a = data::assignment(a.lhs(), substitute_copy(a.rhs()));
      }
    } 
  
    /// \brief Applies the substitution to an action
    /// \param a An action
    void substitute(action& a) const
    {
      a = action(a.label(), substitute_list_copy(a.arguments())); 
    }
    
    /// \brief Applies the substitution to a deadlock
    /// \param d A deadlock
    void substitute(deadlock& d) const
    {
      if (d.has_time())
      {
        substitute(d.time());
      }
    } 
  
    /// \brief Applies the substitution to a multi-action
    /// \param a A multi-action
    void substitute(multi_action& a) const
    {
      if (a.has_time())
      {
        substitute(a.time());
      }
      substitute_list(a.actions());
    } 
  
    /// \brief Applies the substitution to a summand
    /// \param s A summand
    void substitute(action_summand& s) const
    {
      substitute(s.condition());
      substitute(s.multi_action());
      substitute_list(s.assignments());
    }

    /// \brief Applies the substitution to a summand
    /// \param s A summand
    void substitute(deadlock_summand& s) const
    {
      substitute(s.condition());
      substitute(s.deadlock());
    }
    
    /// \brief Applies the substitution to a process_initializer
    /// \param s A process_initializer
    void substitute(process_initializer& i) const
    {
      i = process_initializer(substitute_list_copy(i.assignments()));
    }
  
    /// \brief Applies the substitution to a linear_process
    /// \param s A linear_process
    void substitute(linear_process& p) const
    {
      if (replace_parameters)
      {
        substitute_list(p.process_parameters());
      }
      substitute_container(p.action_summands());
      substitute_container(p.deadlock_summands());
    }
  
    /// \brief Applies the substitution to a linear process specification
    /// \param spec A linear process specification
    void substitute(specification& spec) const
    {
      if (replace_parameters)
      {
        atermpp::set<data::variable> glob;
        for (atermpp::set<data::variable>::iterator i = spec.global_variables().begin(); i != spec.global_variables().end(); ++i)
        {
          glob.insert(substitute_copy(*i));
        }
        spec.global_variables() = glob;
      }
      substitute(spec.process());
      substitute(spec.initial_process());
    }
    
    template <typename Term>
    void operator()(Term& t) const
    {
      substitute(t);
    }
  };

  /// \brief Utility function to create an lps_replacer.
  template <typename Substitution>    
  lps_replacer<Substitution> make_lps_replacer(const Substitution& sigma)        
  {
    return lps_replacer<Substitution>(sigma);
  } 

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_REPLACER_H
