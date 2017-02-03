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

#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/set.h"


namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

template <typename Specification>
class symbolic_bisim_algorithm: public mcrl2::lps::detail::lps_algorithm<Specification>
{
  typedef typename mcrl2::lps::detail::lps_algorithm<Specification> super;
  typedef typename Specification::process_type process_type;
  typedef typename process_type::action_summand_type action_summand_type;
  using super::m_spec;

protected:
  const rewrite_strategy strat;
  rewriter r;

  variable_list                  process_parameters;
  data_expression_vector         partition;

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
    for(data_expression_vector::const_iterator it = partition.cbegin(); it != partition.cend(); it++)
    {
      std::cout << "  block " << i << "  " << pp(*it) << std::endl;
      i++;
    }
    std::cout << "Specify phi_k, phi_l, and summand index (separate with spaces): ";
    std::string s;
    std::getline(std::cin, s);

    int k = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int l = std::stoi(s.substr(0,s.find(' ')));
    s = s.substr(s.find(' ') + 1);
    int summ_i = std::stoi(s);
    std::cout <<
      "phi_k   " << partition[k] << std::endl <<
      "phi_l   " << partition[l] << std::endl <<
      "summand " << m_spec.process().action_summands()[summ_i] << std::endl;

    lps::action_summand as = m_spec.process().action_summands()[summ_i];
    const lps::multi_action& action = as.multi_action();
    data_expression_list args = action.arguments();
    i = 0;
    data_expression match_concrete_args = sort_bool::true_();
    for(data_expression_list::const_iterator it = args.begin(); it != args.end(); it++)
    {
      std::cout << "Specify action parameter " << i << ": ";
      std::getline(std::cin, s);
      match_concrete_args = lazy::and_(match_concrete_args, equal_to(*it , parse_data_expression(s, variable_list(), m_spec.data())));
    }

    data_expression_list updates;
    for(assignment_list::const_iterator it = as.assignments().begin(); it != as.assignments().end(); it++)
    {
      updates.push_front(it->rhs());
    }
    data_expression split1, split2;
    data_expression exist = lazy::and_(
            match_concrete_args,
            lazy::and_(
              as.condition(),
              application(partition[l],updates)
        ));
    if(!as.summation_variables().empty())
    {
      exist = exists(as.summation_variables(), exist);
    }
    split1 = lambda(process_parameters, 
      lazy::and_(
        application(partition[k],process_parameters),
        exist));
    split2 = lambda(process_parameters, 
      lazy::and_(
        application(partition[k],process_parameters),
        lazy::not_(exist)));

    partition.erase(partition.begin() + k);
    partition.push_back(r(split2));
    partition.push_back(r(split1));
  }

public:
  symbolic_bisim_algorithm(Specification& spec, const rewrite_strategy st = jitty)
    : mcrl2::lps::detail::lps_algorithm<Specification>(spec),
    strat(st),
    r(spec.data(),strat)
  {
  }

  void run()
  {
    mCRL2log(verbose) << "Running symbolic bisimulation.." << std::endl;
    process_parameters = m_spec.process().process_parameters();
    partition.push_back( lambda(process_parameters, sort_bool::true_()));
    while(true)
    {
      refine();
    }
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_LPSSYMBOLICBISIM_SYMBOLIC_BISIM_H