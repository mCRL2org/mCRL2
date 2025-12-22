// Author(s): Jan Friso Groote, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parvalues.h
/// \brief Explore statically which parameter values may be reached

#ifndef MCRL2_LPS_LPSPARVALUES_H
#define MCRL2_LPS_LPSPARVALUES_H

#include "mcrl2/data/detail/parvalues.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/suminst.h"

namespace mcrl2::lps
{

template <typename DataRewriter, typename Specification>
class lps_parvalues_algorithm: public data::detail::parvalues_algorithm<DataRewriter>
{
  using super = typename data::detail::parvalues_algorithm<DataRewriter>;
  using super::m_graph;
  using super::m_rewriter;

protected:
  Specification m_spec;

public:

  lps_parvalues_algorithm(const Specification& spec,
                  DataRewriter& r,
                  const std::size_t qlimit,
                  const std::size_t max_rounds)
    : super(r, spec.data(), qlimit, max_rounds)
    , m_spec(spec)
  {}

  std::map<data::variable, std::unordered_set<data::data_expression>> run()
  {
    detail::instantiate_global_variables(m_spec);

    // First instantiate the finite sorts in the sum operator.
    std::set<data::sort_expression> sorts = lps::finite_sorts(m_spec.data());
    mCRL2log(log::verbose) << "expanding summation variables of sorts: " << data::pp(sorts) << std::endl;
    lps::suminst_algorithm<DataRewriter, Specification>(m_spec, m_rewriter, sorts).run();

    //TODO: consider stochastic action summands as well
    for (const action_summand& a: m_spec.process().action_summands())
    {
      m_graph.add_edge(a.summation_variables(),
                       a.condition(),
                       a.assignments() | std::views::transform([](const data::assignment& as){
                        return std::make_pair(data::detail::parameter(as.lhs()), as.rhs());
                       }));
    }

    const data::variable_list& pars = m_spec.process().process_parameters();
    const data::data_expression_list& init = m_spec.initial_process().expressions();
    for (const auto& [v,e] : utilities::zip(pars, init))
    {
      m_graph.new_parameter(v, m_rewriter(e));
    }

    super::run();
    
    std::map<data::variable, std::unordered_set<data::data_expression>> result;
    for (const data::variable& var: m_spec.process().process_parameters())
    {
      result.emplace(var, m_graph.at(var).stable);
    }
    return result;
  }
};

} // namespace mcrl2::lps

#endif