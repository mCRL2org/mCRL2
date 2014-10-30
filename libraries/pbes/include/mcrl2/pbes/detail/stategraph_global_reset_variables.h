// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_global_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_RESET_VARIABLES_H

#include "mcrl2/pbes/detail/stategraph_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_global_algorithm.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class global_reset_variables_algorithm;
pbes_expression reset_variables(global_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X);

/// \brief Adds the reset variables procedure to the stategraph algorithm
class global_reset_variables_algorithm: public stategraph_global_algorithm
{
  public:
    typedef stategraph_global_algorithm super;

  protected:
    const pbes& m_original_pbes;

    // if true, the resulting PBES is simplified
    bool m_simplify;

    data::data_expression default_value(const data::sort_expression& x)
    {
      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

    // returns the parameters of the propositional variable with name X
    std::set<data::variable> propvar_parameters(const core::identifier_string& X) const
    {
      auto const& eq_X = *find_equation(m_pbes, X);
      auto const& d_X = eq_X.parameters();
      return std::set<data::variable>(d_X.begin(), d_X.end());
    }

    template <typename Graph>
    void compute_global_control_flow_marking(Graph& G)
    {
      using utilities::detail::pick_element;

      mCRL2log(log::debug, "stategraph") << "--- compute initial marking ---" << std::endl;
      // initialization
      for (auto i = G.begin(); i != G.end(); ++i)
      {
#ifdef MCRL2_STATEGRAPH_NEW_GLOBAL_ALGORITHM
        auto const& v = *i;
#else
        auto const& v = i->second;
#endif
        std::set<data::variable> dx = propvar_parameters(v.name());
        v.set_marking(data::detail::set_intersection(v.sig(), dx));
        mCRL2log(log::debug, "stategraph") << "vertex " << v << " sig = " << core::detail::print_set(v.sig()) << " dx = " << core::detail::print_set(dx) << "\n";
      }
      mCRL2log(log::debug, "stategraph") << "--- initial control flow marking ---\n" << G.print_marking();

      // backwards reachability algorithm
      std::set<const typename Graph::vertex_type*> todo;
      for (auto i = G.begin(); i != G.end(); ++i)
      {
#ifdef MCRL2_STATEGRAPH_NEW_GLOBAL_ALGORITHM
        auto const& v = *i;
#else
        auto const& v = i->second;
#endif
        todo.insert(&v);
      }
      mCRL2log(log::debug, "stategraph") << "--- update marking ---" << std::endl;
      while (!todo.empty())
      {
        auto const& v = *pick_element(todo);
        mCRL2log(log::debug, "stategraph") << "selected marking todo element " << v << std::endl;
        std::set<std::size_t> I = v.marking_variable_indices(m_pbes);

#ifdef MCRL2_STATEGRAPH_NEW_GLOBAL_ALGORITHM
        auto const& incoming_edges = v.incoming_edges();
        for (auto ei = incoming_edges.begin(); ei != incoming_edges.end(); ++ei)
        {
          auto const& u = *ei->first;
          const std::set<std::size_t>& labels = ei->second;
          for (auto li = labels.begin(); li != labels.end(); ++li)
          {
            std::size_t i = *li;
            std::size_t last_size = u.marking().size();
            const stategraph_equation& eq_X = *find_equation(m_pbes, u.name());
            const propositional_variable_instantiation& Y = eq_X.predicate_variables()[i].variable();
            std::set<data::variable> dx = propvar_parameters(u.name());
            mCRL2log(log::debug, "stategraph") << "  vertex u = " << v << " label = " << i << " I = " << print_set(I) << " u.marking = " << core::detail::print_set(u.marking()) << std::endl;
            for (auto j = I.begin(); j != I.end(); ++j)
            {
              std::size_t m = *j;
              data::data_expression_list e = Y.parameters();
              data::data_expression e_m = nth_element(e, m);
              std::set<data::variable> fv = data::find_free_variables(e_m);
              u.set_marking(data::detail::set_union(data::detail::set_intersection(fv, dx), u.marking()));
              mCRL2log(log::debug, "stategraph") << "  m = " << m << " freevars = " << core::detail::print_set(fv) << " dx = " << core::detail::print_set(dx) << "\n";
            }
            if (u.marking().size() > last_size)
            {
              todo.insert(&u);
              mCRL2log(log::debug, "stategraph") << "updated marking " << u.print_marking() << " using edge " << pbes_system::pp(Y) << "\n";
            }
          }
        }
#else
        for (std::set<stategraph_edge>::iterator ei = v.incoming_edges.begin(); ei != v.incoming_edges.end(); ++ei)
        {
          stategraph_vertex& u = *(ei->source);
          std::size_t i = ei->label;
          std::size_t last_size = u.marking().size();
          const stategraph_equation& eq_X = *find_equation(m_pbes, u.name());
          const propositional_variable_instantiation& Y = eq_X.predicate_variables()[i].variable();
          std::set<data::variable> dx = propvar_parameters(u.name());
          mCRL2log(log::debug, "stategraph") << "  vertex u = " << v << " label = " << i << " I = " << print_set(I) << " u.marking = " << core::detail::print_set(u.marking()) << std::endl;
          for (auto j = I.begin(); j != I.end(); ++j)
          {
            std::size_t m = *j;
            data::data_expression_list e = Y.parameters();
            data::data_expression e_m = nth_element(e, m);
            std::set<data::variable> fv = data::find_free_variables(e_m);
            u.set_marking(data::detail::set_union(data::detail::set_intersection(fv, dx), u.marking()));
            mCRL2log(log::debug, "stategraph") << "  m = " << m << " freevars = " << core::detail::print_set(fv) << " dx = " << core::detail::print_set(dx) << "\n";
          }
          if (u.marking().size() > last_size)
          {
            todo.insert(&u);
            mCRL2log(log::debug, "stategraph") << "updated marking " << u.print_marking() << " using edge " << pbes_system::pp(Y) << "\n";
          }
        }
#endif
      }

      // set the marking_parameters attributes
      for (auto i = G.begin(); i != G.end(); ++i)
      {
#ifdef MCRL2_STATEGRAPH_NEW_GLOBAL_ALGORITHM
        auto const& v = *i;
#else
        auto const& v = i->second;
#endif
        const stategraph_equation& eqn = *find_equation(m_pbes, v.name());
        const std::vector<data::variable>& d = eqn.parameters();
        for (auto j = d.begin(); j != d.end(); ++j)
        {
          v.add_marked_parameter(v.marking().find(*j) != v.marking().end());
        }
      }
    }

    // First determine whether this location should be considered
    template <typename Vertex>
    bool location_possibly_reachable(const core::identifier_string& Y, const Vertex& u, const std::vector<data::data_expression>& e_X, const stategraph_equation& eq_X,std::size_t i)
    {
      std::size_t N = e_X.size();
      data::data_expression_list::const_iterator k = u.values().begin();
      for (std::size_t j = 0; j < N; ++j)
      {
        if (is_global_control_flow_parameter(Y, j))
        {
          const predicate_variable& X_i = eq_X.predicate_variables()[i];
          std::map<std::size_t, data::data_expression>::const_iterator dest_j = X_i.target().find(j);
          if (dest_j != X_i.target().end())
          {
            data::data_expression f_k = *k;
            if(f_k != dest_j->second)
            {
              return false;
            }
          }
          ++k;
        }
      }
      return true;
    }

  public:
    // expands a propositional variable instantiation using the control flow graph
    // x = Y(e)
    // Y(e) = PVI(phi_X, i)
    pbes_expression reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i)
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variable Y(e) = " << pbes_system::pp(x) << " with index " << i << std::endl;
      assert(eq_X.predicate_variables()[i].variable() == x);

