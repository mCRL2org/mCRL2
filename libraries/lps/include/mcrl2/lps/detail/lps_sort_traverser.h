// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
#define MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H

#include <vector>
#include "mcrl2/data/traverse.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template <typename OutIter>
  struct lps_sort_traverser
  {
    OutIter dest;
  
    lps_sort_traverser(OutIter dest_)
      : dest(dest_)
    {}

    /// \brief Traverses the elements of a container
    template <typename Container>
    void traverse_container(const Container& c)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        traverse(*i);
      }
    }   

    /// \brief Traverses a sort expression
    /// \param d A sort expression
    void traverse(const data::sort_expression& d)    
    {                                         
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses a variable
    /// \param d A variable
    void traverse(const data::variable& d)    
    {                                         
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses a data expression
    /// \param d A data expression
    void traverse(const data::data_expression& d)    
    {                                         
      dest = data::traverse_sort_expressions(d, dest);
    } 
  
    /// \brief Traverses an assignment
    /// \param a An assignment
    void traverse(const data::assignment& a)
    {
      dest = data::traverse_sort_expressions(a, dest);
    } 

    /// \brief Traverses an action label
    void traverse(const action_label& l)
    {
      traverse_container(l.sorts());
    }

    /// \brief Traverses an action
    /// \param a An action
    void traverse(const action& a)
    {
      traverse(a.label());
      traverse_container(a.arguments());
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void traverse(const deadlock& d)
    {
      if (d.has_time())
      {
        traverse(d.time());
      }
    } 

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void traverse(const multi_action& a)
    {
      if (a.has_time())
      {
        traverse(a.time());
      }
      traverse_container(a.actions());
    } 

    /// \brief Traverses a summand
    /// \param s A summand
    void traverse(const action_summand& s)
    {
      traverse(s.condition());
      traverse(s.multi_action());
      traverse_container(s.assignments());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void traverse(const deadlock_summand& s)
    {
      traverse(s.condition());
      traverse(s.deadlock());
    }
    
    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void traverse(const process_initializer& i)
    {
      traverse_container(i.assignments());
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void traverse(const linear_process& p)
    {
      traverse_container(p.process_parameters());
      traverse_container(p.action_summands());
      traverse_container(p.deadlock_summands());
    }
                        
    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void traverse(const specification& spec)
    {
      traverse(spec.process());
      traverse_container(spec.global_variables());
      traverse(spec.initial_process());
      traverse_container(spec.action_labels());
    }

    template <typename Term>
    void operator()(const Term& t)
    {
      traverse(t);
    }
  };

  /// \brief Utility function to create an lps_sort_traverser.
  template <typename OutIter>    
  lps_sort_traverser<OutIter> make_lps_sort_traverser(OutIter dest)        
  {
    return lps_sort_traverser<OutIter>(dest);
  } 

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
