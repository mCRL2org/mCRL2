// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file forward_exploration.h


#ifndef MCRL2_LPSSYMBOLICBISIM_FORWARD_EXPLORATION_H
#define MCRL2_LPSSYMBOLICBISIM_FORWARD_EXPLORATION_H

// #define DBM_PACKAGE_AVAILABLE 1

#include <chrono>

#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/utilities/logger.h"

#include "simplifier.h"
#ifdef DBM_PACKAGE_AVAILABLE
  #include "simplifier_dbm.h"
#else
  #include "simplifier_fourier_motzkin.h"
#endif

namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

template <typename Specification>
class forward_exploration_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  typedef rewriter::substitution_type substitution_t;
  using super::m_spec;

protected:
  rewrite_strategy m_strat;
  rewriter rewr;
  rewriter proving_rewr;
  data_expression reach;

  std::map< lps::action_summand, variable_list > m_primed_summation_variables_map;
  std::map< lps::action_summand, data_expression > m_updates_map;
  std::map< lps::action_summand, substitution_t > m_primed_substitution_map;

  std::map< variable, std::pair< data_expression, data_expression > > lu_map;

  variable_list m_process_parameters;
  variable_list m_primed_process_parameters;
  simplifier* simpl;

  data_expression symbolic_init()
  {
    data_expression result = sort_bool::true_();
    for(const assignment& init_expr: m_spec.initial_process().assignments())
    {
      result = lazy::and_(result, equal_to(init_expr.lhs(), init_expr.rhs()));
    }
    return lambda(m_process_parameters, result);
  }

  bool explore()
  {
    data_expression start = reach;

    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      variable_list primed_summand_variables = m_primed_process_parameters + m_primed_summation_variables_map[as];
      reach = sort_bool::or_(
        rewr(application(reach,m_process_parameters)),
        exists(
          primed_summand_variables,
          rewr(sort_bool::and_(
            application(reach, m_primed_process_parameters),
            sort_bool::and_(
              rewr(as.condition(), m_primed_substitution_map[as]),
              m_updates_map[as]
            )
          ))
        )
      );

      reach = one_point_rule_rewrite(quantifiers_inside_rewrite(reach));
      reach = replace_data_expressions(reach, fourier_motzkin_sigma(rewriter(m_spec.data())), true);
      reach = rewr(lambda(m_process_parameters, proving_rewr(simpl->apply(reach))));
    }

    return start != reach;
  }

  /**
   * \brief Initialise several data structures based on the information
   * contained in the input LTS.
   */
  void build_summand_maps()
  {
    // Later we will use this to build one substitution per action summand
    substitution_t sub_primed_base;
    for(const variable& v: m_process_parameters)
    {
      // Create one primed variable per process parameter
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      m_primed_process_parameters.push_front(var);
      sub_primed_base[v] = var;
    }
    m_primed_process_parameters = reverse(m_primed_process_parameters);

    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      // Base the substitution for this action summand on the
      // substitution that was created from the process parameters
      substitution_t sub_primed = sub_primed_base;
      variable_list primed_summation_variables;
      for(const variable& v: as.summation_variables())
      {
        // Create one primed variable per summation variable
        variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
        primed_summation_variables.push_front(var);
        sub_primed[v] = var;
      }
      primed_summation_variables = reverse(primed_summation_variables);

      // A list of updates for each process parameter
      data_expression update_equalities = sort_bool::true_();
      for(variable_list::const_iterator it = m_process_parameters.begin(); it != m_process_parameters.end(); it++)
      {
        assignment_list::const_iterator assign = std::find_if(as.assignments().begin(), as.assignments().end(), [&it](const assignment& arg) {return arg.lhs() == *it;});
        if(assign != as.assignments().end())
        {
          update_equalities = lazy::and_(update_equalities, equal_to(*it, rewr(assign->rhs(),sub_primed)));
        }
        else
        {
          update_equalities = lazy::and_(update_equalities, equal_to(*it, rewr(*it,sub_primed)));
        }
      }

      m_primed_substitution_map.insert(std::make_pair(as, sub_primed));
      m_primed_summation_variables_map.insert(std::make_pair(as, primed_summation_variables));
      m_updates_map.insert(std::make_pair(as, update_equalities));
    }
  }

  void determineLUbounds()
  {
    for(const variable& v: m_process_parameters)
    {
      if(v.sort() == sort_real::real_())
      {
        lu_map[v] = std::make_pair(data_expression(), data_expression());
      }
    }

    for(const lps::action_summand& as: m_spec.process().action_summands())
    {
      variable_list real_sum_vars;
      for(const variable& v: as.summation_variables())
      {
        if(v.sort() == sort_real::real_())
        {
          real_sum_vars.push_front(v);
        }
      }

      std::vector < data_expression_list > real_conditions;
      std::vector < data_expression > non_real_conditions;
      detail::split_condition(as.condition(), real_conditions, non_real_conditions);

      for(const data_expression_list& li_list: real_conditions)
      {
        std::vector< linear_inequality > simpl_vector;
        std::vector< linear_inequality > li_vector;
        for(const data_expression& d: li_list)
        {
          li_vector.push_back(linear_inequality(d, rewr));
        }
        fourier_motzkin(li_vector, real_sum_vars.begin(), real_sum_vars.end(), simpl_vector, rewr);
        for(const linear_inequality& li: simpl_vector)
        {
          // std::cout << "lin ineq " << pp(li) << " size " << li.lhs().size() << std::endl;
          if(li.lhs().size() < 2)
          {
            std::pair<data_expression, data_expression>& bounds = lu_map[li.lhs().begin()->variable_name()];
            if(is_negative(li.lhs().begin()->factor(), rewr))
            {
              // This inequality is of the shape -x_i < -r
              if(bounds.first == data_expression())
              {
                // No bound has been set yet
                bounds.first = rewr(sort_real::negate(li.rhs()));
              }
              else
              {
                bounds.first = rewr(sort_real::maximum(bounds.first, sort_real::negate(li.rhs())));
              }
            }
            else
            {
              // This inequality is of the shape x_i < r
              if(bounds.second == data_expression())
              {
                // No bound has been set yet
                bounds.second = li.rhs();
              }
              else
              {
                bounds.second = rewr(sort_real::maximum(bounds.second, li.rhs()));
              }
            }
          }
        }
      }
    }
  }

public:

  forward_exploration_algorithm(Specification& spec,
    const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec)
    , m_strat(st)
    , rewr(spec.data(),jitty)
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover)
#else
    , proving_rewr(spec.data(), jitty_prover)
#endif
    , m_process_parameters(m_spec.process().process_parameters())
  {}


  data_expression run()
  {
    rewr = rewriter(merge_data_specifications(m_spec.data(), simplifier::norm_rules_spec()), m_strat);
    determineLUbounds();

#ifdef DBM_PACKAGE_AVAILABLE
    simpl = new simplifier_dbm(rewr, proving_rewr, m_process_parameters, m_spec.data(), lu_map);
#else
    simpl = new simplifier_fourier_motzkin(rewr, proving_rewr);
#endif

    build_summand_maps();
    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = std::chrono::high_resolution_clock::now();
    int i = 0;
    reach = symbolic_init();
    while(explore())
    {
      std::cout << "Reach " << i << "  " << reach << std::endl;
      i++;
    }

    std::cout << "Exploration completed in " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << " seconds" << std::endl;
    return reach;
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_FORWARD_EXPLORATION_H
