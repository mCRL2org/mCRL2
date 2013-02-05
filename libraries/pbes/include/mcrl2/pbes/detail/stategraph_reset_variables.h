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
#include "mcrl2/pbes/detail/stategraph_graph.h"

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

  atermpp::vector<pbes_expression>::const_iterator i;

  reset_variable_builder(atermpp::vector<pbes_expression>::const_iterator i_)
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
class stategraph_reset_variables_algorithm: public stategraph_graph_algorithm
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

    std::string print_variable_set(const std::set<data::variable>& v) const
    {
      std::ostringstream out;
      out << "{";
      for (std::set<data::variable>::const_iterator j = v.begin(); j != v.end(); ++j)
      {
        if (j != v.begin())
        {
          out << ", ";
        }
        out << data::pp(*j);
      }
      out << "}";
      return out.str();
    }

    std::string print_control_flow_marking(const stategraph_vertex& v) const
    {
      std::ostringstream out;
      out << "vertex " << pbes_system::pp(v.X) << " = " << print_variable_set(v.marking);
      return out.str();
    }

    std::string print_control_flow_marking() const
    {
      std::ostringstream out;
      for (atermpp::map<propositional_variable_instantiation, stategraph_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        const stategraph_vertex& v = i->second;
        out << print_control_flow_marking(v) << std::endl;
      }
      return out.str();
    }

    void compute_control_flow_marking()
    {
      mCRL2log(log::debug, "stategraph") << "--- compute initial marking ---" << std::endl;
      // initialization
      for (atermpp::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        stategraph_vertex& v = i->second;
        std::set<data::variable> fv = v.free_guard_variables();
        std::set<data::variable> dx = propvar_parameters(v.X.name());
        v.marking = data::detail::set_intersection(fv, dx);
        mCRL2log(log::debug, "stategraph") << "vertex " << pbes_system::pp(v.X) << " freevars = " << print_variable_set(fv) << " dx = " << print_variable_set(dx) << "\n";
      }
      mCRL2log(log::debug, "stategraph") << "--- initial control flow marking ---\n" << print_control_flow_marking();

      // backwards reachability algorithm
      std::set<stategraph_vertex*> todo;
      for (atermpp::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
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

        for (atermpp::set<stategraph_edge>::iterator i = v.incoming_edges.begin(); i != v.incoming_edges.end(); ++i)
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
            mCRL2log(log::debug, "stategraph") << "updated marking " << print_control_flow_marking(u) << " using edge " << pbes_system::pp(Y) << "\n";
          }
        }
      }

      // set the marking_parameters attributes
      for (atermpp::map<propositional_variable_instantiation, stategraph_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
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
      atermpp::vector<pbes_expression> Xij_conjuncts;
      core::identifier_string X = x.name();
      std::vector<data::data_expression> d_X = atermpp::convert<std::vector<data::data_expression> >(x.parameters());

      // iterate over the alternatives as defined by the control flow graph
      std::set<stategraph_vertex*>& inst = m_stategraph_index[X];
      for (std::set<stategraph_vertex*>::const_iterator q = inst.begin(); q != inst.end(); ++q)
      {
        stategraph_vertex& w = **q;
        mCRL2log(log::debug, "stategraph") << "    vertex X = " << pbes_system::pp(w.X) << std::endl;
        atermpp::vector<data::data_expression> e;
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

    // generates a PBES from the control flow graph and the marking
    pbes<> reset_variables()
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variables ---" << std::endl;
      pbes<> result;
      result.initial_state() = m_pbes.initial_state();
      result.data() = m_pbes.data();
      result.global_variables() = m_pbes.global_variables();

      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);

      atermpp::vector<stategraph_equation>& equations = m_pbes.equations();
      for (atermpp::vector<stategraph_equation>::iterator k = equations.begin(); k != equations.end(); ++k)
      {
        stategraph_equation& eqn = *k;
        mCRL2log(log::debug, "stategraph") << "resetting equation: " << print_equation(eqn) << std::endl;
        atermpp::vector<pbes_expression> replacements;
        const predicate_variable_vector& predvars = eqn.predicate_variables();
        for (predicate_variable_vector::const_iterator i = predvars.begin(); i != predvars.end(); ++i)
        {
          replacements.push_back(reset_variable(i->first));
        }
        reset_variable_builder f(replacements.begin());
        eqn.formula() = f(eqn.formula());
        mCRL2log(log::debug, "stategraph") << "resetted equation:  " << print_equation(eqn) << std::endl;
        result.equations().push_back(eqn);
      }
      return result;
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
    pbes<> run(const pbes<>& p, bool simplify = true, bool apply_to_original_pbes = false)
    {
      m_simplify = simplify;

      compute_control_flow_marking();
      mCRL2log(log::verbose) <<  "--- control flow marking ---\n" << print_control_flow_marking();

      if (apply_to_original_pbes)
      {
        pbes<> result = p;
        reset_variables_to_original(result);
        return result;
      }
      else
      {
        return reset_variables();
      }
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

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    return algorithm.reset_variable(x);
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
