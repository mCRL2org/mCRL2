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
#include <iterator>
#include <set>
#include <vector>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/find.h"  
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/lps/specification.h"  
#include "mcrl2/lps/find.h"  
#include "mcrl2/lps/rewrite.h"  
#include "mcrl2/lps/substitute.h"  
#include "mcrl2/lps/remove.h"  

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Algorithm class for algorithms on linear process specifications.
  class lps_algorithm
  {
    protected:
      /// \brief The specification that is processed by the algorithm
      specification& m_spec;
     
      /// \brief If true, verbose output is written
      bool m_verbose;

      template <typename OutIter>
      void sumelm_find_variables(const action_summand& s, OutIter result) const
      {
        data::find_all_free_variables(s.condition(), result);
        lps::find_all_free_variables(s.multi_action(), result);
        data::find_all_free_variables(s.assignments(), result);
      }
    
      template <typename OutIter>
      void sumelm_find_variables(const deadlock_summand& s, OutIter result) const
      {
        data::find_all_free_variables(s.condition(), result);
        lps::find_all_free_variables(s.deadlock(), result);
      }
    
      template <typename SummandType>
      void remove_unused_summand_variables(SummandType& summand_)
      {
        data::variable_vector new_summation_variables;
    
        std::set<data::variable> occurring_vars;
        sumelm_find_variables(summand_, std::inserter(occurring_vars, occurring_vars.end()));
    
        std::set<data::variable> summation_variables(data::convert<std::set<data::variable> >(summand_.summation_variables()));
        std::set_intersection(summation_variables.begin(), summation_variables.end(),
                            occurring_vars.begin(), occurring_vars.end(),
                            std::inserter(new_summation_variables, new_summation_variables.end()));
    
        summand_.summation_variables() = data::convert<data::variable_list>(new_summation_variables);
      }

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

      /// \brief Attempts to eliminate the free variables of the specification, by substituting
      /// a constant value for them. If no constant value is found for one of the variables,
      /// an exception is thrown.
      void instantiate_free_variables()
      {
        data::mutable_map_substitution<> sigma;
        data::representative_generator default_expression_generator(m_spec.data());
        std::set<data::variable> to_be_removed;
        const data::variable_list& v = m_spec.process().free_variables();
        for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          data::data_expression d = default_expression_generator(i->sort());
          if (d == data::data_expression())
          {
            throw mcrl2::runtime_error("Error in specification::instantiate_free_variables: could not instantiate " + pp(*i));
          }
          sigma[*i] = d;
          to_be_removed.insert(*i);
        }
        lps::substitute(m_spec, sigma);
        lps::remove_parameters(m_spec, to_be_removed);
        assert(m_spec.process().free_variables().empty());
        assert(m_spec.initial_process().free_variables().empty());
      }

      /// \brief Removes formal parameters from the specification
      void remove_parameters(const std::set<data::variable>& to_be_removed)
      {
        lps::remove_parameters(m_spec, to_be_removed);       
      }
      
      /// \brief Removes parameters with a singleton sort
      void remove_singleton_sorts()
      {
        lps::remove_singleton_sorts(m_spec);
      }
      
      /// \brief Removes summands with condition equal to false
      void remove_trivial_summands()
      {
        lps::remove_trivial_summands(m_spec);
      }

      /// \brief Removes unused summand variables.
      void remove_unused_summand_variables()
      {
        action_summand_vector& v = m_spec.process().action_summands();
        std::for_each(v.begin(), v.end(), boost::bind(&lps_algorithm::remove_unused_summand_variables<action_summand>, this, _1));

        deadlock_summand_vector& w = m_spec.process().deadlock_summands();
        std::for_each(w.begin(), w.end(), boost::bind(&lps_algorithm::remove_unused_summand_variables<deadlock_summand>, this, _1));
      }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_ALGORITHM_H
