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

#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/pbes/detail/stategraph_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_algorithm.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Container>
std::string print_vector(const Container& v, const std::string& delim)
{
  std::ostringstream os;
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    if(i != v.begin())
    {
      os << delim;
    }
    os << *i;
  }
  return os.str();
}

class local_reset_variables_algorithm;
pbes_expression local_reset_variables(local_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X);

/// \brief Adds the reset variables procedure to the stategraph algorithm
class local_reset_variables_algorithm: public stategraph_local_algorithm
{
  public:
    typedef stategraph_local_algorithm super;

  protected:
    const pbes& m_original_pbes;

    // if true, the resulting PBES is simplified
    bool m_simplify;

    // m_occurring_data_parameters[X] contains the indices of data parameters that occur in at least one local control flow graph
    std::map<core::identifier_string, std::set<std::size_t> > m_occurring_data_parameters;

    // returns a default value for the given sort, that corresponds to parameter d_X[j]
    data::data_expression default_value(const core::identifier_string& X, std::size_t j, const data::sort_expression& x)
    {
      auto const& Xinit = m_pbes.initial_state();
      if (X == Xinit.name())
      {
        return nth_element(Xinit.parameters(), j);
      }

      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

    void compute_occurring_data_parameters()
    {
      m_occurring_data_parameters.clear();

      // first collect all parameters (X, p) that are being used in a local control flow graph
      for (auto k = m_local_control_flow_graphs.begin(); k != m_local_control_flow_graphs.end(); ++k)
      {
        auto const& V = k->vertices;
        for (auto j = V.begin(); j != V.end(); ++j)
        {
          auto const& u = *j;
          auto const& X = u.name();
          auto p = u.index();
          m_occurring_data_parameters[X].insert(p);
        }
      }

      // then intersect them with the data parameter indices
      for (auto i = m_occurring_data_parameters.begin(); i != m_occurring_data_parameters.end(); ++i)
      {
        auto const& X = i->first;
        auto const& eq_X = *find_equation(m_pbes, X);
        auto const& dp_X = eq_X.data_parameter_indices();
        i->second = data::detail::set_intersection(i->second, std::set<std::size_t>(dp_X.begin(), dp_X.end()));
      }
    }

  public:

    // expands a propositional variable instantiation using the control flow graph
    // x = Y(e)
    // Y(e) = PVI(phi_X, i)
    pbes_expression reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i);

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes& p)
    {
      mCRL2log(log::debug, "stategraph") << "=== resetting variables to the original PBES ---" << std::endl;

      // apply the reset variable procedure to all propositional variable instantiations
      std::vector<pbes_equation>& p_eqn = p.equations();
      const std::vector<stategraph_equation>& s_eqn = m_pbes.equations();

      for (std::size_t k = 0; k < p_eqn.size(); k++)
      {
        mCRL2log(log::debug1, "stategraph") << "--- resetting equation " << p_eqn[k] << std::endl;
        p_eqn[k].formula() = local_reset_variables(*this, p_eqn[k].formula(), s_eqn[k]);
      }

      // Commented out, since Tim thinks this should not have any effect
      // if (m_simplify)
      // {
      //   pbes_system::simplify_data_rewriter<data::rewriter> pbesr(m_datar);
      //   pbes_system::pbes_rewrite(p, pbesr);
      // }
    }

    local_reset_variables_algorithm(const pbes& p, const pbesstategraph_options& options)
      : stategraph_local_algorithm(p, options),
        m_original_pbes(p),
        m_simplify(options.simplify)
    {}

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes run()
    {
      super::run();
      pbes result = m_original_pbes;
      compute_occurring_data_parameters();

      start_timer("reset_variables_to_original");
      reset_variables_to_original(result);
      finish_timer("reset_variables_to_original");

      return result;
    }
};

/// N.B. It is essential that this traverser uses the same traversal order as the guard_traverser.
struct local_reset_traverser: public pbes_expression_traverser<local_reset_traverser>
{
  typedef pbes_expression_traverser<local_reset_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  local_reset_variables_algorithm& algorithm;
  const stategraph_equation& eq_X;
  std::size_t& i;

