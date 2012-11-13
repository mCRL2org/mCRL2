// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_RESET_VARIABLES_H

#include "boost/bind.hpp"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/pbes/detail/control_flow_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class control_flow_reset_variables_algorithm;

pbes_expression reset_variable_rewrite(control_flow_reset_variables_algorithm& algorithm, const pbes_expression& x);

/// \brief Adds the reset variables procedure to the control_flow algorithm
class control_flow_reset_variables_algorithm: public control_flow_graph_algorithm
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

    std::string print_control_flow_marking(const control_flow_vertex& v) const
    {
      std::ostringstream out;
      out << "vertex " << pbes_system::pp(v.X) << " = " << print_variable_set(v.marking);
      return out.str();
    }

    std::string print_control_flow_marking() const
    {
      std::ostringstream out;
      out << "--- control flow marking ---" << std::endl;
      for (std::map<propositional_variable_instantiation, control_flow_vertex>::const_iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        const control_flow_vertex& v = i->second;
        out << print_control_flow_marking(v) << std::endl;
      }
      return out.str();
    }

    void compute_control_flow_marking()
    {
      // initialization
      for (std::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        std::set<data::variable> fv = v.free_guard_variables();
        std::set<data::variable> dx = propvar_parameters(v.X.name());
        v.marking = data::detail::set_intersection(fv, dx);
        mCRL2log(log::debug, "control_flow") << "initial marking " << print_control_flow_marking(v) << "\n";
      }

      // backwards reachability algorithm
      std::set<control_flow_vertex*> todo;
      for (std::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        todo.insert(&v);
      }
      while (!todo.empty())
      {
        std::set<control_flow_vertex*>::iterator i = todo.begin();
        todo.erase(i);
        control_flow_vertex& v = **i;
        mCRL2log(log::debug, "control_flow") << "selected marking todo element " << pbes_system::pp(v.X) << std::endl;
        std::set<std::size_t> I = v.marking_variable_indices(m_pbes);

        for (std::set<control_flow_edge>::iterator i = v.incoming_edges.begin(); i != v.incoming_edges.end(); ++i)
        {
          control_flow_vertex& u = *(i->source);
          std::size_t last_size = u.marking.size();
          const propositional_variable_instantiation& Xij = i->label;
          std::set<data::variable> dx = propvar_parameters(u.X.name());
          for (std::set<std::size_t>::const_iterator j = I.begin(); j != I.end(); ++j)
          {
            std::size_t m = *j;
            data::data_expression_list e = Xij.parameters();
            data::data_expression_list::const_iterator k = e.begin();
            for (std::size_t p = 0; p < m; ++p)
            {
              ++k;
            }
            data::data_expression e_m = *k;
            std::set<data::variable> fv = pbes_system::find_free_variables(e_m);
            u.marking = data::detail::set_union(data::detail::set_intersection(fv, dx), u.marking);
          }
          if (u.marking.size() > last_size)
          {
            todo.insert(&u);
            mCRL2log(log::debug, "control_flow") << "updated marking " << print_control_flow_marking(u) << " using edge " << pbes_system::pp(Xij) << "\n";
          }
        }
      }

      // set the marking_parameters attributes
      for (std::map<propositional_variable_instantiation, control_flow_vertex>::iterator i = m_control_vertices.begin(); i != m_control_vertices.end(); ++i)
      {
        control_flow_vertex& v = i->second;
        const pfnf_equation& eqn = *find_equation(m_pbes, v.X.name());
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
      mCRL2log(log::debug, "control_flow") << "<reset_variable>" << pbes_system::pp(x) << std::endl;
      std::vector<pbes_expression> Xij_conjuncts;
      core::identifier_string X = x.name();
      std::vector<data::data_expression> d_X = atermpp::convert<std::vector<data::data_expression> >(x.parameters());

      // iterate over the alternatives as defined by the control flow graph
      std::set<control_flow_vertex*>& inst = m_control_flow_index[X];
      for (std::set<control_flow_vertex*>::const_iterator q = inst.begin(); q != inst.end(); ++q)
      {
        control_flow_vertex& w = **q;
        mCRL2log(log::debug, "control_flow") << "  <vertex>" << pbes_system::pp(w.X) << std::endl;
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
            e.push_back(v_X_r);
          }
          else if (w.is_marked_parameter(r))
          {
            e.push_back(d_X[r]);
          }
          else
          {
            e.push_back(default_value(d_X[r].sort()));
          }
        }
        propositional_variable_instantiation Xe(X, atermpp::convert<data::data_expression_list>(e));
        mCRL2log(log::debug, "control_flow") << "  <alternative>" << pbes_system::pp(Xe) << std::endl;
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
      }
      return pbes_expr::join_and(Xij_conjuncts.begin(), Xij_conjuncts.end());
    }

    // generates a PBES from the control flow graph and the marking
    pbes<> reset_variables()
    {
      pbes<> result;
      result.initial_state() = m_pbes.initial_state();
      result.data() = m_pbes.data();
      result.global_variables() = m_pbes.global_variables();

      pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);

      // expand the equations, and add them to the result
      std::vector<pfnf_equation>& equations = m_pbes.equations();
      for (std::vector<pfnf_equation>::iterator k = equations.begin(); k != equations.end(); ++k)
      {
        mCRL2log(log::debug, "control_flow") << "<reset equation>" << pbes_system::pp(k->convert()) << std::endl;
        std::vector<pfnf_implication>& implications = k->implications();
        std::vector<pbes_expression> new_implications;
        for (std::vector<pfnf_implication>::iterator i = implications.begin(); i != implications.end(); ++i)
        {
          std::vector<propositional_variable_instantiation>& v = i->variables();
          std::vector<pbes_expression> disjuncts;
          for (std::vector<propositional_variable_instantiation>::iterator j = v.begin(); j != v.end(); ++j)
          {
            disjuncts.push_back(reset_variable(*j));
          }
          pbes_expression dj = pbes_expr::join_or(disjuncts.begin(), disjuncts.end());
          if (m_simplify)
          {
            new_implications.push_back(utilities::optimized_imp(pbesr(i->g()), dj));
          }
          else
          {
            new_implications.push_back(imp(i->g(), dj));
          }
        }
        pbes_expression phi = pbes_expr::join_and(new_implications.begin(), new_implications.end());
        pbes_equation eqn = k->apply_implication(phi);
        if (!eqn.is_well_typed())
        {
          mCRL2log(log::warning) << "<eqn not well typed>" << pbes_system::pp(eqn) << std::endl;
        }
        result.equations().push_back(eqn);
      }

      return result;
    }

    // Applies resetting of variables to the original PBES p.
    void reset_variables(pbes<>& p)
    {
      // apply the reset variable procedure to all propositional variable instantiations
      pbes_system::pbes_rewrite(p, boost::bind(reset_variable_rewrite, *this, _1));

      // TODO: merge the two rewriters?
      if (m_simplify)
      {
        pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
        pbes_system::pbes_rewrite(p, pbesr);
      }
    }

    /// \brief Runs the control_flow algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the PFNF one
    pbes<> run(const pbes<>& p, bool simplify = true, bool apply_to_original_pbes = false)
    {
      m_simplify = simplify;

      compute_control_flow_marking();
      mCRL2log(log::verbose) << print_control_flow_marking();

      if (apply_to_original_pbes)
      {
        return reset_variables();
      }
      else
      {
        pbes<> result = p;
        reset_variables(result);
        return result;
      }
    }
};

struct reset_variable_rewrite_builder: public pbes_expression_builder<reset_variable_rewrite_builder>
{
  typedef pbes_expression_builder<reset_variable_rewrite_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

  control_flow_reset_variables_algorithm& algorithm;

  reset_variable_rewrite_builder(control_flow_reset_variables_algorithm& algorithm_)
    : algorithm(algorithm_)
  {}

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    return algorithm.reset_variable(x);
  }
};

inline
pbes_expression reset_variable_rewrite(control_flow_reset_variables_algorithm& algorithm, const pbes_expression& x)
{
  reset_variable_rewrite_builder f(algorithm);
  return f(x);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_RESET_VARIABLES_H
