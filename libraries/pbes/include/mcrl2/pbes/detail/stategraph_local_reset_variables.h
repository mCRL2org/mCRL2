// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_local_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H

#include "mcrl2/pbes/detail/stategraph_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class local_reset_variables_algorithm;

pbes_expression local_reset_variable_rewrite(local_reset_variables_algorithm& algorithm, const pbes_expression& x);

/// \brief Adds the reset variables procedure to the stategraph algorithm
class local_reset_variables_algorithm: public stategraph_local_algorithm
{
  public:
    typedef stategraph_local_algorithm super;

  protected:
    const pbes<>& m_original_pbes;

    // if true, the resulting PBES is simplified
    bool m_simplify;

    data::data_expression default_value(const data::sort_expression& x)
    {
      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

  public:
    // expands a propositional variable instantiation using the control flow graph
    // x = X(e)
    pbes_expression reset_variable(const propositional_variable_instantiation& x)
    {
      mCRL2log(log::debug, "stategraph") << "  resetting variable " << pbes_system::pp(x) << std::endl;
      std::vector<pbes_expression> phi;
      core::identifier_string X = x.name();
      std::vector<data::data_expression> e_X = atermpp::convert<std::vector<data::data_expression> >(x.parameters());
/*
      // iterate over the alternatives as defined by the control flow graph
      const std::set<stategraph_vertex*>& inst = m_control_flow_graph.index(x.name());
      for (std::set<stategraph_vertex*>::const_iterator q = inst.begin(); q != inst.end(); ++q)
      {
        stategraph_vertex& u = **q;
        mCRL2log(log::debug, "stategraph") << "    vertex X = " << pbes_system::pp(u.X) << std::endl;
        std::vector<data::data_expression> r;
        std::size_t N = u.marked_parameters.size();
        data::data_expression_list::const_iterator i = u.X.parameters().begin();
        data::data_expression condition = data::sort_bool::true_();
        for (std::size_t j = 0; j < N; ++j)
        {
          if (is_control_flow_parameter(X, j))
          {
            data::data_expression f_i = *i++;
            condition = data::lazy::and_(condition, data::equal_to(e_X[j], f_i));
            mCRL2log(log::debug, "stategraph") << "    X[" << j << "] is a stategraph parameter -> " << data::pp(f_i) << std::endl;
            r.push_back(f_i);
          }
          else if (u.is_marked_parameter(j))
          {
            mCRL2log(log::debug, "stategraph") << "    X[" << j << "] is a marked parameter -> " << data::pp(e_X[j]) << std::endl;
            r.push_back(e_X[j]);
          }
          else
          {
            mCRL2log(log::debug, "stategraph") << "    X[" << j << "] is a default parameter -> " << data::pp(default_value(e_X[j].sort())) << std::endl;
            r.push_back(default_value(e_X[j].sort()));
          }
        }
        propositional_variable_instantiation Xe(X, atermpp::convert<data::data_expression_list>(r));
        if (m_simplify)
        {
          condition = m_datar(condition);
          if (condition != data::sort_bool::false_())
          {
            phi.push_back(imp(condition, Xe));
          }
        }
        else
        {
          phi.push_back(imp(condition, Xe));
        }
        mCRL2log(log::debug, "stategraph") << "    condition = " << data::pp(condition) << std::endl;
        mCRL2log(log::debug, "stategraph") << "  alternative = " << pbes_system::pp(Xe) << std::endl;
      }
      return pbes_expr::join_and(phi.begin(), phi.end());
*/
      return x;
    }

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes<>& p)
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variables to the original PBES ---" << std::endl;

      // apply the reset variable procedure to all propositional variable instantiations
      pbes_system::pbes_rewrite(p, boost::bind(local_reset_variable_rewrite, *this, _1));

      // TODO: merge the two rewriters?
      if (m_simplify)
      {
        pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
        pbes_system::pbes_rewrite(p, pbesr);
      }
    }

    local_reset_variables_algorithm(const pbes<>& p, data::rewriter::strategy rewrite_strategy = data::jitty)
      : stategraph_local_algorithm(p, rewrite_strategy),
        m_original_pbes(p)
    {}

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes<> run(bool simplify = true)
    {
      super::run();
      m_simplify = simplify;
      pbes<> result = m_original_pbes;
      reset_variables_to_original(result);
      return result;
    }
};

struct local_reset_variable_rewrite_builder: public pbes_expression_builder<local_reset_variable_rewrite_builder>
{
  typedef pbes_expression_builder<local_reset_variable_rewrite_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

  local_reset_variables_algorithm& algorithm;

  local_reset_variable_rewrite_builder(local_reset_variables_algorithm& algorithm_)
    : algorithm(algorithm_)
  {}

  bool has_equal_variables(const pbes_expression& x, const propositional_variable_instantiation& Y) const
  {
    std::set<propositional_variable_instantiation> v = find_propositional_variable_instantiations(x);
    for (std::set<propositional_variable_instantiation>::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      if (i->name() != Y.name())
      {
        return false;
      }
    }
    return true;
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    pbes_expression result = algorithm.reset_variable(x);
    assert(has_equal_variables(result, x));
    return result;
  }
};

inline
pbes_expression local_reset_variable_rewrite(local_reset_variables_algorithm& algorithm, const pbes_expression& x)
{
  local_reset_variable_rewrite_builder f(algorithm);
  return f(x);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
