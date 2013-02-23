// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H

#include "boost/bind.hpp"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/stategraph_graph_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Applies the reset variables expansion to a pbes expression
struct reset_variable_builder: public pbes_expression_builder<reset_variable_builder>
{
  typedef pbes_expression_builder<reset_variable_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

  std::vector<pbes_expression>::const_iterator i;

  reset_variable_builder(std::vector<pbes_expression>::const_iterator i_)
    : i(i_)
  {}

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    mCRL2log(log::debug, "stategraph") << "<reset>" << pbes_system::pp(x) << " -> " << pbes_system::pp(*i) << std::endl;
    return *i++;
  }
};

class stategraph_reset_variables_algorithm;

pbes_expression stategraph_reset_variable_rewrite(stategraph_reset_variables_algorithm& algorithm, const pbes_expression& x);

/// \brief Adds the reset variables procedure to the stategraph algorithm
class stategraph_reset_variables_algorithm: public stategraph_graph_global_algorithm
{
  protected:
    // if true, the resulting PBES is simplified
    bool m_simplify;

    data::data_expression default_value(const data::sort_expression& x)
    {
      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

    void compute_control_flow_marking()
    {
      mCRL2log(log::debug, "stategraph") << "--- compute initial marking ---" << std::endl;
      // initialization
      for (vertex_iterator i = m_control_flow_graph.begin(); i != m_control_flow_graph.end(); ++i)
      {
        stategraph_vertex& v = i->second;
        std::set<data::variable> fv = v.free_guard_variables();
        std::set<data::variable> dx = propvar_parameters(v.X.name());
        v.marking = data::detail::set_intersection(fv, dx);
        mCRL2log(log::debug, "stategraph") << "vertex " << pbes_system::pp(v.X) << " freevars = " << print_variable_set(fv) << " dx = " << print_variable_set(dx) << "\n";
      }
      mCRL2log(log::debug, "stategraph") << "--- initial control flow marking ---\n" << m_control_flow_graph.print_marking();

      // backwards reachability algorithm
      std::set<stategraph_vertex*> todo;
      for (vertex_iterator i = m_control_flow_graph.begin(); i != m_control_flow_graph.end(); ++i)
      {
        stategraph_vertex& v = i->second;
        todo.insert(&v);
      }
      mCRL2log(log::debug, "stategraph") << "--- update marking ---" << std::endl;
      while (!todo.empty())
      {
        std::set<stategraph_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        stategraph_vertex& v = **i;
        mCRL2log(log::debug, "stategraph") << "selected marking todo element " << pbes_system::pp(v.X) << std::endl;
        std::set<std::size_t> I = v.marking_variable_indices(m_pbes);

        for (std::set<stategraph_edge>::iterator i = v.incoming_edges.begin(); i != v.incoming_edges.end(); ++i)
        {
          stategraph_vertex& u = *(i->source);
          std::size_t last_size = u.marking.size();
          const propositional_variable_instantiation& Y = i->label;
          std::set<data::variable> dx = propvar_parameters(u.X.name());
          mCRL2log(log::debug, "stategraph") << "  vertex u = " << pbes_system::pp(v.X) << " label = " << pbes_system::pp(Y) << " I = " << print_set(I) << " u.marking = " << print_variable_set(u.marking) << std::endl;
          for (std::set<std::size_t>::const_iterator j = I.begin(); j != I.end(); ++j)
          {
            std::size_t m = *j;
            data::data_expression_list e = Y.parameters();
            data::data_expression e_m = nth_element(e, m);
            std::set<data::variable> fv = pbes_system::find_free_variables(e_m);
            u.marking = data::detail::set_union(data::detail::set_intersection(fv, dx), u.marking);
            mCRL2log(log::debug, "stategraph") << "  m = " << m << " freevars = " << print_variable_set(fv) << " dx = " << print_variable_set(dx) << "\n";
          }
          if (u.marking.size() > last_size)
          {
            todo.insert(&u);
            mCRL2log(log::debug, "stategraph") << "updated marking " << u.print_marking() << " using edge " << pbes_system::pp(Y) << "\n";
          }
        }
      }

      // set the marking_parameters attributes
      for (vertex_iterator i = m_control_flow_graph.begin(); i != m_control_flow_graph.end(); ++i)
      {
        stategraph_vertex& v = i->second;
        const stategraph_equation& eqn = *find_equation(m_pbes, v.X.name());
        const std::vector<data::variable>& d = eqn.parameters();
        for (std::vector<data::variable>::const_iterator j = d.begin(); j != d.end(); ++j)
        {
          v.marked_parameters.push_back(v.marking.find(*j) != v.marking.end());
        }
      }
    }

  public:
    // expands a propositional variable instantiation using the control flow graph
    pbes_expression reset_variable(const propositional_variable_instantiation& x)
    {
      mCRL2log(log::debug, "stategraph") << "  resetting variable " << pbes_system::pp(x) << std::endl;
      std::vector<pbes_expression> Xij_conjuncts;
      core::identifier_string X = x.name();
      std::vector<data::data_expression> d_X = atermpp::convert<std::vector<data::data_expression> >(x.parameters());

      // iterate over the alternatives as defined by the control flow graph
      std::set<stategraph_vertex*>& inst = m_control_flow_graph.index(X);
      for (std::set<stategraph_vertex*>::const_iterator q = inst.begin(); q != inst.end(); ++q)
      {
        stategraph_vertex& w = **q;
        mCRL2log(log::debug, "stategraph") << "    vertex X = " << pbes_system::pp(w.X) << std::endl;
        std::vector<data::data_expression> e;
        std::size_t N = w.marked_parameters.size();
        data::data_expression_list::const_iterator s = w.X.parameters().begin();
        data::data_expression condition = data::sort_bool::true_();
        for (std::size_t r = 0; r < N; ++r)
        {
          if (is_control_flow_parameter(X, r))
          {
            data::data_expression v_X_r = *s++;
            condition = data::lazy::and_(condition, data::equal_to(d_X[r], v_X_r));
            mCRL2log(log::debug, "stategraph") << "    X[" << r << "] is a stategraph parameter -> " << data::pp(v_X_r) << std::endl;
            e.push_back(v_X_r);
          }
          else if (w.is_marked_parameter(r))
          {
            mCRL2log(log::debug, "stategraph") << "    X[" << r << "] is a marked parameter -> " << data::pp(d_X[r]) << std::endl;
            e.push_back(d_X[r]);
          }
          else
          {
            mCRL2log(log::debug, "stategraph") << "    X[" << r << "] is a default parameter -> " << data::pp(default_value(d_X[r].sort())) << std::endl;
            e.push_back(default_value(d_X[r].sort()));
          }
        }
        propositional_variable_instantiation Xe(X, atermpp::convert<data::data_expression_list>(e));
        if (m_simplify)
        {
          condition = m_datar(condition);
          if (condition != data::sort_bool::false_())
          {
            Xij_conjuncts.push_back(imp(condition, Xe));
          }
        }
        else
        {
          Xij_conjuncts.push_back(imp(condition, Xe));
        }
        mCRL2log(log::debug, "stategraph") << "    condition = " << data::pp(condition) << std::endl;
        mCRL2log(log::debug, "stategraph") << "  alternative = " << pbes_system::pp(Xe) << std::endl;
      }
      return pbes_expr::join_and(Xij_conjuncts.begin(), Xij_conjuncts.end());
    }

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes<>& p)
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variables to the original PBES ---" << std::endl;

