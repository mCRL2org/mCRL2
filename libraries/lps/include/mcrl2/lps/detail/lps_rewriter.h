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

  /// \brief Function object for converting a data rewriter into a rewriter that
  /// applies a substitution.
  template <typename DataRewriter, typename Substitution>
  struct rewriter_adapter
  {
    const DataRewriter& R_;
    const Substitution& sigma_;
    
    rewriter_adapter(const DataRewriter& R, const Substitution& sigma)
      : R_(R), sigma_(sigma)
    {}
    
    data::data_expression operator()(const data::data_expression& t) const
    {
      return R_(t, sigma_);
    }
  };
  
  /// \brief Function object for applying a data rewriter to LPS data types.
  template <typename DataRewriter>
  struct lps_rewriter
  {
    const DataRewriter& R;
  
    lps_rewriter(const DataRewriter& R_)
      : R(R_)
    {}

    template <typename TermList>
    TermList rewrite_list(const TermList& l) const
    {
      // TODO: how to make this function efficient?
      typedef typename TermList::value_type value_type;
      atermpp::vector<value_type> v(l.begin(), l.end());
      for (typename std::vector<value_type>::iterator i = v.begin(); i != v.end(); ++i)
      {
        rewrite(*i);
      }
      return TermList(v.begin(), v.end());
    }   

    template <typename Container>
    void rewrite_container(Container& c) const
    {
      for (typename Container::iterator i = c.begin(); i != c.end(); ++i)
      {
        rewrite(*i);
      }
    }   

    /// \brief Applies the rewriter to a data expression
    /// \param d A data expression
    void rewrite(data::data_expression& d) const    
    {                                         
      d = R(d);
    } 
  
    /// \brief Applies the rewriter to an assignment
    /// \param a An assignment
    void rewrite(data::assignment& a) const
    {
      a = data::assignment(a.lhs(), R(a.rhs())); 
    } 
  
    /// \brief Applies the rewriter to an action
    /// \param a An action
    void rewrite(action& a) const
    {
      data::data_expression_list l = rewrite_list(a.arguments());
      a = action(a.label(), l); 
    }
    
    /// \brief Applies the rewriter to a deadlock
    /// \param d A deadlock
    void rewrite(deadlock& d) const
    {
      if (d.has_time())
      {
        rewrite(d.time());
      }
    } 
  
    /// \brief Applies the rewriter to a multi-action
    /// \param a A multi-action
    void rewrite(multi_action& a) const
    {
      if (a.has_time())
      {
        rewrite(a.time());
      }
      a.actions() = rewrite_list(a.actions());
    } 
  
    /// \brief Applies the rewriter to a summand
    /// \param s A summand
    void rewrite(action_summand& s) const
    {
      rewrite(s.condition());
      rewrite(s.multi_action());
      s.assignments() = rewrite_list(s.assignments());
    }

    /// \brief Applies the rewriter to a summand
    /// \param s A summand
    void rewrite(deadlock_summand& s) const
    {
      rewrite(s.condition());
      rewrite(s.deadlock());
    }
    
    /// \brief Applies the rewriter to a process_initializer
    /// \param s A process_initializer
    void rewrite(process_initializer& i) const
    {
      data::assignment_list a = rewrite_list(i.assignments());
      i = process_initializer(i.free_variables(), a);
    }
  
    /// \brief Applies the rewriter to a linear_process
    /// \param s A linear_process
    void rewrite(linear_process& p) const
    {
      rewrite_container(p.action_summands());
      rewrite_container(p.deadlock_summands());
    }
  
    /// \brief Applies the rewriter to a linear process specification
    /// \param spec A linear process specification
    void rewrite(specification& spec) const
    {
      rewrite(spec.process());
      rewrite(spec.initial_process());
    }
    
    template <typename Term>
    void operator()(Term& t)
    {
      rewrite(t);
    }
  };

  /// \brief Utility function to create an lps_rewriter.
  template <typename DataRewriter>    
  lps_rewriter<DataRewriter> make_lps_rewriter(const DataRewriter& R)        
  {
    return lps_rewriter<DataRewriter>(R);
  } 

} // namespace detail

} // namespace lps                        

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_REWRITER_H
