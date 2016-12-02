// Author(s): Jan Friso Groote and Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file realzone.h
/// \brief Implements simplification of real conditions

#ifndef MCRL2_LPSREALZONE_REALZONE_H
#define MCRL2_LPSREALZONE_REALZONE_H


#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/stochastic_specification.h"

#include "mcrl2/data/linear_inequalities.h"
#include "dbm.h"

namespace mcrl2
{
namespace data
{

class summand_information;

mcrl2::lps::stochastic_specification simplify(mcrl2::lps::stochastic_specification s,
                                             const rewrite_strategy strat=jitty);
void normalize_specification(
          const mcrl2::lps::stochastic_specification& s,
          const variable_list& real_parameters,
          const rewriter& r,
          std::vector < summand_information >& summand_info);
void move_real_parameters_out_of_actions(mcrl2::lps::stochastic_specification& s,
                                                  const variable_list& real_parameters,
                                                  const rewriter& r);
mcrl2::lps::stochastic_specification realzone(mcrl2::lps::stochastic_specification s,
                                             const rewrite_strategy strat=jitty);

using namespace mcrl2::lps;
using namespace atermpp;
using namespace mcrl2::core;

class real_representing_variable
{
protected:
  mcrl2::data::variable m_variable;
  mcrl2::data::data_expression m_lowerbound;
  mcrl2::data::data_expression m_upperbound;
  detail::comparison_t m_comparison_operator;

public:
  real_representing_variable
          (const mcrl2::data::variable& v,
           const mcrl2::data::data_expression& lb,
           const mcrl2::data::data_expression& ub,
           const detail::comparison_t co)
  {
    m_variable=v;
    m_lowerbound=lb;
    m_upperbound=ub;
    m_comparison_operator=co;
  }

  real_representing_variable(const real_representing_variable& other)
  {
    m_variable=other.m_variable;
    m_lowerbound=other.m_lowerbound;
    m_upperbound=other.m_upperbound;
    m_comparison_operator=other.m_comparison_operator;
  }

  real_representing_variable& operator =(const real_representing_variable& other)
  {
    m_variable=other.m_variable;
    m_lowerbound=other.m_lowerbound;
    m_upperbound=other.m_upperbound;
    m_comparison_operator=other.m_comparison_operator;
    return *this;
  }

  const mcrl2::data::variable& get_variable() const
  {
    return m_variable;
  }

  const mcrl2::data::data_expression& get_lowerbound() const
  {
    return m_lowerbound;
  }

  const mcrl2::data::data_expression& get_upperbound() const
  {
    return m_upperbound;
  }

  const mcrl2::data::detail::comparison_t& comparison_operator() const
  {
    return m_comparison_operator;
  }
};

typedef std::vector< real_representing_variable > context_type;

class summand_information
{

protected:
  mcrl2::lps::summand_base m_smd;
  bool m_is_delta_summand;
  assignment_list m_assignments;
  lps::stochastic_distribution m_distribution;
  lps::multi_action m_multi_action;
  lps::deadlock m_deadlock;

  variable_list real_summation_variables;
  variable_list non_real_summation_variables;
  std::vector < linear_inequality > summand_real_conditions;
  mutable_map_substitution< std::map<variable, data_expression> > summand_real_nextstate_map;


  function_symbol comparison_to_function(linear_inequality l)
  {
    detail::comparison_t comp = l.comparison();
    switch(comp)
    {
      case detail::less: return less(sort_real::real_());
      case detail::less_eq:return less_equal(sort_real::real_());
      case detail::equal:return equal_to(sort_real::real_());
    }
    assert(0);
    return equal_to(sort_real::real_()); //supress compiler warning
  }

  data_expression var_factor_to_data_expression(detail::variable_with_a_rational_factor v)
  {
    if(v.factor() == real_one())
    {
      return v.variable_name();
    }
    else if(v.factor() == real_minus_one())
    {
      return sort_real::negate(v.variable_name());
    }
    else
    {
      return sort_real::times(v.variable_name(),v.factor());
    }
  }

  data_expression linear_inequality_to_data_expression(linear_inequality l)
  {
    if(l.lhs_begin() == l.lhs_end())
    {
      return sort_bool::true_();
    }
    data_expression lhs = var_factor_to_data_expression(*(l.lhs_begin()));
    detail::lhs_t::const_iterator i = l.lhs_begin();
    for(i++; i != l.lhs_end(); i++)
    {
      lhs = sort_real::plus(lhs, var_factor_to_data_expression(*i));
    }
    data_expression result = comparison_to_function(l)(lhs,l.rhs());
    return result;
  }

  data_expression merge_conditions(data_expression nrc, std::vector< linear_inequality > rc)
  {
    data_expression merged = nrc;//equal_to(sort_bound::cbound(sort_real::creal(sort_int::cneg(sort_pos::c1()), sort_pos::c1()), sort_inequality::lt()), sort_bound::cbound(real_minus_one(), sort_inequality::lt()));//nrc;
    for(linear_inequality& k: rc) {
      merged = lazy::and_(merged,linear_inequality_to_data_expression(k));
    }
    return merged;
  }