      std::vector<pbes_expression> phi;
      core::identifier_string Y = x.name();
      std::vector<data::data_expression> e(x.parameters().begin(),x.parameters().end());

      // iterate over the alternatives as defined by the control flow graph
      auto inst = m_control_flow_graph.index(Y);
      if (inst.empty())
      {
        mCRL2log(log::verbose) << "Equation " << Y << " is not reachable!" << std::endl;
      }
      for (auto q = inst.begin(); q != inst.end(); ++q)
      {
        auto& u = **q;
        mCRL2log(log::debug, "stategraph") << "  vertex u = " << u << std::endl;

        if(!location_possibly_reachable(Y, u, e, eq_X, i))
        {
          continue;
        }

        // Now build the actual formula
        std::vector<data::data_expression> r;
        std::size_t N = u.marked_parameters().size();
        assert(e.size() == u.marked_parameters().size());
        data::data_expression_list::const_iterator k = u.values().begin();
        data::data_expression condition = data::sort_bool::true_();
        for (std::size_t j = 0; j < N; ++j)
        {
          mCRL2log(log::debug, "stategraph") << "    j = " << j;
          if (is_global_control_flow_parameter(Y, j))
          {
            mCRL2log(log::debug, "stategraph") << " CFP(Y, j) = true";
            const data::data_expression& f_k = *k++;
            const predicate_variable& X_i = eq_X.predicate_variables()[i];
            if (X_i.target().find(j) == X_i.target().end() || !m_simplify)
            {
              condition = data::lazy::and_(condition, data::equal_to(e[j], f_k));
              r.push_back(e[j]);
              mCRL2log(log::debug, "stategraph") << " target(X, i, j) = false";
              mCRL2log(log::debug, "stategraph") << " c := c && " << data::pp(data::equal_to(e[j], f_k));
            }
            else
            {
              r.push_back(X_i.target().find(j)->second);
            }
            mCRL2log(log::debug, "stategraph") << " r := r <| " << data::pp(r.back());

          }
          else if (u.is_marked_parameter(j))
          {
            mCRL2log(log::debug, "stategraph") << " e[j] is in marking(u)";
            mCRL2log(log::debug, "stategraph") << " r := r <| " << data::pp(e[j]);
            r.push_back(e[j]);
          }
          else
          {
            mCRL2log(log::debug, "stategraph") << " default parameter ";
            mCRL2log(log::debug, "stategraph") << " r := r <| " << data::pp(default_value(e[j].sort()));
            r.push_back(default_value(e[j].sort()));
          }
          mCRL2log(log::debug, "stategraph") << std::endl;
        }
        propositional_variable_instantiation Yr(Y, data::data_expression_list(r.begin(),r.end()));
        if (m_simplify)
        {
          condition = m_datar(condition);
          if (condition != data::sort_bool::false_())
          {
            phi.push_back(imp(condition, Yr));
          }
        }
        else
        {
          phi.push_back(imp(condition, Yr));
        }
      }
      return pbes_expr::join_and(phi.begin(), phi.end());
    }

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes& p)
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variables to the original PBES ---" << std::endl;

      // apply the reset variable procedure to all propositional variable instantiations
      std::vector<pbes_equation>& p_eqn = p.equations();
      const std::vector<stategraph_equation>& s_eqn = m_pbes.equations();

      for (std::size_t k = 0; k < p_eqn.size(); k++)
      {

        p_eqn[k].formula() = reset_variables(*this, p_eqn[k].formula(), s_eqn[k]);
      }

      // TODO: merge the two rewriters?
      if (m_simplify)
      {
        pbes_system::simplify_data_rewriter<data::rewriter> pbesr(m_datar);
        pbes_system::pbes_rewrite(p, pbesr);
      }
    }

    global_reset_variables_algorithm(const pbes& p, const pbesstategraph_options& options)
      : stategraph_global_algorithm(p, options),
        m_original_pbes(p),
        m_simplify(options.simplify)
    {}

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes run()
    {
      super::run();
      start_timer("compute_global_control_flow_marking");
      compute_global_control_flow_marking(m_control_flow_graph);
      finish_timer("compute_global_control_flow_marking");
      mCRL2log(log::verbose) << "Computed control flow marking" << std::endl;
      mCRL2log(log::debug) <<  "--- control flow marking ---\n" << m_control_flow_graph.print_marking();
      pbes result = m_original_pbes;
      reset_variables_to_original(result);
      return result;
    }
};

