// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_rewriter.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_REWRITER_H
#define MCRL2_LPS_DETAIL_LPS_REWRITER_H

#include <vector>
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a rewriter to LPS data types.
  template <typename DataRewriter, typename Substitution>
  struct lps_rewriter
  {
    const DataRewriter& R;
    const Substitution& sigma;
  
    lps_rewriter(const DataRewriter& R_, const Substitution& sigma_)
      : R(R_),
        sigma(sigma_)                       
    {}

    template <typename TermList>
    TermList rewrite_list(const TermList& l) const
    {
      // TODO: how to make this function efficient?
      typedef typename TermList::value_type value_type;
      std::vector<value_type> v(l.begin(), l.end());
      for (typename std::vector<value_type>::iterator i = v.begin(); i != v.end(); ++i)
      {
        (*this)(*i);
      }
      return TermList(v.begin(), v.end());
    }   

    /// \brief Applies the rewriter to a data expression
    /// \param d A data expression
    void operator()(data::data_expression& d) const    
    {                                         
      d = R(d, sigma);
    } 
  
    /// \brief Applies the rewriter to an assignment
    /// \param a An assignment
    void operator()(data::assignment& a) const
    {
      a = data::assignment(a.lhs(), R(a.rhs(), sigma)); 
    } 
  
    /// \brief Applies the rewriter to an action
    /// \param a An action
    void operator()(action& a) const
    {
      data::data_expression_list l = rewrite_list(a.arguments());
      a = action(a.label(), l); 
    }
    
    /// \brief Applies the rewriter to a deadlock
    /// \param d A deadlock
    void operator()(deadlock& d) const
    {
      if (d.has_time())
      {
        (*this)(d.time());
      }
    } 
  
    /// \brief Applies the rewriter to a multi-action
    /// \param a A multi-action
    void operator()(multi_action& a) const
    {
      if (a.has_time())
      {
        (*this)(a.time());
      }
      a.actions() = rewrite_list(a.actions());
    } 
  
    /// \brief Applies the rewriter to a summand
    /// \param s A summand
    void operator()(summand& s) const
    {
      data::data_expression c = s.condition();
      (*this)(c);
      if (s.is_delta())
      {
        deadlock delta = s.deadlock();
        (*this)(delta);
        s = summand(s.summation_variables(), c, delta);
      }
      else
      {
        multi_action m = s.multi_action();
        (*this)(m);
        data::assignment_list a = rewrite_list(s.assignments());
        s = summand(s.summation_variables(), c, m, a);
      }
    }
  
    /// \brief Applies the rewriter to a process_initializer
    /// \param s A process_initializer
    void operator()(process_initializer& i) const
    {
      data::assignment_list a = rewrite_list(i.assignments());
      i = process_initializer(i.free_variables(), a);
    }
  
    /// \brief Applies the rewriter to a linear_process
    /// \param s A linear_process
    void operator()(linear_process& p) const
    {
      summand_list l = rewrite_list(p.summands());
      p = linear_process(p.free_variables(), p.process_parameters(), l);
    }
  
    /// \brief Applies the rewriter to a linear process specification
    /// \param spec A linear process specification
    void operator()(specification& spec) const
    {
      (*this)(spec.process());
      (*this)(spec.initial_process());
    } 
  };
  
  /// \brief Utility function to create an lps_rewriter.
  template <typename DataRewriter, typename Substitution>    
  lps_rewriter<DataRewriter, Substitution> make_lps_rewriter(const DataRewriter& R, const Substitution& sigma)        
  {
    return lps_rewriter<DataRewriter, Substitution>(R, sigma);
  } 

} // namespace detail

} // namespace lps                        

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_REWRITER_H