  // The list of real assignments is updated according to summand_real_nextstate_map
  assignment_list update_assignments(assignment_list assign, sort_specification s)
  {
    assignment_list result;
    for(assignment_list::const_iterator i = assign.begin(); i != assign.end(); i++)
    {
      if(summand_real_nextstate_map.find(i->lhs()) == summand_real_nextstate_map.end())
      {
        result.push_front(assignment(i->lhs(), normalize_sorts(i->rhs(), s)));
      }
      else
      {
        result.push_front(assignment(i->lhs(), normalize_sorts(summand_real_nextstate_map(i->lhs()), s)));
      }
    }
    return reverse(result);
  }

public:
  summand_information(
          const mcrl2::lps::summand_base& s,
          bool  is_delta_summand,
          const assignment_list& assignments,
          const lps::stochastic_distribution& distribution,
          const lps::multi_action& multi_action,
          const lps::deadlock& deadlock,
          const variable_list& rsv, // real summantion variables
          const variable_list& nrsv,// non real summation variables
          const std::vector < linear_inequality >& src, // summand real condition
          const mutable_map_substitution< std::map<mcrl2::data::variable, mcrl2::data::data_expression> >& srnm //summand real next state map
  ):
          m_smd(s),
          m_is_delta_summand(is_delta_summand),
          m_assignments(assignments),
          m_distribution(distribution),
          m_multi_action(multi_action),
          m_deadlock(deadlock),
          real_summation_variables(rsv),
          non_real_summation_variables(nrsv),
          summand_real_conditions(src),
          summand_real_nextstate_map(srnm)
  {
  }

  summand_information(const summand_information& s)
  {
    m_smd=s.m_smd;
    m_is_delta_summand=s.m_is_delta_summand;
    m_assignments=s.m_assignments;
    m_distribution=s.m_distribution;
    m_multi_action=s.m_multi_action;
    m_deadlock=s.m_deadlock;
    real_summation_variables=s.real_summation_variables;
    non_real_summation_variables=s.non_real_summation_variables;
    summand_real_conditions=s.summand_real_conditions;
    summand_real_nextstate_map=s.summand_real_nextstate_map;
  }

  ~summand_information()
  {
  }

  const mcrl2::lps::summand_base& get_summand() const
  {
    return m_smd;
  }

  mcrl2::lps::summand_base& get_summand()
  {
    return m_smd;
  }

  mcrl2::lps::deadlock_summand get_represented_deadlock_summand(sort_specification s)
  {
    assert(m_is_delta_summand);
    variable_list sumvars = non_real_summation_variables + real_summation_variables;
    return deadlock_summand(sumvars,
                               normalize_sorts(merge_conditions(m_smd.condition(),summand_real_conditions), s),
                               deadlock(get_deadlock().time()));
  }

  mcrl2::lps::action_summand get_represented_action_summand(sort_specification s)
  {
    assert(!m_is_delta_summand);
    variable_list sumvars = non_real_summation_variables + real_summation_variables;
    return stochastic_action_summand(sumvars,
                                      normalize_sorts(merge_conditions(m_smd.condition(),summand_real_conditions), s),
                                      get_multi_action(),
                                      update_assignments(m_assignments, s),
                                      get_distribution());
  }

  // void update_assignments()
  // {
  //   m_assignments = update_assignments(m_assignments);
  //   summand_real_nextstate_map.clear();
  // }

  const bool& is_delta_summand() const
  {
    return m_is_delta_summand;
  }

  const mcrl2::data::assignment_list& get_assignments() const
  {
    return m_assignments;
  }

  const mcrl2::lps::stochastic_distribution& get_distribution() const
  {
    return m_distribution;
  }

  const mcrl2::lps::multi_action& get_multi_action() const
  {
    return m_multi_action;
  }

  const mcrl2::lps::deadlock& get_deadlock() const
  {
    return m_deadlock;
  }

  variable_list get_real_summation_variables() const
  {
    return real_summation_variables;
  }

  variable_list get_non_real_summation_variables() const
  {
    return non_real_summation_variables;
  }

  const std::vector < linear_inequality >& get_summand_real_conditions() const
  {
    return summand_real_conditions;
  }

  std::vector < linear_inequality >::const_iterator get_summand_real_conditions_begin() const
  {
    return summand_real_conditions.begin();
  }

  std::vector < linear_inequality >::const_iterator get_summand_real_conditions_end() const
  {
    return summand_real_conditions.end();
  }

  mutable_map_substitution< std::map<mcrl2::data::variable, mcrl2::data::data_expression> >& get_summand_real_nextstate_map()
  {
    return summand_real_nextstate_map;
  }
};

} // namespace data
} // namsepace mcrl2


#endif // MCRL2_LPSREALZONE_REALZONE_H