      // apply the reset variable procedure to all propositional variable instantiations
      pbes_system::pbes_rewrite(p, boost::bind(stategraph_reset_variable_rewrite, *this, _1));

      // TODO: merge the two rewriters?
      if (m_simplify)
      {
        pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
        pbes_system::pbes_rewrite(p, pbesr);
      }
    }

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes<> run(const pbes<>& p, bool simplify = true, bool /* apply_to_original_pbes */ = false)
    {
      m_simplify = simplify;

      compute_control_flow_marking();
      mCRL2log(log::verbose) <<  "--- control flow marking ---\n" << m_control_flow_graph.print_marking();
      pbes<> result = p;
      reset_variables_to_original(result);
      return result;
    }
};

struct stategraph_reset_variable_rewrite_builder: public pbes_expression_builder<stategraph_reset_variable_rewrite_builder>
{
  typedef pbes_expression_builder<stategraph_reset_variable_rewrite_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

  stategraph_reset_variables_algorithm& algorithm;

  stategraph_reset_variable_rewrite_builder(stategraph_reset_variables_algorithm& algorithm_)
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
pbes_expression stategraph_reset_variable_rewrite(stategraph_reset_variables_algorithm& algorithm, const pbes_expression& x)
{
  stategraph_reset_variable_rewrite_builder f(algorithm);
  return f(x);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_RESET_VARIABLES_H
