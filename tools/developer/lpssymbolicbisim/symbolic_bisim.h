// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include <string>

#include "mcrl2/data/bool.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fourier_motzkin.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/utilities/logger.h"


namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

struct eliminate_existential_quantifier_sigma: public std::unary_function<data_expression, data_expression>
{

  rewriter rewr;

  eliminate_existential_quantifier_sigma(rewriter rewr_)
  :  rewr(rewr_)
  {}

  const data_expression operator()(const data_expression& d) const
  {
    if(is_exists(d))
    {
      const variable_list variables = static_cast<exists>(d).variables();
      const data_expression body = static_cast<exists>(d).body();

      variable_list new_variables;
      data_expression new_body;
      fourier_motzkin(body, variables, new_body, new_variables, rewr);
      std::cout << "Simplified " << pp(d) << " to " << exists(new_variables, new_body) << std::endl;
      return rewr(exists(new_variables, new_body));
    }
    return d;
  }
};

template <typename Specification>
class symbolic_bisim_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  typedef rewriter::substitution_type substitution_t;
  using super::m_spec;

protected:
  const rewrite_strategy strat;
  rewriter rewr;

  variable_list                  process_parameters;
  std::set<data_expression>      partition;
  // data_expression                part_union;
  // sort_expression                union_sort;
  // structured_sort                singleton_sort;
  data_specification             ad_hoc_data;

  void add_ad_hoc_rules()
  {
    ad_hoc_data = m_spec.data();
    variable vb1("b1", sort_bool::bool_());
    variable vb2("b2", sort_bool::bool_());
    variable vp1("p1", sort_pos::pos());
    variable vp2("p2", sort_pos::pos());
    variable vr1("r1", sort_real::real_());
    variable vr2("r2", sort_real::real_());

    //  a && a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::and_(vb1, vb1), vb1));
    //  a && (a && b) = a && b;
    // ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1,vb2), sort_bool::and_(vb1, sort_bool::and_(vb1, vb2)), sort_bool::and_(vb1, vb2)));
    //  a || a = a;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1), sort_bool::or_(vb1, vb1), vb1));
    //  a => b = !a || b;
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::implies(vb1, vb2), sort_bool::or_(sort_bool::not_(vb1), vb2)));
    // Pushing not inside
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::and_(vb1, vb2)), sort_bool::or_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(sort_bool::or_(vb1, vb2)), sort_bool::and_(sort_bool::not_(vb1), sort_bool::not_(vb2))));
    // Formulate all linear equalities with positive rhs: -1 * x_P <= -5   !(1 * x_P < 5)
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2), 
      less_equal(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)), 
      sort_bool::not_(less(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1, vp1, vp2), 
      less(vr1, sort_real::creal(sort_int::cneg(vp1), vp2)), 
      sort_bool::not_(less_equal(sort_real::times(real_minus_one(), vr1), sort_real::creal(sort_int::cint(sort_nat::cnat(vp1)), vp2)))));
    // Eliminate nested -1 * -1
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_minus_one(), sort_real::times(real_minus_one(), vr1)), vr1));
    // Eliminate 1 * r
    ad_hoc_data.add_equation(data_equation(atermpp::make_vector(vr1), sort_real::times(real_one(), vr1), vr1));


    rewr = rewriter(ad_hoc_data, strat);
  }

  void enumerate(data_expression block, std::set<variable> vars, data_expression split_block)
  {
    typedef enumerator_algorithm_with_iterator<rewriter> enumerator_type;
    enumerator_type enumerator(rewr, m_spec.data(), rewr, 10000);

    data::mutable_indexed_substitution<> sigma;
    std::deque<enumerator_list_element_with_substitution<> >
         enumerator_deque(1, enumerator_list_element_with_substitution<>(variable_list(vars.begin(), vars.end()), block));
    for (enumerator_type::iterator i = enumerator.begin(sigma, enumerator_deque); i != enumerator.end(); ++i)
    {
      i->add_assignments(vars,sigma,rewr);

      partition.insert(rewr(split_block,sigma));
    }
  }

  void refine()
  {
    std::cout << "=====================================================" << std::endl;
    int i = 0;
    for(typename std::vector< action_summand_type >::const_iterator it = m_spec.process().action_summands().begin();
        it != m_spec.process().action_summands().end(); it++)
    {
      std::cout << "Summand " << i << " " << pp(*it) << std::endl;
      i++;
    }
    std::cout << "Partition:" << std::endl;
    i = 0;
    data_expression_vector indexed_blocks;
    for(std::set< data_expression >::const_iterator it = partition.cbegin(); it != partition.cend(); it++)
    {
      std::cout << "  block " << i << "  " << pp(*it) << std::endl;
      i++;
      indexed_blocks.push_back(*it);
    }
    std::cout << "Specify phi_k, phi_l, and summand index (separate with spaces): ";
    std::string s;
    std::getline(std::cin, s);

    int k = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int l = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int summ_i = std::stoi(s);
    data_expression phi_k = indexed_blocks[k];
    data_expression phi_l = indexed_blocks[l];
    std::cout <<
      "phi_k   " << phi_k << std::endl <<
      "phi_l   " << phi_l << std::endl <<
      "summand " << m_spec.process().action_summands()[summ_i] << std::endl;

    const lps::action_summand as = m_spec.process().action_summands()[summ_i];
    const lps::multi_action action = as.multi_action();
    substitution_t sub_primed;
    variable_list primed_summation_variables;
    for(const variable& v: as.summation_variables())
    {
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      primed_summation_variables.push_front(var);
      sub_primed[v] = var;
    }
    primed_summation_variables = reverse(primed_summation_variables);
    variable_list primed_process_parameters;
    for(const variable& v: process_parameters)
    {
      variable var(static_cast<std::string>(v.name()) + std::string("'"), v.sort());
      primed_process_parameters.push_front(var);
      sub_primed[v] = var;
    }
    primed_process_parameters = reverse(primed_process_parameters);
    variable_list primed_variables = primed_process_parameters + primed_summation_variables;

    data_expression_list updates;
    for(assignment_list::const_iterator it = as.assignments().begin(); it != as.assignments().end(); it++)
    {
      updates.push_front(it->rhs());
    }

    data_expression arguments_equal = sort_bool::true_();
    for(const data_expression& expr: action.arguments())
    {
      arguments_equal = lazy::and_(arguments_equal, equal_to(expr, rewr(expr, sub_primed)));
    }
    std::cout << "Constructing block expression ..." << std::endl;
    data_expression split_block =
      rewr(lambda(process_parameters,
        lazy::and_(
          application(phi_k,process_parameters),
          lazy::and_(
            sort_bool::not_(exists(primed_summation_variables,
              sort_bool::not_(lazy::implies(
                rewr(lazy::and_(
                  as.condition(),
                  application(phi_l, updates)), sub_primed),
                exists(as.summation_variables(),
                  lazy::and_(
                    lazy::and_(
                      as.condition(),
                      application(phi_l, updates)),
                    arguments_equal
                  )
                )
              ))
            )),
            sort_bool::not_(exists(as.summation_variables(),
              sort_bool::not_(lazy::implies(
                lazy::and_(
                  as.condition(),
                  application(phi_l, updates)),
                exists(primed_summation_variables,
                  lazy::and_(
                    rewr(lazy::and_(
                      as.condition(),
                      application(phi_l, updates)), sub_primed),
                    arguments_equal
                  )
                )
              ))
            ))
          )
        )
      ));

    std::cout << "Starting simplification of " << std::endl << pp(split_block) << std::endl;
    
    data_expression result = replace_data_expressions(split_block, eliminate_existential_quantifier_sigma(rewr), true);
    std::cout << "Simplified block: " << std::endl << pp(result) << std::endl;

    split_block = rewr(result);
    std::cout << "New block before enumeration: " << split_block << std::endl;
    std::set<variable> free_vars = find_free_variables(split_block);
    std::cout << "Free variables: {";
    for(const variable& v: free_vars)
    {
      std::cout << v << ", ";
    }
    std::cout << "}" << std::endl;

    data_expression original_block = phi_k;
    partition.erase(partition.find(phi_k));
    enumerate(application(original_block,primed_process_parameters), free_vars, split_block);
  }

  // void simplify()
  // {
  // }

  // data_expression find_initial_block()
  // {
  //   for(std::set< data_expression >::const_iterator it = partition.begin(); it != partition.end(); it++)
  //   {
  //     if(rewr((*it)(m_spec.initial_process().state(process_parameters))) == sort_bool::true_())
  //     {
  //       std::cout << "Found initial block " << *it << std::endl;
  //       return *it;
  //     }
  //   }
  //   throw mcrl2::runtime_error("Initial block not found");
  // }

  // void reach()
  // {
  //   std::set<data_expression> reachable;
  //   data_expression init = find_initial_block();
  //   reachable.insert(init);
  // }

public:
  symbolic_bisim_algorithm(Specification& spec, const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec),
    strat(st),
    rewr(spec.data(),strat)
  {
  }

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;
    process_parameters = m_spec.process().process_parameters();
    partition.insert( lambda(process_parameters, sort_bool::true_()));
    add_ad_hoc_rules();

    while(true)
    {
      refine();
    }
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H