  local_reset_traverser(local_reset_variables_algorithm& algorithm_, const stategraph_equation& eq_X_, std::size_t& i_)
    : algorithm(algorithm_),
      eq_X(eq_X_),
      i(i_)
  {}

  std::vector<pbes_expression> expression_stack;

  void push(const pbes_expression& x)
  {
    mCRL2log(log::debug2) << "<push>" << "\n" << x << std::endl;
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
    mCRL2log(log::debug1, "stategraph") << "reset variable " << x << " with index " << i << " to " << result << std::endl;
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
pbes_expression local_reset_variables(local_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X)
{
  std::size_t i = 0;
  local_reset_traverser f(algorithm, eq_X, i);
  f.apply(x);
  return f.top();
}

pbes_expression local_reset_variables_algorithm::reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i)
{
  using utilities::detail::contains;

  // mCRL2log(log::debug, "stategraph") << "--- resetting variable Y(e) = " << x << " with index " << i << std::endl;
  assert(i < eq_X.predicate_variables().size());
  const predicate_variable& Ye = eq_X.predicate_variables()[i];
  assert(Ye.variable() == x);

  const core::identifier_string& X = eq_X.variable().name();
  const core::identifier_string& Y = Ye.name();
  const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
  auto const& e = x.parameters();
  std::vector<data::data_expression> e1(e.begin(), e.end());
  const std::vector<data::variable>& d_Y = eq_Y.parameters();
  assert(d_Y.size() == Ye.parameters().size());
  const std::size_t J = m_local_control_flow_graphs.size();

  auto const& dp_Y = eq_Y.data_parameter_indices();
  for (auto dpi = dp_Y.begin(); dpi != dp_Y.end(); ++dpi)
  {
    std::size_t k = *dpi;
    bool relevant = true;
    std::set<data::data_expression> condition;
    for (std::size_t j = 0; j < J; j++)
    {
      auto const& Vj = m_local_control_flow_graphs[j];
      auto& Bj = m_belongs[j];
      default_rules_predicate rules(Vj);
      if (rules(X, i))
      {
        auto const& v = Vj.find_vertex(Y); // v = (Y, p, q)
        std::size_t p = v.index();
        auto di = Ye.target().find(p);
        if (di != Ye.target().end())
        {
          auto const& q1 = di->second; // q1 = target(X, i, p)
          auto const& u = Vj.find_vertex(local_control_flow_graph_vertex(Y, p, data::undefined_variable(), q1));
          if (contains(Bj[Y], d_Y[k]) && !contains(u.marking(), d_Y[k]))
          {
            relevant = false;
            break;
          }
        }
        else if(!v.has_variable())
        {
          if (contains(Bj[Y], d_Y[k]) && !contains(v.marking(), d_Y[k]))
          {
            relevant = false;
            break;
          }
        }
        else
        {
          // update relevant and condition
          if (contains(Bj[Y], d_Y[k]))
          {
            bool found = false;
            for (auto wi = Vj.vertices.begin(); wi != Vj.vertices.end(); ++wi)
            {
              auto const& w = *wi;
              if (w.name() == Y && w.index() == p)  // w = (Y, p, d_Y[p]=r)
              {
                if (contains(w.marking(), d_Y[k]))
                {
                  found = true;
                }
                else
                {
                  if  (w.has_variable())
                  {
                    auto const& r = w.value();
                    condition.insert(data::equal_to(nth_element(e, p), r));
                  }
                }
              }
            }
            if (!found)
            {
              relevant = false;
              break;
            }
          }
        }
      }
    }
    if (!relevant)
    {
      e1[k] = default_value(Y, k, e1[k].sort());
    }
    else
    {
      if (!condition.empty())
      {
        e1[k] = data::if_(data::lazy::join_or(condition.begin(), condition.end()), default_value(Y, k, e1[k].sort()), nth_element(e, k));
        mCRL2log(log::debug1, "stategraph") << "  reset copy Y = " << Y << " k = " << k << " e'[k] = " << e1[k] << std::endl;
      }
    }
  }
  return propositional_variable_instantiation(Y, data::data_expression_list(e1.begin(), e1.end()));
}


} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