/// \brief reset propositional variables
/// N.B. It is essential that this traverser uses the same traversal order as the guard_traverser.
struct reset_traverser: public pbes_expression_traverser<reset_traverser>
{
  typedef pbes_expression_traverser<reset_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  global_reset_variables_algorithm& algorithm;
  const stategraph_equation& eq_X;
  std::size_t& i;

  reset_traverser(global_reset_variables_algorithm& algorithm_, const stategraph_equation& eq_X_, std::size_t& i_)
    : algorithm(algorithm_),
      eq_X(eq_X_),
      i(i_)
  {}

  std::vector<pbes_expression> expression_stack;

  void push(const pbes_expression& x)
  {
    mCRL2log(log::debug1) << "<push>" << "\n" << x << std::endl;
    expression_stack.push_back(x);
  }

  pbes_expression& top()
  {
    return expression_stack.back();
  }

  const pbes_expression& top() const
  {
    return expression_stack.back();
  }

  pbes_expression pop()
  {
    pbes_expression result = top();
    expression_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(x);
  }

  void leave(const pbes_system::propositional_variable_instantiation& x)
  {
    pbes_expression result = algorithm.reset_variable(x, eq_X, i);
    mCRL2log(log::debug, "stategraph") << "reset variable " << pbes_system::pp(x) << " with index " << i << " to " << pbes_system::pp(result) << std::endl;
    i++;
    push(result);
  }

  void leave(const pbes_system::not_& /* x */)
  {
    pbes_expression operand = pop();
    push(not_(atermpp::deprecated_cast<atermpp::aterm_appl>(operand)));
  }

  void leave(const pbes_system::and_& /* x */)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(and_(left, right));
  }

  void leave(const pbes_system::or_& /* x */)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(or_(left, right));
  }

  void leave(const pbes_system::imp& /* x */)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(imp(left, right));
  }

  void leave(const pbes_system::forall& x)
  {
    pbes_expression operand = pop();
    push(forall(x.variables(), operand));
  }

  void leave(const pbes_system::exists& x)
  {
    pbes_expression operand = pop();
    push(exists(x.variables(), operand));
  }
};

inline
pbes_expression reset_variables(global_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X)
{
  std::size_t i = 0;
  reset_traverser f(algorithm, eq_X, i);
  f(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_GLOBAL_RESET_VARIABLES_H
