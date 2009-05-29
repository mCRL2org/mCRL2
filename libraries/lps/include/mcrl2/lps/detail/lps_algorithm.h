// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
#define MCRL2_LPS_DETAIL_LPS_ALGORITHM_H

#include <algorithm>
#include <set>
#include <vector>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/lps/specification.h"  
#include "mcrl2/lps/detail/lps_rewriter.h"  
#include "mcrl2/lps/detail/remove_parameters.h"  

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object that checks if a summand has a false condition
  struct is_trivial_summand
  {
    bool operator()(const summand_base& s) const
    {
      return s.condition() == data::sort_bool_::false_();
    }
  };
  
  /// \brief Function object that checks if a sort is a singleton sort.
  /// Note that it is an approximation, meaning that in some cases it
  /// may return false whereas in reality the answer is true.
  struct is_singleton_sort
  {
    const data::data_specification& m_data_spec;

    is_singleton_sort(const data::data_specification& data_spec)
      : m_data_spec(data_spec)
    {}

    bool operator()(const data::sort_expression& s) const
    {
      data::data_specification::constructors_const_range c = m_data_spec.constructors(s);
      if (boost::distance(c) != 1)
      {
        return false;
      }
      data::function_symbol f = *c.begin();
      return !f.sort().is_function_sort();
    }
  };
  
  /// \brief Algorithm class for algorithms on linear process specifications.
  class lps_algorithm
  {
    protected:
      /// \brief The specification that is processed by the algorithm
      specification& m_spec;
     
      /// \brief If true, verbose output is written
      bool m_verbose;

    public:
      /// \brief Constructor
      lps_algorithm(specification& spec, bool verbose = false)
        : m_spec(spec),
          m_verbose(verbose)
      {}

      /// \brief Applies the next state substitution to the variable v.
      data::data_expression next_state(const summand& s, const data::variable& v) const
      {
        assert(!s.is_delta());
        const data::assignment_list& a = s.assignments();
        for (data::assignment_list::const_iterator i = a.begin(); i != a.end(); ++i)
        {
          if (i->lhs() == v)
          {
            return i->rhs();
          }
        }
        return v; // no assignment to v found, so return v itself
      }

      /// \brief Applies the next state substitution to the variable v.
      data::data_expression next_state(const action_summand& s, const data::variable& v) const
      {
        const data::assignment_list& a = s.assignments();
        for (data::assignment_list::const_iterator i = a.begin(); i != a.end(); ++i)
        {
          if (i->lhs() == v)
          {
            return i->rhs();
          }
        }
        return v; // no assignment to v found, so return v itself
      }

      /// \brief Removes formal parameters from the specification
      void remove_parameters(const std::set<data::variable>& to_be_removed)
      {
        lps::detail::remove_parameters(m_spec, to_be_removed);       
      }
      
      /// \brief Removes parameters with a singleton sort
      void remove_singleton_sorts()
      {
        data::mutable_substitution<> sigma;
        const data::variable_list& p = m_spec.process().process_parameters();
        for (data::variable_list::const_iterator i = p.begin(); i != p.end(); ++i)
        {
          if (is_singleton_sort(m_spec.data())(i->sort()))
          {
            sigma[*i] = *m_spec.data().constructors(i->sort()).begin();
          }
        }
        // lps::detail::lps_substituter(sigma)(m_spec);
        // remove_parameters(m_spec.process().process_parameters(), ...);
      }
      
      /// \brief Removes summands with condition equal to false
      void remove_trivial_summands()
      {
        action_summand_vector& v = m_spec.process().action_summands();
        v.erase(std::remove_if(v.begin(), v.end(), is_trivial_summand()), v.end());

        deadlock_summand_vector& w = m_spec.process().deadlock_summands();
        w.erase(std::remove_if(w.begin(), w.end(), is_trivial_summand()), w.end());
      }
  };

  /// \brief Algorithm class for algorithms on linear process specifications
  /// that use a rewriter.
  template <typename DataRewriter>
  class lps_rewriter_algorithm: public lps_algorithm
  {
    protected:
      const DataRewriter& R;

    public:
      lps_rewriter_algorithm(specification& spec, const DataRewriter& rewr, bool verbose = false)
        : lps_algorithm(spec, verbose),
          R(rewr)
      {}

      /// \brief Rewrites the specification
      /// \brief sigma A substitution
      void rewrite()
      {
        lps::detail::make_lps_rewriter(R).rewrite(m_spec);
      }

      /// \brief Rewrites the specification with a substitution.
      /// \brief sigma A substitution
      template <typename Substitution>
      void rewrite(Substitution& sigma)
      {
        lps::detail::rewriter_adapter<DataRewriter, Substitution> Rsigma(R, sigma);
        lps::detail::make_lps_rewriter(Rsigma).rewrite(m_spec);
      }
  };
  
} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
